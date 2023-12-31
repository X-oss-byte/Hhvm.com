/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * thrift - a lightweight cross-language rpc/serialization tool
 *
 * This file contains the main compiler engine for Thrift, which invokes the
 * scanner/parser to build the thrift object tree. The interface generation
 * code for each language lives in a file by the language name under the
 * generate/ folder, and all parse structures live in parse/
 *
 */

#include <thrift/compiler/compiler.h>

#ifdef _WIN32
#include <process.h> // @manual
#else
#include <unistd.h>
#endif
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem.hpp>

#include <thrift/compiler/ast/diagnostic_context.h>
#include <thrift/compiler/ast/t_program_bundle.h>
#include <thrift/compiler/detail/system.h>
#include <thrift/compiler/diagnostic.h>
#include <thrift/compiler/generate/t_generator.h>
#include <thrift/compiler/parse/parse_ast.h>
#include <thrift/compiler/sema/standard_mutator.h>
#include <thrift/compiler/sema/standard_validator.h>
#include <thrift/compiler/validator/validator.h>

namespace apache {
namespace thrift {
namespace compiler {
namespace {

/**
 * Flags to control code generation
 */
struct gen_params {
  bool gen_recurse = false;
  std::string genfile;
  std::vector<std::string> targets;
  std::string out_path;
  bool add_gen_dir = true;
};

/**
 * Display the usage message.
 */
void usage() {
  fprintf(stderr, "Usage: thrift [options] file\n");
  fprintf(stderr, "Options:\n");
  fprintf(
      stderr,
      "  -o dir      Set the output directory for gen-* packages\n"
      "              (default: current directory)\n");
  fprintf(
      stderr,
      "  -out dir    Set the output location for generated files\n"
      "              (no gen-* folder will be created)\n");
  fprintf(
      stderr,
      "  -I dir      Add a directory to the list of directories\n"
      "              searched for include directives\n");
  fprintf(stderr, "  -nowarn     Suppress all compiler warnings (BAD!)\n");
  fprintf(
      stderr,
      "  -legacy-strict     Strict compiler warnings on (DEPRECATED)\n");
  fprintf(stderr, "  -v[erbose]  Verbose mode\n");
  fprintf(stderr, "  -r[ecurse]  Also generate included files\n");
  fprintf(stderr, "  -debug      Parse debug trace to stdout\n");
  fprintf(
      stderr,
      "  --allow-neg-keys  Allow negative field keys (Used to preserve protocol\n"
      "                    compatibility with older .thrift files)\n");
  fprintf(
      stderr,
      "  --allow-neg-enum-vals Allow negative enum vals (DEPRECATED)\n");
  fprintf(
      stderr,
      "  --allow-64bit-consts  Do not print warnings about using 64-bit constants\n");
  fprintf(
      stderr,
      "  --gen STR   Generate code with a dynamically-registered generator.\n"
      "              STR has the form language[:key1=val1[,key2,[key3=val3]]].\n"
      "              Keys and values are options passed to the generator.\n"
      "              Many options will not require values.\n");
  fprintf(
      stderr,
      "  --record-genfiles FILE\n"
      "              Save the list of generated files to FILE,\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Available generators (and options):\n");

  for (const auto& gen : generator_registry::get_generators()) {
    fmt::print(
        stderr,
        "  {} ({}):\n{}",
        gen.second->name(),
        gen.second->long_name(),
        gen.second->documentation());
  }
}

bool isPathSeparator(const char& c) {
#ifdef _WIN32
  return c == ';';
#else
  return c == ':';
#endif
}

// Returns the input file name if successful, otherwise returns an empty
// string.
std::string parse_args(
    const std::vector<std::string>& arguments,
    parsing_params& pparams,
    gen_params& gparams,
    diagnostic_params& dparams) {
  // Check for necessary arguments, you gotta have at least a filename and
  // an output language flag.
  if (arguments.size() < 3 && gparams.targets.empty()) {
    usage();
    return {};
  }

  // A helper that grabs the next argument, if possible.
  // Outputs an error and returns nullptr if not.
  size_t arg_i = 1; // Skip the binary name.
  auto consume_arg = [&](const char* arg_name) -> const std::string* {
    // Note: The input filename must be the last argument.
    if (arg_i + 2 >= arguments.size()) {
      fprintf(
          stderr,
          "!!! Missing %s between %s and '%s'\n",
          arg_name,
          arguments[arg_i].c_str(),
          arguments[arg_i + 1].c_str());
      usage();
      return nullptr;
    }
    return &arguments[++arg_i];
  };

  // Hacky parameter handling... I didn't feel like using a library sorry!
  bool nowarn =
      false; // Guard so --nowarn and --legacy-strict are order agnostic.
  for (; arg_i < arguments.size() - 1;
       ++arg_i) { // Last argument is the src file.
    // Parse flag.
    std::string flag;
    if (arguments[arg_i].size() < 2 || arguments[arg_i][0] != '-') {
      fprintf(stderr, "!!! Expected flag, got: %s\n", arguments[arg_i].c_str());
      usage();
      return {};
    } else if (arguments[arg_i][1] == '-') {
      flag = arguments[arg_i].substr(2);
    } else {
      flag = arguments[arg_i].substr(1);
    }

    // Interpret flag.
    if (flag == "debug") {
      dparams.debug = true;
    } else if (flag == "nowarn") {
      dparams.warn_level = 0;
      nowarn = true;
    } else if (flag == "legacy-strict") {
      pparams.strict = 255;
      if (!nowarn) { // Don't override nowarn.
        dparams.warn_level = 2;
      }
    } else if (flag == "v" || flag == "verbose") {
      dparams.info = true;
    } else if (flag == "r" || flag == "recurse") {
      gparams.gen_recurse = true;
    } else if (flag == "allow-neg-keys") {
      pparams.allow_neg_field_keys = true;
    } else if (flag == "allow-neg-enum-vals") {
      // no-op
    } else if (flag == "allow-64bit-consts") {
      pparams.allow_64bit_consts = true;
    } else if (flag == "record-genfiles") {
      auto* arg = consume_arg("genfile file specification");
      if (arg == nullptr) {
        return {};
      }
      gparams.genfile = *arg;
    } else if (flag == "gen") {
      auto* arg = consume_arg("generator specification");
      if (arg == nullptr) {
        return {};
      }
      gparams.targets.push_back(*arg);
    } else if (flag == "I") {
      auto* arg = consume_arg("include directory");
      if (arg == nullptr) {
        return {};
      }
      // An argument of "-I\ asdf" is invalid and has unknown results
      pparams.incl_searchpath.push_back(*arg);
    } else if (flag == "o" || flag == "out") {
      auto* arg = consume_arg("output directory");
      if (arg == nullptr) {
        return {};
      }
      std::string out_path = *arg;
      bool add_gen_dir = (flag == "o");

      // Strip out trailing \ on a Windows path
      if (detail::platform_is_windows()) {
        int last = out_path.length() - 1;
        if (out_path[last] == '\\') {
          out_path.erase(last);
        }
      }

      if (!add_gen_dir) {
        // Invoker specified `-out blah`. We are supposed to output directly
        // into blah, e.g. `blah/Foo.java`. Make the directory if necessary,
        // just like how for `-o blah` we make `o/gen-java`
        boost::system::error_code errc;
        boost::filesystem::create_directory(out_path, errc);
        if (errc) {
          fprintf(
              stderr,
              "Output path %s is unusable or not a directory\n",
              out_path.c_str());
          return {};
        }
      }
      if (!boost::filesystem::is_directory(out_path)) {
        fprintf(
            stderr,
            "Output path %s is unusable or not a directory\n",
            out_path.c_str());
        return {};
      }
      gparams.out_path = std::move(out_path);
      gparams.add_gen_dir = add_gen_dir;
    } else {
      fprintf(
          stderr, "!!! Unrecognized option: %s\n", arguments[arg_i].c_str());
      usage();
      return {};
    }
  }

  if (const char* env_p = std::getenv("THRIFT_INCLUDE_PATH")) {
    std::vector<std::string> components;
    boost::algorithm::split(components, env_p, isPathSeparator);
    pparams.incl_searchpath.insert(
        pparams.incl_searchpath.end(), components.begin(), components.end());
  }

  // You gotta generate something!
  if (gparams.targets.empty()) {
    fprintf(stderr, "!!! No output language(s) specified\n\n");
    usage();
    return {};
  }

  // Return the input file name.
  assert(arg_i == arguments.size() - 1);
  return arguments[arg_i];
}

enum class parse_control : bool { stop, more };

// `callback(key, value)` will be called for each key=value generator's option.
// If there is no value, `value` will be empty string.
void parse_generator_options(
    const std::string& option_string,
    std::function<parse_control(std::string, std::string)> callback) {
  std::vector<std::string> parts;
  bool inside_braces = false;
  boost::algorithm::split(parts, option_string, [&inside_braces](char c) {
    if (c == '{' || c == '}') {
      inside_braces = (c == '{');
    }
    return c == ',' && !inside_braces;
  });
  for (const auto& part : parts) {
    auto key = part.substr(0, part.find('='));
    auto value = part.substr(std::min(key.size() + 1, part.size()));
    if (callback(std::move(key), std::move(value)) == parse_control::stop) {
      break;
    }
  }
}

struct generator_specs {
  std::string generator_name;
  std::map<std::string, std::string> generator_options;
};

/**
 * Parses the given target specification string.
 *
 * For example, if `target == "ast:protocol=json"`, the return value would be:
 *
 * ```
 * { "ast", { "protocol": "json" } }
 * ```
 */
generator_specs parse_generator_specs(const std::string& target) {
  const auto colon_pos = target.find(':');
  std::string name = target.substr(0, colon_pos);

  std::map<std::string, std::string> options;
  parse_generator_options(
      target.substr(colon_pos + 1), [&](std::string k, std::string v) {
        options.emplace(std::move(k), std::move(v));
        return parse_control::more;
      });
  return {std::move(name), std::move(options)};
}

/**
 * Creates and processes options for a generator based on the given `target`
 * specification string.
 *
 * If a generator cannot be successfully created with the given parameters, an
 * error message is printed to stderr and this method returns `nullptr`.
 *
 * @param target specification string, eg "ast:protocol=json".
 *
 * @return successfully created generator, or `nullptr` on failure.
 *
 * @throws std::exception if the generator options could not be processed (see
 * `t_generator::process_options()`).
 */
std::unique_ptr<t_generator> create_generator(
    const std::string& target,
    const gen_params& params,
    t_program& program,
    t_program_bundle& program_bundle,
    source_manager& source_mgr) {
  const auto [generator_name, generator_options] =
      parse_generator_specs(target);
  std::unique_ptr<t_generator> generator = generator_registry::make_generator(
      generator_name, program, source_mgr, program_bundle);
  if (generator == nullptr) {
    fmt::print(stderr, "Error: Invalid generator name: {}\n", generator_name);
    usage();
    return nullptr;
  }
  generator->process_options(
      generator_options, params.out_path, params.add_gen_dir);
  return generator;
}

/**
 * Validates the program currently being generated by `generator`.
 *
 * The validators to run are specified by the `generator` (see
 * `t_generator::fill_validator_list()`).
 *
 * @return true iff no validator reports an error (i.e.,
 * `diagnostic_level::error`.
 */
bool validate_program(t_generator& generator, diagnostics_engine& diags) {
  bool success = true;
  diagnostics_engine validator_diags(
      diags.source_mgr(),
      [&](diagnostic d) {
        if (d.level() == diagnostic_level::error) {
          success = false;
        }
        diags.report(std::move(d));
      },
      diags.params());
  validator_list validators(validator_diags);
  generator.fill_validator_list(validators);
  validators.traverse(generator.get_program());
  return success;
}

/**
 * Validates and generates the output for the given `program`.
 *
 * @return true iff generation was successful for all targets in `params`
 */
bool generate_code_for_single_program(
    const gen_params& params,
    t_program& program,
    t_program_bundle& program_bundle,
    diagnostics_engine& diags) {
  try {
    bool all_targets_successful = true;

    std::ofstream genfile_list_out;
    if (!params.genfile.empty()) {
      genfile_list_out.open(params.genfile);
    }

    for (const std::string& target : params.targets) {
      std::unique_ptr<t_generator> generator = create_generator(
          target, params, program, program_bundle, diags.source_mgr());
      if (generator == nullptr) {
        continue;
      }

      if (!validate_program(*generator, diags)) {
        all_targets_successful = false;
        continue;
      }

      diags.report(
          source_location(),
          diagnostic_level::info,
          "generating \"{}\"",
          target);
      generator->generate_program();

      if (genfile_list_out.is_open()) {
        for (const std::string& s : generator->get_genfiles()) {
          genfile_list_out << s << "\n";
        }
      }
    }
    return all_targets_successful;
  } catch (const std::string& s) {
    printf("Error: %s\n", s.c_str());
    return false;
  } catch (const char* exc) {
    printf("Error: %s\n", exc);
    return false;
  }
}

/**
 * Validates and generates the output for the given `program` and all its
 * (transitive) dependencies.
 *
 * @return true iff the (validation and) generation was successful for all
 * (transitive) dependencies of `program` and target generators.
 */
bool generate_code_recursively(
    const gen_params& params,
    t_program& program,
    std::set<std::string>& already_generated,
    t_program_bundle& program_bundle,
    diagnostics_engine& diags) {
  // Add the path we are about to generate.
  [[maybe_unused]] const auto [_, inserted] =
      already_generated.emplace(program.path());
  assert(inserted); // Was not already generated

  for (t_program* included_program : program.get_included_programs()) {
    if (already_generated.count(included_program->path()) > 0) {
      continue;
    }
    if (!generate_code_recursively(
            params,
            *included_program,
            already_generated,
            program_bundle,
            diags)) {
      return false;
    }
  }

  return generate_code_for_single_program(
      params, program, program_bundle, diags);
}

/**
 * Validates and generates the output for the given `program_bundle`.
 *
 * Depending on `gparams.gen_recurse`, this will either generate the output for
 * the root program in the bundle, or for all programs (i.e., the root program
 * and all of its transitive dependencies).
 *
 * @return `compile_retcode::success` if validation and generation of all
 * requested programs (see above) was successful (for all generator targets
 * specified in `gparams`). Otherwise, `compile_retcode::failure`.
 */
compile_retcode generate_code_for_program_bundle(
    t_program_bundle& program_bundle,
    const gen_params& gparams,
    diagnostic_context& ctx) {
  compile_retcode retcode = compile_retcode::failure;

  ctx.begin_visit(*program_bundle.root_program());
  try {
    if (gparams.gen_recurse) {
      std::set<std::string> already_generated;
      if (generate_code_recursively(
              gparams,
              *program_bundle.get_root_program(),
              already_generated,
              program_bundle,
              ctx)) {
        retcode = compile_retcode::success;
      }
    } else {
      if (generate_code_for_single_program(
              gparams,
              *program_bundle.get_root_program(),
              program_bundle,
              ctx)) {
        retcode = compile_retcode::success;
      }
    }
  } catch (const std::exception& e) {
    ctx.error(*program_bundle.root_program(), "{}", e.what());
  }
  ctx.end_visit(*program_bundle.root_program());
  return retcode;
}

std::string get_include_path(
    const std::vector<std::string>& generator_targets,
    const std::string& input_filename) {
  std::string include_prefix;
  for (const auto& target : generator_targets) {
    const auto colon_pos = target.find(':');
    if (colon_pos == std::string::npos) {
      continue;
    }
    const auto lang_name = target.substr(0, colon_pos);
    if (lang_name != "cpp2" && lang_name != "mstch_cpp2") {
      continue;
    }

    const auto lang_args = target.substr(colon_pos + 1);
    parse_generator_options(lang_args, [&](std::string k, std::string v) {
      if (k.find("include_prefix") != std::string::npos) {
        include_prefix = std::move(v);
        return parse_control::stop;
      }
      return parse_control::more;
    });
  }

  // infer cpp include prefix from the filename passed in if none specified.
  if (include_prefix == "") {
    if (input_filename.rfind('/') != std::string::npos) {
      include_prefix = input_filename.substr(0, input_filename.rfind('/'));
    }
  }

  return include_prefix;
}

/**
 * Parses and validates the given Thrift file.
 *
 * @returns The AST root of the t_program_bundle corresponding to
 * `input_filename`, or nullptr on error.
 */
std::unique_ptr<t_program_bundle> parse_and_mutate(
    source_manager& source_mgr,
    diagnostic_context& ctx,
    const std::string& input_filename,
    const parsing_params& pparams,
    const gen_params& gparams) {
  // Parse it!
  std::unique_ptr<t_program_bundle> program_bundle = parse_and_mutate_program(
      source_mgr,
      ctx,
      input_filename,
      pparams,
      true /* return_nullptr_on_failure */);
  if (program_bundle == nullptr) {
    return nullptr;
  }

  // Load standard library if available.
  static const std::string kSchemaPath = "thrift/lib/thrift/schema.thrift";
  auto found_or_error =
      source_mgr.find_include_file(kSchemaPath, "", pparams.incl_searchpath);
  if (found_or_error.index() == 0) {
    // Found
    if (!program_bundle->find_program(kSchemaPath)) {
      diagnostic_context stdlib_ctx(
          source_mgr,
          [&](diagnostic&& d) {
            ctx.warning(
                source_location{},
                "Could not load Thrift standard libraries: {}",
                d.str());
          },
          diagnostic_params::only_errors());
      std::unique_ptr<t_program_bundle> inc = parse_and_mutate_program(
          source_mgr,
          stdlib_ctx,
          kSchemaPath,
          pparams,
          true /* return_nullptr_on_failure */,
          program_bundle.get());
      if (inc) {
        program_bundle->add_implicit_includes(std::move(inc));
      }
    }
  } else {
    // Not found
    ctx.warning(
        source_location{},
        "Could not load Thrift standard libraries: {}",
        std::get<1>(found_or_error));
  }

  program_bundle->root_program()->set_include_prefix(
      get_include_path(gparams.targets, input_filename));

  // Validate it!
  validator::validate(program_bundle->root_program(), ctx);
  standard_validator()(ctx, *program_bundle->root_program());
  return ctx.has_errors() ? nullptr : std::move(program_bundle);
}

} // namespace

std::unique_ptr<t_program_bundle> parse_and_mutate_program(
    source_manager& sm,
    diagnostic_context& ctx,
    const std::string& filename,
    parsing_params params,
    bool return_nullptr_on_failure,
    t_program_bundle* already_parsed) {
  auto programs =
      parse_ast(sm, ctx, filename, std::move(params), already_parsed);
  if (!programs || ctx.has_errors()) {
    // Mutations should be only performed on a valid AST.
    return !return_nullptr_on_failure ? std::move(programs) : nullptr;
  }
  auto result = standard_mutators()(ctx, *programs);
  if (result.unresolvable_typeref && return_nullptr_on_failure) {
    // Stop processing if there is unresolvable typeref.
    programs = nullptr;
  }
  return programs;
}

std::pair<std::unique_ptr<t_program_bundle>, diagnostic_results>
parse_and_mutate_program(
    source_manager& sm,
    const std::string& filename,
    parsing_params params,
    diagnostic_params dparams) {
  diagnostic_results results;
  diagnostic_context ctx(sm, results, std::move(dparams));
  return {
      parse_and_mutate_program(sm, ctx, filename, std::move(params)), results};
}

std::unique_ptr<t_program_bundle> parse_and_dump_diagnostics(
    const std::string& filename,
    source_manager& sm,
    parsing_params pparams,
    diagnostic_params dparams) {
  diagnostic_results results;
  diagnostic_context ctx(sm, results, std::move(dparams));
  auto program =
      parse_and_mutate_program(sm, ctx, filename, std::move(pparams));
  for (const auto& diag : results.diagnostics()) {
    std::cerr << diag << "\n";
  }
  return program;
}

std::unique_ptr<t_program_bundle> parse_and_get_program(
    source_manager& sm, const std::vector<std::string>& arguments) {
  // Parse arguments.
  parsing_params pparams;
  pparams.allow_missing_includes = true;
  gen_params gparams;
  diagnostic_params dparams;
  gparams.targets.push_back(""); // Avoid needing to pass --gen
  std::string filename = parse_args(arguments, pparams, gparams, dparams);
  if (filename.empty()) {
    return {};
  }

  diagnostic_context ctx(
      sm,
      [](const diagnostic& d) { fmt::print(stderr, "{}\n", d.str()); },
      diagnostic_params::only_errors());
  return parse_ast(sm, ctx, filename, std::move(pparams));
}

compile_result compile(
    const std::vector<std::string>& arguments, source_manager& source_mgr) {
  compile_result result;

  // Parse arguments.
  parsing_params pparams;
  gen_params gparams;
  diagnostic_params dparams;
  std::string input_filename = parse_args(arguments, pparams, gparams, dparams);
  if (input_filename.empty()) {
    return result;
  }
  diagnostic_context ctx(source_mgr, result.detail, std::move(dparams));

  std::unique_ptr<t_program_bundle> program_bundle =
      parse_and_mutate(source_mgr, ctx, input_filename, pparams, gparams);
  if (program_bundle == nullptr) {
    return result;
  }

  result.retcode =
      generate_code_for_program_bundle(*program_bundle, gparams, ctx);
  return result;
}

} // namespace compiler
} // namespace thrift
} // namespace apache
