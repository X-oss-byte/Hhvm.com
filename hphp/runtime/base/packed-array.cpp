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
#include "hphp/runtime/base/packed-array.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

#include <folly/Format.h>
#include <folly/Likely.h>

#include "hphp/runtime/base/apc-array.h"
#include "hphp/runtime/base/apc-stats.h"
#include "hphp/runtime/base/array-init.h"
#include "hphp/runtime/base/tv-val.h"
#include "hphp/runtime/base/mixed-array.h"
#include "hphp/runtime/base/runtime-error.h"
#include "hphp/runtime/base/request-info.h"
#include "hphp/runtime/base/tv-comparisons.h"
#include "hphp/runtime/base/tv-mutate.h"
#include "hphp/runtime/base/tv-refcount.h"
#include "hphp/runtime/base/tv-type.h"
#include "hphp/runtime/base/tv-uncounted.h"
#include "hphp/runtime/base/tv-variant.h"

#include "hphp/runtime/base/mixed-array-defs.h"
#include "hphp/runtime/base/packed-array-defs.h"
#include "hphp/runtime/base/packed-block.h"

namespace HPHP {

//////////////////////////////////////////////////////////////////////

std::aligned_storage<sizeof(ArrayData), 16>::type s_theEmptyVec;
std::aligned_storage<sizeof(ArrayData), 16>::type s_theEmptyMarkedVec;

auto constexpr kDefaultVanillaArrayExtra = ArrayData::kDefaultVanillaArrayExtra;

struct PackedArray::VecInitializer {
  VecInitializer() {
    auto const aux = packSizeIndexAndAuxBits(0, 0);
    auto const ad = reinterpret_cast<ArrayData*>(&s_theEmptyVec);
    ad->m_size = 0;
    ad->m_extra = kDefaultVanillaArrayExtra;
    ad->initHeader_16(HeaderKind::Vec, StaticValue, aux);
    assertx(checkInvariants(ad));
  }
};
PackedArray::VecInitializer PackedArray::s_vec_initializer;

struct PackedArray::MarkedVecInitializer {
  MarkedVecInitializer() {
    auto const aux = packSizeIndexAndAuxBits(0, ArrayData::kLegacyArray);
    auto const ad = reinterpret_cast<ArrayData*>(&s_theEmptyMarkedVec);
    ad->m_size = 0;
    ad->m_extra = kDefaultVanillaArrayExtra;
    ad->initHeader_16(HeaderKind::Vec, StaticValue, aux);
    assertx(checkInvariants(ad));
  }
};
PackedArray::MarkedVecInitializer PackedArray::s_marked_vec_initializer;

//////////////////////////////////////////////////////////////////////

namespace {

inline ArrayData* alloc_packed_static(const ArrayData* ad) {
  auto const size = PackedArray::capacityToSizeBytes(ad->size());
  auto const ret = RuntimeOption::EvalLowStaticArrays
    ? low_malloc(size)
    : uncounted_malloc(size);
  return reinterpret_cast<ArrayData*>(reinterpret_cast<char*>(ret));
}

}

bool PackedArray::checkInvariants(const ArrayData* arr) {
  assertx(arr->checkCountZ());
  assertx(arr->isVanillaVec());
  assertx(arr->m_size <= MixedArray::MaxSize);
  assertx(arr->m_size <= capacity(arr));
  assertx(arr->m_extra == kDefaultVanillaArrayExtra);

  // This loop is too slow for normal use, but can be enabled to debug
  // packed arrays.
  if (false) {
    for (uint32_t i = 0; i < arr->m_size; ++i) {
      auto const DEBUG_ONLY tv = NvGetInt(arr, i);
      assertx(tv.is_init());
      assertx(tvIsPlausible(tv));
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////////////

ALWAYS_INLINE
MixedArray* PackedArray::ToMixedHeader(const ArrayData* old,
                                       size_t neededSize) {
  assertx(checkInvariants(old));

  auto const oldSize = old->m_size;
  auto const scale   = MixedArray::computeScaleFromSize(neededSize);
  auto const ad      = MixedArray::reqAlloc(scale);
  auto const kind    = HeaderKind::Dict;
  ad->initHeader_16(kind, OneReference, MixedArrayKeys::packIntsForAux());
  ad->m_size         = oldSize;
  ad->m_extra        = old->m_extra;
  ad->m_scale_used   = scale | uint64_t{oldSize} << 32; // used=oldSize
  ad->m_nextKI       = oldSize;

  assertx(ad->m_size == oldSize);
  assertx(ad->isVanillaDict());
  assertx(ad->hasExactlyOneRef());
  assertx(ad->m_used == oldSize);
  assertx(ad->m_scale == scale);
  assertx(ad->m_nextKI == oldSize);
  // Can't checkInvariants yet, since we haven't populated the payload.
  return ad;
}

/*
 * Converts a packed array to mixed, leaving the packed array in an
 * empty state.  You need ToMixedCopy in cases where the old array
 * needs to remain un-modified (usually if `copy' is true).
 *
 * The returned array is mixed, and is guaranteed not to be isFull().
 * (Note: only unset can call ToMixed when we aren't about to insert.)
 */
MixedArray* PackedArray::ToMixed(ArrayData* old) {
  auto const oldSize = old->m_size;
  auto const ad      = ToMixedHeader(old, oldSize + 1);
  auto const mask    = ad->mask();
  auto dstData       = ad->data();

  auto const dstHash = ad->initHash(ad->scale());
  for (uint32_t i = 0; i < oldSize; ++i) {
    auto h = hash_int64(i);
    *ad->findForNewInsert(dstHash, mask, h) = i;
    dstData->setIntKey(i, h);
    tvCopy(GetPosVal(old, i), dstData->data);
    ++dstData;
  }
  old->m_size = 0;

  assertx(ad->checkInvariants());
  assertx(!ad->isFull());
  assertx(ad->hasExactlyOneRef());
  return ad;
}

/*
 * Convert a packed array to mixed, without moving the elements out of
 * the old packed array.  This effectively performs a Copy at the same
 * time as converting to mixed.  The returned mixed array is
 * guaranteed not to be full.
 */
MixedArray* PackedArray::ToMixedCopy(const ArrayData* old) {
  assertx(checkInvariants(old));
  auto ad = PackedArray::ToMixedCopyReserve(old, old->m_size + 1);
  assertx(!ad->isFull());
  return ad;
}

/*
 * Convert to mixed, reserving space for at least `neededSize' elems.
 * `neededSize' must be at least old->size(), and may be equal to it.
 */
MixedArray* PackedArray::ToMixedCopyReserve(const ArrayData* old,
                                            size_t neededSize) {
  assertx(neededSize >= old->m_size);
  auto const ad      = ToMixedHeader(old, neededSize);
  auto const oldSize = old->m_size;
  auto const mask    = ad->mask();
  auto dstData       = ad->data();

  auto const dstHash = ad->initHash(ad->scale());
  for (uint32_t i = 0; i < oldSize; ++i) {
    auto const h = hash_int64(i);
    *ad->findForNewInsert(dstHash, mask, h) = i;
    dstData->setIntKey(i, h);
    tvDup(GetPosVal(old, i), dstData->data);
    ++dstData;
  }

  assertx(ad->checkInvariants());
  assertx(ad->hasExactlyOneRef());
  return ad;
}

NEVER_INLINE
ArrayData* PackedArray::Grow(ArrayData* adIn, bool copy) {
  assertx(checkInvariants(adIn));
  assertx(adIn->m_size == capacity(adIn));

  auto const sizeIndex = sizeClass(adIn) + kSizeClassesPerDoubling;
  if (UNLIKELY(sizeIndex > MaxSizeIndex)) return nullptr;
  auto ad = static_cast<ArrayData*>(tl_heap->objMallocIndex(sizeIndex));

  if (copy) {
    // CopyPackedHelper will copy the header and m_size; since we pass
    // convertingPackedToVec = false, it can't fail. All we have to do
    // afterwards is fix the capacity and refcount on the copy; it's easiest
    // to do that by reinitializing the whole header.
    CopyPackedHelper(adIn, ad);
    ad->initHeader_16(
      adIn->m_kind,
      OneReference,
      packSizeIndexAndAuxBits(sizeIndex, adIn->auxBits())
    );

    assertx(ad->m_size == adIn->m_size);
  } else {
    // Copy everything from `adIn' to `ad', including header and m_size
    memcpy16_inline(ad, adIn, PackedArray::capacityToSizeBytes(adIn->m_size));
    ad->initHeader_16(
      adIn->m_kind,
      OneReference,
      packSizeIndexAndAuxBits(sizeIndex, adIn->auxBits())
    );

    assertx(ad->m_size == adIn->m_size);
    adIn->m_size = 0; // old is a zombie now
  }

  assertx(ad->kind() == adIn->kind());
  assertx(capacity(ad) > capacity(adIn));
  assertx(ad->hasExactlyOneRef());
  assertx(ad->m_extra == adIn->m_extra);
  assertx(checkInvariants(ad));
  return ad;
}

ALWAYS_INLINE
ArrayData* PackedArray::PrepareForInsert(ArrayData* adIn, bool copy) {
  assertx(checkInvariants(adIn));
  if (adIn->m_size == capacity(adIn)) return Grow(adIn, copy);
  if (copy) return Copy(adIn);
  return adIn;
}

//////////////////////////////////////////////////////////////////////

/* This helper copies everything from adIn to ad, including the header
 * (capacity, kind, and refcount) and m_size. It then increfs the
 * contents, if needed.
 *
 * If convertingPackedToVec is false, it will always succeed (return true).
 *
 * If convertingPackedToVec is true and adIn contains a Ref, then it will
 * return false. Refcounts of the contents will be left in a consistent state.
 * It is the callers responsibility to free ad and throw an appropriate
 * exception in this case.
 */
ALWAYS_INLINE
void PackedArray::CopyPackedHelper(const ArrayData* adIn, ArrayData* ad) {
  // Copy everything from `adIn' to `ad', including refcount, kind and cap
  auto const size = adIn->m_size;
  memcpy16_inline(ad, adIn, PackedArray::capacityToSizeBytes(size));

  // Copy counted types correctly
  for (uint32_t i = 0; i < size; ++i) {
    auto const elm = LvalUncheckedInt(ad, i);
    tvIncRefGen(*elm);
  }
}

NEVER_INLINE
ArrayData* PackedArray::Copy(const ArrayData* adIn) {
  assertx(checkInvariants(adIn));

  auto ad = static_cast<ArrayData*>(tl_heap->objMallocIndex(sizeClass(adIn)));

  // CopyPackedHelper will copy the header (including capacity and kind), and
  // m_size.  All we have to do afterwards is fix the refcount on the copy.
  CopyPackedHelper(adIn, ad);
  ad->m_count = OneReference;

  assertx(ad->kind() == adIn->kind());
  assertx(ad->isLegacyArray() == adIn->isLegacyArray());
  assertx(capacity(ad) == capacity(adIn));
  assertx(ad->m_size == adIn->m_size);
  assertx(ad->m_extra == adIn->m_extra);
  assertx(ad->hasExactlyOneRef());
  assertx(checkInvariants(ad));
  return ad;
}

ArrayData* PackedArray::CopyStatic(const ArrayData* adIn) {
  assertx(checkInvariants(adIn));

  auto const sizeIndex = capacityToSizeIndex(adIn->m_size);
  auto ad = alloc_packed_static(adIn);
  // CopyPackedHelper will copy the header and m_size. All we have to do
  // afterwards is fix the capacity and refcount on the copy; it's easiest to do
  // that by reinitializing the whole header.
  CopyPackedHelper(adIn, ad);
  ad->initHeader_16(
    adIn->m_kind,
    StaticValue,
    packSizeIndexAndAuxBits(sizeIndex, adIn->auxBits())
  );

  assertx(ad->kind() == adIn->kind());
  assertx(capacity(ad) >= adIn->m_size);
  assertx(ad->m_size == adIn->m_size);
  assertx(ad->m_extra == adIn->m_extra);
  assertx(ad->isStatic());
  assertx(checkInvariants(ad));
  return ad;
}

/* This helper allocates an ArrayData and initializes the header (including
 * capacity, kind, and refcount). The caller is responsible for initializing
 * m_size, and initializing array entries (if any).
 */
ALWAYS_INLINE
ArrayData* PackedArray::MakeReserveImpl(uint32_t cap, HeaderKind hk) {
  auto const sizeIndex = capacityToSizeIndex(cap);
  auto ad = static_cast<ArrayData*>(tl_heap->objMallocIndex(sizeIndex));
  ad->initHeader_16(hk, OneReference, packSizeIndexAndAuxBits(sizeIndex, 0));

  assertx(ad->m_kind == hk);
  assertx(capacity(ad) >= cap);
  assertx(ad->hasExactlyOneRef());
  return ad;
}

ArrayData* PackedArray::MakeReserveVec(uint32_t capacity) {
  auto ad = MakeReserveImpl(capacity, HeaderKind::Vec);
  ad->m_size = 0;
  ad->m_extra = kDefaultVanillaArrayExtra;
  assertx(ad->isVanillaVec());
  assertx(ad->m_size == 0);
  assertx(checkInvariants(ad));
  return ad;
}

template<bool reverse>
ALWAYS_INLINE
ArrayData* PackedArray::MakePackedImpl(uint32_t size,
                                       const TypedValue* values,
                                       HeaderKind hk) {
  assertx(size > 0);
  auto ad = MakeReserveImpl(size, hk);
  ad->m_size = size;
  ad->m_extra = kDefaultVanillaArrayExtra;

  // Append values by moving; this function takes ownership of them.
  if (reverse) {
    uint32_t i = size - 1;
    for (auto end = values + size; values < end; ++values, --i) {
      tvCopy(*values, LvalUncheckedInt(ad, i));
    }
  } else {
    if (debug) {
      for (uint32_t i = 0; i < size; ++i) {
        assertx(tvIsPlausible(*(values + i)));
      }
    }
    if constexpr (stores_typed_values) {
      auto const data = PackedArray::entries(ad);
      memcpy16_inline(data, values, sizeof(TypedValue) * size);
    } else {
      for (uint32_t i = 0; i < size; ++values, ++i) {
        tvCopy(*values, LvalUncheckedInt(ad, i));
      }
    }
  }

  assertx(ad->m_size == size);
  assertx(checkInvariants(ad));
  return ad;
}

ArrayData* PackedArray::MakeVec(uint32_t size, const TypedValue* values) {
  return MakePackedImpl<true>(size, values, HeaderKind::Vec);
}

ArrayData* PackedArray::MakeVecNatural(uint32_t size, const TypedValue* values) {
  return MakePackedImpl<false>(size, values, HeaderKind::Vec);
}

ArrayData* PackedArray::MakeUninitializedVec(uint32_t size) {
  auto ad = MakeReserveImpl(size, HeaderKind::Vec);
  ad->m_size = size;
  ad->m_extra = kDefaultVanillaArrayExtra;
  assertx(ad->isVanillaVec());
  assertx(ad->m_size == size);
  assertx(checkInvariants(ad));
  return ad;
}

ArrayData* PackedArray::MakeVecFromAPC(const APCArray* apc, bool isLegacy) {
  assertx(apc->isPacked());
  auto const apcSize = apc->size();
  VecInit init{apcSize};
  for (uint32_t i = 0; i < apcSize; ++i) {
    init.append(apc->getValue(i)->toLocal());
  }
  auto const ad = init.create();
  ad->setLegacyArrayInPlace(isLegacy);
  return ad;
}

void PackedArray::Release(ArrayData* ad) {
  ad->fixCountForRelease();
  assertx(checkInvariants(ad));
  assertx(ad->isRefCounted());
  assertx(ad->hasExactlyOneRef());

  if constexpr (stores_typed_values) {
    for (uint32_t i = 0; i < ad->m_size; ++i) {
      tvDecRefGen(LvalUncheckedInt(ad, i));
    }
  } else {
    auto constexpr n = PackedBlock::kNumItems;
    auto block = PackedBlock::BlockAt(ad, 0);
    auto remainder = ad->size();
    while (remainder >= n) {
      if (!block.AllTypesAreUncounted(n)) {
        for (auto i = 0; i < n; i++) {
          tvDecRefGen(block[i]);
        }
      }
      ++block;
      remainder -= n;
    }
    if (remainder && !block.AllTypesAreUncounted(remainder)) {
      auto i = 0;
      do {
        tvDecRefGen(block[i++]);
      } while (i < remainder);
    }
  }

  tl_heap->objFreeIndex(ad, sizeClass(ad));
  AARCH64_WALKABLE_FRAME();
}

NEVER_INLINE
void PackedArray::ReleaseUncounted(ArrayData* ad) {
  assertx(checkInvariants(ad));
  assertx(!ad->uncountedCowCheck());

  for (uint32_t i = 0; i < ad->m_size; ++i) {
    DecRefUncounted(*LvalUncheckedInt(ad, i));
  }

  if (APCStats::IsCreated()) {
    APCStats::getAPCStats().removeAPCUncountedBlock();
  }

  auto const extra = uncountedAllocExtra(ad, ad->hasApcTv());
  auto const allocSize = extra + PackedArray::capacityToSizeBytes(ad->m_size);
  uncounted_sized_free(reinterpret_cast<char*>(ad) - extra, allocSize);
}

////////////////////////////////////////////////////////////////////////////////

TypedValue PackedArray::NvGetInt(const ArrayData* ad, int64_t k) {
  assertx(checkInvariants(ad));
  return LIKELY(size_t(k) < ad->m_size)
    ? *LvalUncheckedInt(const_cast<ArrayData*>(ad), k)
    : make_tv<KindOfUninit>();
}

TypedValue PackedArray::NvGetStr(const ArrayData* ad, const StringData* /*s*/) {
  assertx(checkInvariants(ad));
  return make_tv<KindOfUninit>();
}

TypedValue PackedArray::GetPosKey(const ArrayData* ad, ssize_t pos) {
  assertx(checkInvariants(ad));
  assertx(pos != ad->m_size);
  return make_tv<KindOfInt64>(pos);
}

TypedValue PackedArray::GetPosVal(const ArrayData* ad, ssize_t pos) {
  assertx(checkInvariants(ad));
  assertx(pos < ad->m_size);
  return *LvalUncheckedInt(const_cast<ArrayData*>(ad), pos);
}

bool PackedArray::ExistsInt(const ArrayData* ad, int64_t k) {
  assertx(checkInvariants(ad));
  return size_t(k) < ad->m_size;
}

bool PackedArray::ExistsStr(const ArrayData* ad, const StringData* /*s*/) {
  assertx(checkInvariants(ad));
  return false;
}

///////////////////////////////////////////////////////////////////////////////

namespace {

template<typename FoundFn>
auto MutableOpInt(ArrayData* adIn, int64_t k, bool copy, FoundFn found) {
  assertx(PackedArray::checkInvariants(adIn));
  if (UNLIKELY(size_t(k) >= adIn->size())) {
    throwOOBArrayKeyException(k, adIn);
  }
  auto const ad = copy ? PackedArray::Copy(adIn) : adIn;
  return found(ad);
}

}

arr_lval PackedArray::LvalInt(ArrayData* adIn, int64_t k) {
  assertx(checkInvariants(adIn));
  return MutableOpInt(adIn, k, adIn->cowCheck(),
    [&] (ArrayData* ad) { return arr_lval { ad, LvalUncheckedInt(ad, k) }; }
  );
}

tv_lval PackedArray::LvalUncheckedInt(ArrayData* ad, int64_t k) {
  assertx(size_t(k) < PackedArray::capacity(ad));
  return stores_typed_values ? &PackedArray::entries(ad)[k]
                             : PackedBlock::LvalAt(ad, k);
}

arr_lval PackedArray::LvalStr(ArrayData* adIn, StringData* key) {
  assertx(checkInvariants(adIn));
  throwInvalidArrayKeyException(key, adIn);
}

tv_lval PackedArray::LvalNewInPlace(ArrayData* ad) {
  assertx(checkInvariants(ad));
  assertx(ad->m_size < capacity(ad));
  assertx(!ad->cowCheck());
  auto const lval = LvalUncheckedInt(ad, ad->m_size++);
  type(lval) = KindOfNull;
  return lval;
}

ArrayData* PackedArray::SetIntMove(ArrayData* adIn, int64_t k, TypedValue v) {
  assertx(v.m_type != KindOfUninit);
  assertx(adIn->cowCheck() || adIn->notCyclic(v));
  auto const copy = adIn->cowCheck();
  return MutableOpInt(adIn, k, copy,
    [&] (ArrayData* ad) {
      assertx((adIn != ad) == copy);
      if (copy && adIn->decReleaseCheck()) PackedArray::Release(adIn);
      tvMove(v, LvalUncheckedInt(ad, k));
      return ad;
    }
  );
}

ArrayData* PackedArray::SetStrMove(ArrayData* adIn, StringData* k, TypedValue v) {
  assertx(checkInvariants(adIn));
  throwInvalidArrayKeyException(k, adIn);
}

///////////////////////////////////////////////////////////////////////////////

ArrayData* PackedArray::RemoveInt(ArrayData* adIn, int64_t k) {
  assertx(checkInvariants(adIn));

  // You're only allowed to remove an element at the end of the varray or
  // vec (or beyond the end, which is a no-op).
  if (UNLIKELY(size_t(k) >= adIn->m_size)) return adIn;
  if (LIKELY(size_t(k) + 1 == adIn->m_size)) {
    auto const ad = adIn->cowCheck() ? Copy(adIn) : adIn;
    auto const size = ad->m_size - 1;
    ad->m_size = size;
    tvDecRefGen(LvalUncheckedInt(ad, size));
    return ad;
  }

  if (adIn->isVanillaVec()) {
    throwVecUnsetException();
  } else {
    throwVarrayUnsetException();
  }
}

ArrayData* PackedArray::RemoveStr(ArrayData* adIn, const StringData*) {
  assertx(checkInvariants(adIn));
  return adIn;
}

ssize_t PackedArray::IterBegin(const ArrayData* ad) {
  assertx(checkInvariants(ad));
  return 0;
}

ssize_t PackedArray::IterLast(const ArrayData* ad) {
  assertx(checkInvariants(ad));
  return ad->m_size ? ad->m_size - 1 : 0;
}

ssize_t PackedArray::IterEnd(const ArrayData* ad) {
  assertx(checkInvariants(ad));
  return ad->m_size;
}

ssize_t PackedArray::IterAdvance(const ArrayData* ad, ssize_t pos) {
  assertx(checkInvariants(ad));
  if (pos < ad->m_size) {
    ++pos;
  }
  return pos;
}

ssize_t PackedArray::IterRewind(const ArrayData* ad, ssize_t pos) {
  assertx(checkInvariants(ad));
  if (pos > 0) {
    return pos - 1;
  }
  return ad->m_size;
}

ArrayData* PackedArray::AppendImpl(
    ArrayData* adIn, TypedValue v, bool copy, bool move) {
  assertx(checkInvariants(adIn));
  assertx(v.m_type != KindOfUninit);
  assertx(copy || adIn->notCyclic(v));
  auto const ad = PrepareForInsert(adIn, copy);
  if (move) {
    if (ad != adIn && adIn->decReleaseCheck()) Release(adIn);
    tvCopy(v, LvalUncheckedInt(ad, ad->m_size++));
  } else {
    tvDup(v, LvalUncheckedInt(ad, ad->m_size++));
  }
  return ad;
}

ArrayData* PackedArray::AppendMove(ArrayData* adIn, TypedValue v) {
  assertx(v.m_type != KindOfUninit);
  return AppendImpl(adIn, v, adIn->cowCheck(), true);
}

ArrayData* PackedArray::AppendInPlace(ArrayData* adIn, TypedValue v) {
  assertx(!adIn->cowCheck());
  assertx(v.m_type != KindOfUninit);
  return AppendImpl(adIn, v, false);
}

ArrayData* PackedArray::Pop(ArrayData* adIn, Variant& value) {
  assertx(checkInvariants(adIn));

  auto const ad = adIn->cowCheck() ? Copy(adIn) : adIn;

  if (UNLIKELY(ad->m_size == 0)) {
    value = uninit_null();
    return ad;
  }

  auto const size = ad->m_size - 1;
  auto const tv = *LvalUncheckedInt(ad, size);
  value = tvAsCVarRef(&tv);
  ad->m_size = size;
  tvDecRefGen(tv);
  return ad;
}

ArrayData* PackedArray::Prepend(ArrayData* adIn, TypedValue v) {
  assertx(checkInvariants(adIn));

  auto const ad = PrepareForInsert(adIn, adIn->cowCheck());
  auto const size = ad->m_size;
  if constexpr (stores_typed_values) {
    auto const data = PackedArray::entries(ad);
    std::memmove(data + 1, data, sizeof *data * size);
  } else {
    for (uint32_t i = size; i > 0; --i) {
      tvCopy(*LvalUncheckedInt(ad, i - 1), LvalUncheckedInt(ad, i));
    }
  }
  tvDup(v, LvalUncheckedInt(ad, 0));
  ad->m_size = size + 1;
  return ad;
}

void PackedArray::OnSetEvalScalar(ArrayData* ad) {
  assertx(checkInvariants(ad));
  auto const size = ad->m_size;
  for (uint32_t i = 0; i < size; ++i) {
    auto lval = LvalUncheckedInt(ad, i);
    auto tv = *lval;
    tvAsVariant(&tv).setEvalScalar();
    tvCopy(tv, lval);
  }
}

void PackedArray::Ksort(ArrayData* ad, int /*flags*/, bool ascending) {
  assertx(ascending);
  assertx(checkInvariants(ad));
}

void PackedArray::Asort(ArrayData* ad, int, bool) {
  always_assert(false);
}

bool PackedArray::Uksort(ArrayData* ad, const Variant&) {
  always_assert(false);
}

bool PackedArray::Uasort(ArrayData* ad, const Variant&) {
  always_assert(false);
}

ArrayData* PackedArray::MakeUncounted(ArrayData* array,
                                      bool withApcTypedValue,
                                      DataWalker::PointerMap* seen) {
  auto const updateSeen = seen && array->hasMultipleRefs();
  if (updateSeen) {
    auto it = seen->find(array);
    assertx(it != seen->end());
    if (auto const arr = static_cast<ArrayData*>(it->second)) {
      arr->uncountedIncRef();
      return arr;
    }
  }
  assertx(checkInvariants(array));
  assertx(!array->empty());
  if (APCStats::IsCreated()) {
    APCStats::getAPCStats().addAPCUncountedBlock();
  }

  auto const size = array->m_size;
  auto const extra = withApcTypedValue ? sizeof(APCTypedValue) : 0;
  auto const bytes = PackedArray::capacityToSizeBytes(size);
  auto const sizeIndex = MemoryManager::size2Index(bytes);
  assertx(sizeIndex <= PackedArray::MaxSizeIndex);

  auto const mem = static_cast<char*>(uncounted_malloc(bytes + extra));
  auto ad = reinterpret_cast<ArrayData*>(mem + extra);
  ad->initHeader_16(
    array->m_kind,
    UncountedValue,
    packSizeIndexAndAuxBits(sizeIndex, array->auxBits()) |
    (withApcTypedValue ? ArrayData::kHasApcTv : 0)
  );
  ad->m_size = array->m_size;
  ad->m_extra = array->m_extra;


  // Do a raw copy without worrying about refcounts. Then, traverse the
  // array and convert refcounted objects to their uncounted types.
  memcpy16_inline(ad + 1, array + 1, bytes - sizeof(ArrayData));
  for (uint32_t i = 0; i < size; i++) {
    ConvertTvToUncounted(LvalUncheckedInt(ad, i), seen);
  }

  assertx(ad->kind() == array->kind());
  assertx(capacity(ad) >= size);
  assertx(ad->m_size == size);
  assertx(ad->m_extra == array->m_extra);
  assertx(ad->isUncounted());
  assertx(checkInvariants(ad));
  if (updateSeen) (*seen)[array] = ad;
  return ad;
}

ALWAYS_INLINE
bool PackedArray::VecEqualHelper(const ArrayData* ad1, const ArrayData* ad2,
                                 bool strict) {
  assertx(checkInvariants(ad1));
  assertx(checkInvariants(ad2));
  assertx(ad1->isVanillaVec());
  assertx(ad2->isVanillaVec());

  if (ad1 == ad2) return true;
  if (ad1->m_size != ad2->m_size) return false;

  // Prevent circular referenced objects/arrays or deep ones.
  check_recursion_error();

  auto const size = ad1->m_size;
  for (uint32_t i = 0; i < size; ++i) {
    auto const elm1 = GetPosVal(ad1, i);
    auto const elm2 = GetPosVal(ad2, i);
    auto const cmp = strict ? tvSame(elm1, elm2) : tvEqual(elm1, elm2);
    if (!cmp) return false;
  }

  return true;
}

bool PackedArray::VecEqual(const ArrayData* ad1, const ArrayData* ad2) {
  return VecEqualHelper(ad1, ad2, false);
}

bool PackedArray::VecNotEqual(const ArrayData* ad1, const ArrayData* ad2) {
  return !VecEqualHelper(ad1, ad2, false);
}

bool PackedArray::VecSame(const ArrayData* ad1, const ArrayData* ad2) {
  return VecEqualHelper(ad1, ad2, true);
}

bool PackedArray::VecNotSame(const ArrayData* ad1, const ArrayData* ad2) {
  return !VecEqualHelper(ad1, ad2, true);
}

//////////////////////////////////////////////////////////////////////

PackedArray::EntryOffset PackedArray::entryOffset(size_t i) {
  if constexpr (stores_typed_values) {
    auto const base = sizeof(ArrayData) + i * sizeof(TypedValue);
    return {ptrdiff_t(base + offsetof(TypedValue, m_type)),
            ptrdiff_t(base + offsetof(TypedValue, m_data))};
  }
  return PackedBlock::EntryOffset(i);
}

int64_t PackedArray::pointerToIndex(const ArrayData* ad, const void* ptr) {
  const auto index = [&]() {
    if constexpr (stores_typed_values) {
      const auto tv = reinterpret_cast<const TypedValue*>(ptr);
      return tv - PackedArray::entries(const_cast<ArrayData*>(ad));
    } else {
      return PackedBlock::PointerToIndex(ad, ptr);
    }
  }();
  return 0 <= index && index < ad->m_size ? index : -1;
}

//////////////////////////////////////////////////////////////////////

}
