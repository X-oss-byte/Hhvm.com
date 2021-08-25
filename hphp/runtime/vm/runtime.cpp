/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/
#include "hphp/runtime/vm/runtime.h"
#include "hphp/runtime/base/execution-context.h"
#include "hphp/runtime/base/coeffects-config.h"
#include "hphp/runtime/server/source-root-info.h"
#include "hphp/runtime/base/zend-string.h"
#include "hphp/runtime/base/builtin-functions.h"
#include "hphp/runtime/base/request-info.h"
#include "hphp/runtime/base/unit-cache.h"
#include "hphp/runtime/base/vanilla-dict.h"
#include "hphp/runtime/ext/std/ext_std_closure.h"
#include "hphp/runtime/ext/generator/ext_generator.h"
#include "hphp/runtime/vm/bytecode.h"
#include "hphp/util/trace.h"
#include "hphp/util/text-util.h"
#include "hphp/runtime/vm/jit/translator-inline.h"
#include "hphp/runtime/base/zend-functions.h"
#include "hphp/runtime/ext/string/ext_string.h"

#include <folly/tracing/StaticTracepoint.h>
#include <folly/Random.h>

namespace HPHP {

TRACE_SET_MOD(runtime);

/**
 * print_string will decRef the string
 */
void print_string(StringData* s) {
  g_context->write(s->data(), s->size());
  TRACE(2, "t-x64 output(str): (%p) %43s\n", s->data(),
        escapeStringForCPP(s->data(), s->size()).data());
  decRefStr(s);
}

void print_int(int64_t i) {
  char intbuf[21];
  auto const s = conv_10(i, intbuf + sizeof(intbuf));

  g_context->write(s.data(), s.size());
}

void print_boolean(bool val) {
  if (val) {
    g_context->write("1", 1);
  }
}

/**
 * concat_ss will will incRef the output string
 * and decref its first argument
 */
StringData* concat_ss(StringData* v1, StringData* v2) {
  if (v1->cowCheck()) {
    FOLLY_SDT(hhvm, hhvm_cow_concat, v1->size(), v2->size());
    StringData* ret = StringData::Make(v1, v2);
    // Because v1 was shared, we know this won't release the string.
    v1->decRefCount();
    return ret;
  }

  auto const rhs = v2->slice();
  UNUSED auto const lsize = v1->size();
  FOLLY_SDT(hhvm, hhvm_mut_concat, lsize, rhs.size());
  auto const ret = v1->append(rhs);
  if (UNLIKELY(ret != v1)) {
    // had to realloc even though count==1
    assertx(v1->hasExactlyOneRef());
    v1->release();
  }
  return ret;
}

/**
 * concat_is will incRef the output string
 */
StringData* concat_is(int64_t v1, StringData* v2) {
  char intbuf[21];
  // Convert the int to a string
  auto const s1 = conv_10(v1, intbuf + sizeof(intbuf));
  auto const s2 = v2->slice();
  return StringData::Make(s1, s2);
}

/**
 * concat_si will incRef the output string
 * and decref its first argument
 */
StringData* concat_si(StringData* v1, int64_t v2) {
  char intbuf[21];
  auto const s2 = conv_10(v2, intbuf + sizeof(intbuf));
  if (v1->cowCheck()) {
    auto const s1 = v1->slice();
    FOLLY_SDT(hhvm, hhvm_cow_concat, s1.size(), s2.size());
    auto const ret = StringData::Make(s1, s2);
    // Because v1 was shared, we know this won't release it.
    v1->decRefCount();
    return ret;
  }

  UNUSED auto const lsize = v1->size();
  FOLLY_SDT(hhvm, hhvm_mut_concat, lsize, s2.size());
  auto const ret = v1->append(s2);
  if (UNLIKELY(ret != v1)) {
    // had to realloc even though count==1
    assertx(v1->hasExactlyOneRef());
    v1->release();
  }
  return ret;
}

StringData* concat_s3(StringData* v1, StringData* v2, StringData* v3) {
  if (v1->cowCheck()) {
    auto s1 = v1->slice();
    auto s2 = v2->slice();
    auto s3 = v3->slice();
    FOLLY_SDT(hhvm, hhvm_cow_concat, s1.size(), s2.size() + s3.size());
    StringData* ret = StringData::Make(s1, s2, s3);
    // Because v1 was shared, we know this won't release it.
    v1->decRefCount();
    return ret;
  }

  UNUSED auto const lsize = v1->size();
  FOLLY_SDT(hhvm, hhvm_mut_concat, lsize, v2->size() + v3->size());
  auto const ret = v1->append(v2->slice(), v3->slice());
  if (UNLIKELY(ret != v1)) {
    // had to realloc even though count==1
    assertx(v1->hasExactlyOneRef());
    v1->release();
  }
  return ret;
}

StringData* concat_s4(StringData* v1, StringData* v2,
                      StringData* v3, StringData* v4) {
  if (v1->cowCheck()) {
    auto s1 = v1->slice();
    auto s2 = v2->slice();
    auto s3 = v3->slice();
    auto s4 = v4->slice();
    FOLLY_SDT(hhvm, hhvm_cow_concat, s1.size(),
              s2.size() + s3.size() + s4.size());
    StringData* ret = StringData::Make(s1, s2, s3, s4);
    // Because v1 was shared, we know this won't release it.
    v1->decRefCount();
    return ret;
  }

  UNUSED auto const lsize = v1->size();
  FOLLY_SDT(hhvm, hhvm_mut_concat, lsize,
            v2->size() + v3->size() + v4->size());
  auto const ret = v1->append(v2->slice(), v3->slice(), v4->slice());
  if (UNLIKELY(ret != v1)) {
    // had to realloc even though count==1
    assertx(v1->hasExactlyOneRef());
    v1->release();
  }
  return ret;
}

void raiseWarning(const StringData* sd) {
  raise_warning("%s", sd->data());
}

void raiseNotice(const StringData* sd) {
  raise_notice("%s", sd->data());
}

void throwArrayIndexException(const ArrayData* ad, const int64_t index) {
  throwOOBArrayKeyException(index, ad);
}

void throwArrayKeyException(const ArrayData* ad, const StringData* key) {
  assertx(ad->isDictType());
  throwOOBArrayKeyException(key, ad);
}

void throwMustBeEnclosedinReadonly(const Class* cls, const StringData* propName) {
  throw_must_be_enclosed_in_readonly(cls->name()->data(), propName->data());
}

void throwMustBeReadonlyException(const Class* cls, const StringData* propName) {
  throw_must_be_readonly(cls->name()->data(), propName->data());
}

void throwMustBeMutableException(const Class* cls, const StringData* propName) {
  throw_must_be_mutable(cls->name()->data(), propName->data());
}

void throwMustBeValueTypeException(const StringData* locName) {
  throw_local_must_be_value_type(locName->data());
}

std::string formatParamInOutMismatch(const char* fname, uint32_t index,
                                   bool funcByRef) {
  if (funcByRef) {
    return folly::sformat(
      "{}() expects parameter {} to be inout, but the call was "
      "not annotated with 'inout'", fname, index + 1
    );
  } else {
    return folly::sformat(
      "{}() does not expect parameter {} to be inout, but the call was "
      "annotated with 'inout'", fname, index + 1
    );
  }
}

void throwParamInOutMismatch(const Func* func, uint32_t index) {
  SystemLib::throwInvalidArgumentExceptionObject(formatParamInOutMismatch(
    func->fullName()->data(), index, func->isInOut(index)));
}

void checkInOutMismatch(const Func* func, uint32_t numArgs,
                        const uint8_t* inoutArgs) {
  assertx(numArgs == 0 || inoutArgs != nullptr);
  uint8_t tmp = 0;
  for (auto i = 0; i < numArgs; ++i) {
    tmp = (i % 8) == 0 ? *(inoutArgs++) : tmp >> 1;
    if (func->isInOut(i) != (tmp & 1)) throwParamInOutMismatch(func, i);
  }
}

std::string formatParamReadonlyMismatch(const char* fname, uint32_t index) {
  return folly::sformat(
    "{}() does not allow for parameter {} to be passed readonly, but the argument "
    "was annotated with 'readonly'", fname, index + 1
  );
}

std::string formatReturnReadonlyMismatch(const char* fname) {
  return folly::sformat(
    "{}() returns readonly, but the caller expects a mutable value",
    fname
  );
}

void throwParamReadonlyMismatch(const Func* func, int32_t index) {
  auto const msg = [&] {
    if (index == kInvalidId) {
      return formatReturnReadonlyMismatch(func->fullName()->data());
    }
    return formatParamReadonlyMismatch(func->fullName()->data(), index);
  };
  if (RO::EvalEnableReadonlyCallEnforcement == 1) {
    raise_warning(msg());
  } else if (RO::EvalEnableReadonlyCallEnforcement > 1) {
    SystemLib::throwInvalidArgumentExceptionObject(msg());
  }
}

void checkReadonlyMismatch(const Func* func, uint32_t numArgs,
                           const uint8_t* readonlyArgs) {
  assertx(numArgs == 0 || readonlyArgs != nullptr);
  uint8_t tmp = 0;
  for (auto i = 0; i < numArgs; ++i) {
    tmp = (i % 8) == 0 ? *(readonlyArgs++) : tmp >> 1;
    if ((tmp & 1) && !func->isReadonly(i)) throwParamReadonlyMismatch(func, i);
  }
}

void throwInvalidUnpackArgs() {
  SystemLib::throwInvalidArgumentExceptionObject(
    "Only containers may be unpacked");
}

namespace {

std::string formatArgumentErrMsg(const Func* func, const char* amount,
                                 uint32_t expected, uint32_t got) {
  return folly::sformat(
    "{}() expects {} {} parameter{}, {} given",
    func->fullNameWithClosureName(),
    amount,
    expected,
    expected == 1 ? "" : "s",
    got
  );
}

}

void throwMissingArgument(const Func* func, int got) {
  auto const expected = func->numRequiredParams();
  assertx(got < expected);
  auto const amount = expected < func->numParams() ? "at least" : "exactly";
  auto const errMsg = formatArgumentErrMsg(func, amount, expected, got);
  SystemLib::throwRuntimeExceptionObject(Variant(errMsg));
}

void raiseTooManyArguments(const Func* func, int got) {
  assertx(!func->hasVariadicCaptureParam());

  if (!RuntimeOption::EvalWarnOnTooManyArguments && !func->isCPPBuiltin()) {
    return;
  }

  auto const total = func->numNonVariadicParams();
  assertx(got > total);
  auto const amount = func->numRequiredParams() < total ? "at most" : "exactly";
  auto const errMsg = formatArgumentErrMsg(func, amount, total, got);

  if (RuntimeOption::EvalWarnOnTooManyArguments > 1 || func->isCPPBuiltin()) {
    SystemLib::throwRuntimeExceptionObject(Variant(errMsg));
  } else {
    raise_warning(errMsg);
  }
}

void raiseTooManyArgumentsPrologue(const Func* func, ArrayData* unpackArgs) {
  SCOPE_EXIT { decRefArr(unpackArgs); };
  if (unpackArgs->empty()) return;
  auto const got = func->numNonVariadicParams() + unpackArgs->size();
  raiseTooManyArguments(func, got);
}

//////////////////////////////////////////////////////////////////////

void raiseCoeffectsCallViolation(const Func* callee,
                                 RuntimeCoeffects provided,
                                 RuntimeCoeffects required) {
  assertx(CoeffectsConfig::enabled());
  auto const callerName = [&] {
    VMRegAnchor _;
    if (!vmfp()) {
      // VM is entering to the first frame
      return String{makeStaticString("[vm-entry]")};
    }
    String result;
    walkStack([&] (const ActRec* fp, Offset) {
      assertx(fp);
      auto const func = fp->func();
      assertx(func);
      if (func->hasCoeffectRules() &&
          func->getCoeffectRules().size() == 1 &&
          func->getCoeffectRules()[0].isCaller()) {
        return false; // keep going
      }
      result = func->fullNameWithClosureName();
      return true;
    });
    assertx(!result.isNull());
    return result;
  }();

  auto const errMsg = folly::sformat(
    "Call to {}() requires [{}] coeffects but {}() provided [{}]",
    callee->fullNameWithClosureName(),
    required.toString(),
    callerName,
    provided.toString()
  );

  FTRACE_MOD(Trace::coeffects, 1, "{}\n {:016b} -> {:016b}\n",
             errMsg, provided.value(), required.value());

  assertx(!provided.canCall(required));
  if (provided.canCallWithWarning(required)) {
    auto const coinflip = []{
      auto const rate = RO::EvalCoeffectViolationWarningSampleRate;
      return rate > 0 && folly::Random::rand32(rate) == 0;
    }();
    if (!coinflip) return;
    raise_warning(errMsg);
  } else {
    SystemLib::throwBadMethodCallExceptionObject(errMsg);
  }
}

void raiseCoeffectsFunParamTypeViolation(TypedValue tv,
                                         int32_t paramIdx) {
  auto const errMsg =
    folly::sformat("Coeffect rule requires parameter at position {} to be a "
                   "closure object, function/method pointer or null but "
                   "{} given",
                   paramIdx + 1, describe_actual_type(&tv));
  raise_warning(errMsg);
}

void raiseCoeffectsFunParamCoeffectRulesViolation(const Func* f) {
  assertx(f);
  auto const errMsg =
    folly::sformat("Function/method pointer to {}() contains polymorphic "
                   "coeffects but is used as a coeffect rule",
                   f->fullNameWithClosureName());
  raise_warning(errMsg);
}

//////////////////////////////////////////////////////////////////////

int64_t zero_error_level() {
  auto& id = RequestInfo::s_requestInfo.getNoCheck()->m_reqInjectionData;
  auto level = id.getErrorReportingLevel();
  id.setErrorReportingLevel(0);
  return level;
}

void restore_error_level(int64_t oldLevel) {
  auto& id = RequestInfo::s_requestInfo.getNoCheck()->m_reqInjectionData;
  if (id.getErrorReportingLevel() == 0) {
    id.setErrorReportingLevel(oldLevel);
  }
}

//////////////////////////////////////////////////////////////////////

}
