(*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

type t = Config_file_common.t

type version_components = Config_file_version.version_components = {
  major: int;
  minor: int;
  build: int;
}

type version = Config_file_version.version =
  | Opaque_version of string option
  | Version_components of version_components

val version_to_string_opt : ?pad:bool -> version -> string option

val file_path_relative_to_repo_root : string

val compare_versions : version -> version -> int

val parse_version : string option -> version

val empty : unit -> t

val parse_hhconfig : silent:bool -> string -> string * t

val parse_local_config : silent:bool -> string -> t

val apply_overrides : silent:bool -> config:t -> overrides:t -> t

val of_list : (string * string) list -> t

val keys : t -> string list

module Getters : sig
  val make_key : string -> prefix:string option -> string

  val string_opt : string -> ?prefix:string option -> t -> string option

  val string_ : string -> ?prefix:string option -> default:string -> t -> string

  val int_ : string -> ?prefix:string option -> default:int -> t -> int

  val int_opt : string -> ?prefix:string option -> t -> int option

  val float_ : string -> ?prefix:string option -> default:float -> t -> float

  val float_opt : string -> ?prefix:string option -> t -> float option

  val bool_ : string -> ?prefix:string option -> default:bool -> t -> bool

  val bool_opt : string -> ?prefix:string option -> t -> bool option

  val string_list_opt :
    string -> ?prefix:string option -> t -> string list option

  val string_list :
    delim:Str.regexp ->
    string ->
    ?prefix:string option ->
    default:string list ->
    t ->
    string list

  val bool_if_min_version :
    string ->
    ?prefix:string option ->
    default:bool ->
    current_version:Config_file_version.version ->
    t ->
    bool
end

module Utils : sig
  val parse_hhconfig_and_hh_conf_to_json :
    root:Path.t -> server_local_config_path:string -> Hh_json.json
end
