(*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(** The relationship between atomic elements in the set *)
module Set_relation : sig
  type t =
    | Equal
    | Subset
    | Superset
    | Disjoint
    | Unknown
end

(** Specifies the domain that the set will range over. Individual
    members of the domain can be related to one another via the [relation]
    function. *)
module type DomainType = sig
  (** Type of atomic elements of the domain *)
  type 'a t

  (** Contextual data that will be provided to [relation] when determining
      [set_relation] between atomic elements of the domain *)
  type ctx

  (** Determines between atomic elements of the domain based on the given context *)
  val relation : 'a t -> ctx:ctx -> 'a t -> Set_relation.t
end

(** An abstract representation of a set, designed specifically to determine if two
    sets are disjoint. Sets consist of atomic elements ([elt]) that can be joined
    using basic set operations like union, intersection and set difference. This is
    only an approximate set, because there are ways to construct sets where it isn't
    possible to determine definitively whether or not they are disjoint. In those cases
    the implementation will default to returning [Unsat]*)
module type S = sig
  (** The domain the set ranges over *)
  module Domain : DomainType

  (** Type of an instance of the approximate set *)
  type 'a t

  val empty : 'a t

  (** Create an approximate set from an atomic element in the domain *)
  val singleton : 'a Domain.t -> 'a t

  (** Set union *)
  val union : 'a t -> 'a t -> 'a t

  (** Set intersection *)
  val inter : 'a t -> 'a t -> 'a t

  (** Set difference. Note that in some cases we cannot determine precisely if
      two sets involving [diff] are disjoint. In these cases the result will
      be approximated to [Unsat] *)
  val diff : 'a t -> 'a t -> 'a t

  val of_list : 'a Domain.t list -> 'a t

  (** The result of testing two sets for disjointness *)
  type 'a disjoint =
    | Sat  (** The two sets are definitely disjoint *)
    | Unsat of {
        left: 'a Domain.t;
        relation: Set_relation.t;
        right: 'a Domain.t;
      }
        (** The two sets are not disjoint because of the relation between
            the given pair of [Domain.t]s *)

  (** Determines if a pair of sets are disjoint in the given [ctx].
      If the sets cannot definitively be proven to be disjoint, will return
      [Unsat] *)
  val disjoint : Domain.ctx -> 'a t -> 'a t -> 'a disjoint

  val are_disjoint : Domain.ctx -> 'a t -> 'a t -> bool
end

(** Constructs an approximate set representation over the given [Domain] *)
module Make (Domain : DomainType) : S with module Domain := Domain
