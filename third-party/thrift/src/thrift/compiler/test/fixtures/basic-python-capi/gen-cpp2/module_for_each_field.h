/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/basic-python-capi/src/module.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */
#pragma once

#include "thrift/compiler/test/fixtures/basic-python-capi/gen-cpp2/module_metadata.h"
#include <thrift/lib/cpp2/visitation/for_each.h>

namespace apache {
namespace thrift {
namespace detail {

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).inty_ref()...);
    f(1, static_cast<T&&>(t).stringy_ref()...);
    f(2, static_cast<T&&>(t).myItemy_ref()...);
    f(3, static_cast<T&&>(t).myEnumy_ref()...);
    f(4, static_cast<T&&>(t).boulet_ref()...);
    f(5, static_cast<T&&>(t).floatListy_ref()...);
    f(6, static_cast<T&&>(t).strMappy_ref()...);
    f(7, static_cast<T&&>(t).intSetty_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyDataItem> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).s_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::TransitiveDoubler> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::detail::DoubledPair> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).s_ref()...);
    f(1, static_cast<T&&>(t).x_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::StringPair> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).normal_ref()...);
    f(1, static_cast<T&&>(t).doubled_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::VapidStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::PrimitiveStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).booly_ref()...);
    f(1, static_cast<T&&>(t).charry_ref()...);
    f(2, static_cast<T&&>(t).shortay_ref()...);
    f(3, static_cast<T&&>(t).inty_ref()...);
    f(4, static_cast<T&&>(t).longy_ref()...);
    f(5, static_cast<T&&>(t).floaty_ref()...);
    f(6, static_cast<T&&>(t).dubby_ref()...);
    f(7, static_cast<T&&>(t).stringy_ref()...);
    f(8, static_cast<T&&>(t).bytey_ref()...);
    f(9, static_cast<T&&>(t).buffy_ref()...);
    f(10, static_cast<T&&>(t).pointbuffy_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::ListStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).boolz_ref()...);
    f(1, static_cast<T&&>(t).intz_ref()...);
    f(2, static_cast<T&&>(t).stringz_ref()...);
    f(3, static_cast<T&&>(t).encoded_ref()...);
    f(4, static_cast<T&&>(t).uidz_ref()...);
    f(5, static_cast<T&&>(t).matrix_ref()...);
    f(6, static_cast<T&&>(t).ucharz_ref()...);
    f(7, static_cast<T&&>(t).voxels_ref()...);
    f(8, static_cast<T&&>(t).buf_ptrs_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::SetStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).enumz_ref()...);
    f(1, static_cast<T&&>(t).intz_ref()...);
    f(2, static_cast<T&&>(t).binnaz_ref()...);
    f(3, static_cast<T&&>(t).encoded_ref()...);
    f(4, static_cast<T&&>(t).uidz_ref()...);
    f(5, static_cast<T&&>(t).charz_ref()...);
    f(6, static_cast<T&&>(t).setz_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MapStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).enumz_ref()...);
    f(1, static_cast<T&&>(t).intz_ref()...);
    f(2, static_cast<T&&>(t).binnaz_ref()...);
    f(3, static_cast<T&&>(t).encoded_ref()...);
    f(4, static_cast<T&&>(t).flotz_ref()...);
    f(5, static_cast<T&&>(t).map_list_ref()...);
    f(6, static_cast<T&&>(t).list_map_ref()...);
    f(7, static_cast<T&&>(t).fast_list_map_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::ComposeStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).enum__ref()...);
    f(1, static_cast<T&&>(t).renamed__ref()...);
    f(2, static_cast<T&&>(t).primitive_ref()...);
    f(3, static_cast<T&&>(t).aliased_ref()...);
    f(4, static_cast<T&&>(t).xenum_ref()...);
    f(5, static_cast<T&&>(t).xstruct_ref()...);
    f(6, static_cast<T&&>(t).friends_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::Shallot> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).myEnum_ref()...);
    f(1, static_cast<T&&>(t).myStruct_ref()...);
    f(2, static_cast<T&&>(t).myString_ref()...);
    f(3, static_cast<T&&>(t).intSet_ref()...);
    f(4, static_cast<T&&>(t).doubleList_ref()...);
    f(5, static_cast<T&&>(t).strMap_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyStructPatchStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).assign_ref()...);
    f(1, static_cast<T&&>(t).clear_ref()...);
    f(2, static_cast<T&&>(t).patchPrior_ref()...);
    f(3, static_cast<T&&>(t).ensure_ref()...);
    f(4, static_cast<T&&>(t).patch_ref()...);
    f(5, static_cast<T&&>(t).remove_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyStructField4PatchStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).assign_ref()...);
    f(1, static_cast<T&&>(t).clear_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyStructField6PatchStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).assign_ref()...);
    f(1, static_cast<T&&>(t).clear_ref()...);
    f(2, static_cast<T&&>(t).prepend_ref()...);
    f(3, static_cast<T&&>(t).append_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyStructField7PatchStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).assign_ref()...);
    f(1, static_cast<T&&>(t).clear_ref()...);
    f(2, static_cast<T&&>(t).patchPrior_ref()...);
    f(3, static_cast<T&&>(t).add_ref()...);
    f(4, static_cast<T&&>(t).patch_ref()...);
    f(5, static_cast<T&&>(t).remove_ref()...);
    f(6, static_cast<T&&>(t).put_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyStructField8PatchStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).assign_ref()...);
    f(1, static_cast<T&&>(t).clear_ref()...);
    f(2, static_cast<T&&>(t).remove_ref()...);
    f(3, static_cast<T&&>(t).add_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyStructFieldPatchStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).inty_ref()...);
    f(1, static_cast<T&&>(t).stringy_ref()...);
    f(2, static_cast<T&&>(t).myItemy_ref()...);
    f(3, static_cast<T&&>(t).myEnumy_ref()...);
    f(4, static_cast<T&&>(t).booly_ref()...);
    f(5, static_cast<T&&>(t).floatListy_ref()...);
    f(6, static_cast<T&&>(t).strMappy_ref()...);
    f(7, static_cast<T&&>(t).intSetty_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyStructEnsureStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).inty_ref()...);
    f(1, static_cast<T&&>(t).stringy_ref()...);
    f(2, static_cast<T&&>(t).myItemy_ref()...);
    f(3, static_cast<T&&>(t).myEnumy_ref()...);
    f(4, static_cast<T&&>(t).booly_ref()...);
    f(5, static_cast<T&&>(t).floatListy_ref()...);
    f(6, static_cast<T&&>(t).strMappy_ref()...);
    f(7, static_cast<T&&>(t).intSetty_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyDataItemPatchStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).assign_ref()...);
    f(1, static_cast<T&&>(t).clear_ref()...);
    f(2, static_cast<T&&>(t).patchPrior_ref()...);
    f(3, static_cast<T&&>(t).ensure_ref()...);
    f(4, static_cast<T&&>(t).patch_ref()...);
    f(5, static_cast<T&&>(t).remove_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyDataItemFieldPatchStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).s_ref()...);
  }
};

template <>
struct ForEachField<::test::fixtures::basic-python-capi::MyDataItemEnsureStruct> {
  template <typename F, typename... T>
  void operator()(FOLLY_MAYBE_UNUSED F&& f, FOLLY_MAYBE_UNUSED T&&... t) const {
    f(0, static_cast<T&&>(t).s_ref()...);
  }
};
} // namespace detail
} // namespace thrift
} // namespace apache
