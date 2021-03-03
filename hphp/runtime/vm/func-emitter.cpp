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

#include "hphp/runtime/vm/func-emitter.h"

#include "hphp/runtime/base/array-iterator.h"
#include "hphp/runtime/base/coeffects-config.h"
#include "hphp/runtime/base/unit-cache.h"
#include "hphp/runtime/base/rds.h"

#include "hphp/runtime/ext/extension.h"

#include "hphp/runtime/vm/blob-helper.h"
#include "hphp/runtime/vm/bytecode.h"
#include "hphp/runtime/vm/native.h"
#include "hphp/runtime/vm/reified-generics.h"
#include "hphp/runtime/vm/repo.h"
#include "hphp/runtime/vm/repo-autoload-map-builder.h"
#include "hphp/runtime/vm/runtime.h"

#include "hphp/runtime/vm/jit/types.h"

#include "hphp/runtime/vm/verifier/cfg.h"

#include "hphp/system/systemlib.h"

#include "hphp/util/atomic-vector.h"
#include "hphp/util/file.h"
#include "hphp/util/trace.h"

namespace HPHP {

///////////////////////////////////////////////////////////////////////////////

using BytecodeArena = ReadOnlyArena<VMColdAllocator<char>, false, 8>;
static BytecodeArena& bytecode_arena() {
  static BytecodeArena arena(RuntimeOption::EvalHHBCArenaChunkSize);
  return arena;
}

/*
 * Export for the admin server.
 */
size_t hhbc_arena_capacity() {
  if (!RuntimeOption::RepoAuthoritative) return 0;
  return bytecode_arena().capacity();
}

///////////////////////////////////////////////////////////////////////////////
// FuncEmitter.

FuncEmitter::FuncEmitter(UnitEmitter& ue, int sn, Id id, const StringData* n)
  : m_ue(ue)
  , m_pce(nullptr)
  , m_sn(sn)
  , m_id(id)
  , m_bc(nullptr)
  , m_bclen(0)
  , m_bcmax(0)
  , name(n)
  , maxStackCells(0)
  , hniReturnType(folly::none)
  , retUserType(nullptr)
  , docComment(nullptr)
  , originalFilename(nullptr)
  , memoizePropName(nullptr)
  , memoizeGuardPropName(nullptr)
  , memoizeSharedPropIndex(0)
  , m_numLocals(0)
  , m_numUnnamedLocals(0)
  , m_numIterators(0)
  , m_nextFreeIterator(0)
  , m_ehTabSorted(false)
{}

FuncEmitter::FuncEmitter(UnitEmitter& ue, int sn, const StringData* n,
                         PreClassEmitter* pce)
  : m_ue(ue)
  , m_pce(pce)
  , m_sn(sn)
  , m_id(kInvalidId)
  , m_bc(nullptr)
  , m_bclen(0)
  , m_bcmax(0)
  , name(n)
  , maxStackCells(0)
  , hniReturnType(folly::none)
  , retUserType(nullptr)
  , docComment(nullptr)
  , originalFilename(nullptr)
  , memoizePropName(nullptr)
  , memoizeGuardPropName(nullptr)
  , memoizeSharedPropIndex(0)
  , m_numLocals(0)
  , m_numUnnamedLocals(0)
  , m_numIterators(0)
  , m_nextFreeIterator(0)
  , m_ehTabSorted(false)
{}

FuncEmitter::~FuncEmitter() {
  if (m_bc) free(m_bc);
}


///////////////////////////////////////////////////////////////////////////////
// Source locations.

SourceLocTable FuncEmitter::createSourceLocTable() const {
  assertx(m_sourceLocTab.size() != 0);
  SourceLocTable locations;
  for (size_t i = 0; i < m_sourceLocTab.size(); ++i) {
    Offset endOff = i < m_sourceLocTab.size() - 1
      ? m_sourceLocTab[i + 1].first
      : past;
    locations.push_back(SourceLocEntry(endOff, m_sourceLocTab[i].second));
  }
  return locations;
}

namespace {

using SrcLoc = std::vector<std::pair<Offset, SourceLoc>>;

/*
 * Create a LineTable from `srcLoc'.
 */
LineTable createLineTable(const SrcLoc& srcLoc, Offset bclen) {
  LineTable lines;
  if (srcLoc.empty()) {
    return lines;
  }

  auto prev = srcLoc.begin();
  for (auto it = prev + 1; it != srcLoc.end(); ++it) {
    if (prev->second.line1 != it->second.line1) {
      lines.push_back(LineEntry(it->first, prev->second.line1));
      prev = it;
    }
  }

  lines.push_back(LineEntry(bclen, prev->second.line1));
  return lines;
}

}

void FuncEmitter::recordSourceLocation(const Location::Range& sLoc,
                                       Offset start) {
  // Some byte codes, such as for the implicit "return 0" at the end of a
  // a source file do not have valid source locations. This check makes
  // sure we don't record a (dummy) source location in this case.
  if (start > 0 && sLoc.line0 == -1) return;
  SourceLoc newLoc(sLoc);
  if (!m_sourceLocTab.empty()) {
    if (m_sourceLocTab.back().second == newLoc) {
      // Combine into the interval already at the back of the vector.
      assertx(start >= m_sourceLocTab.back().first);
      return;
    }
    assertx(m_sourceLocTab.back().first < start &&
           "source location offsets must be added to UnitEmitter in "
           "increasing order");
  } else {
    // First record added should be for bytecode offset zero or very rarely one
    // when the source starts with a label and a Nop is inserted.
    assertx(start == 0 || start == 1);
  }
  m_sourceLocTab.push_back(std::make_pair(start, newLoc));
}

///////////////////////////////////////////////////////////////////////////////
// Initialization and execution.

void FuncEmitter::init(int l1, int l2, Attr attrs_,
                       const StringData* docComment_) {
  line1 = l1;
  line2 = l2;
  attrs = fix_attrs(attrs_);
  docComment = docComment_;

  if (!SystemLib::s_inited) assertx(attrs & AttrBuiltin);
}

void FuncEmitter::finish(Offset past_) {
  past = past_;
  sortEHTab();
}

void FuncEmitter::commit(RepoTxn& txn) {
  Repo& repo = Repo::get();
  FuncRepoProxy& frp = repo.frp();
  int repoId = m_ue.m_repoId;
  int64_t usn = m_ue.m_sn;

  if (!m_sourceLocTab.empty()) {
    m_lineTable = createLineTable(m_sourceLocTab, past);
  }

  frp.insertFunc[repoId]
     .insert(*this, txn, usn, m_sn, m_pce ? m_pce->id() : -1, name, m_bc, m_bclen);

  if (RuntimeOption::RepoDebugInfo) {
    for (size_t i = 0; i < m_sourceLocTab.size(); ++i) {
      SourceLoc& e = m_sourceLocTab[i].second;
      Offset endOff = i < m_sourceLocTab.size() - 1
                          ? m_sourceLocTab[i + 1].first
                          : past;

      frp.insertFuncSourceLoc[repoId]
          .insert(txn, usn, m_sn, endOff, e.line0, e.char0, e.line1, e.char1);
    }
  }
}

const StaticString s_DynamicallyCallable("__DynamicallyCallable");

static const unsigned char*
allocateBCRegion(const unsigned char* bc, size_t bclen) {
  g_hhbc_size->addValue(bclen);
  auto mem = static_cast<unsigned char*>(
    RuntimeOption::RepoAuthoritative ? bytecode_arena().allocate(bclen)
                                     : malloc(bclen));
  std::copy(bc, bc + bclen, mem);
  return mem;
}

Func* FuncEmitter::create(Unit& unit, PreClass* preClass /* = NULL */, bool saveLineTable) const {
  bool isGenerated = isdigit(name->data()[0]);

  auto attrs = fix_attrs(this->attrs);
  if (preClass && preClass->attrs() & AttrInterface) {
    attrs |= AttrAbstract;
  }
  if (attrs & AttrIsMethCaller && RuntimeOption::RepoAuthoritative) {
    attrs |= AttrPersistent | AttrUnique;
  }
  if (attrs & AttrPersistent && !preClass) {
    if ((RuntimeOption::EvalJitEnableRenameFunction ||
         attrs & AttrInterceptable ||
         (!RuntimeOption::RepoAuthoritative && SystemLib::s_inited))) {
      if (attrs & AttrBuiltin) {
        SystemLib::s_anyNonPersistentBuiltins = true;
      }
      attrs = Attr(attrs & ~AttrPersistent);
    }
  } else {
    assertx(preClass || !(attrs & AttrBuiltin) || (attrs & AttrIsMethCaller));
  }
  if (isVariadic()) {
    attrs |= AttrVariadicParam;
  }
  if (isAsync && !isGenerator) {
    // Async functions can return results directly.
    attrs |= AttrSupportsAsyncEagerReturn;
  }

  auto const dynCallSampleRate = [&] () -> folly::Optional<int64_t> {
    if (!(attrs & AttrDynamicallyCallable)) return {};

    auto const uattr = userAttributes.find(s_DynamicallyCallable.get());
    if (uattr == userAttributes.end()) return {};

    auto const tv = uattr->second;
    assertx(isArrayLikeType(type(tv)));
    auto const rate = val(tv).parr->get(int64_t(0));
    if (!isIntType(type(rate)) || val(rate).num < 0) return {};

    attrs = Attr(attrs & ~AttrDynamicallyCallable);
    return val(rate).num;
  }();

  assertx(!m_pce == !preClass);
  auto f = m_ue.newFunc(this, unit, name, attrs, params.size());

  f->m_isPreFunc = !!preClass;

  auto const uait = userAttributes.find(s___Reified.get());
  auto const hasReifiedGenerics = uait != userAttributes.end();

  bool const needsExtendedSharedData =
    isNative ||
    line2 - line1 >= Func::kSmallDeltaLimit ||
    past >= Func::kSmallDeltaLimit ||
    m_sn >= Func::kSmallDeltaLimit ||
    hasReifiedGenerics ||
    hasParamsWithMultiUBs ||
    hasReturnWithMultiUBs ||
    dynCallSampleRate ||
    !coeffectRules.empty();

  auto const bc = allocateBCRegion(m_bc, m_bclen);
  f->m_shared.reset(
    needsExtendedSharedData
      ? new Func::ExtendedSharedData(bc, m_bclen, preClass, past, m_sn, line1, line2,
                                     !containsCalls, docComment)
      : new Func::SharedData(bc, m_bclen, preClass, past, m_sn,
                             line1, line2, !containsCalls, docComment)
  );

  f->init(params.size());

  if (auto const ex = f->extShared()) {
    ex->m_allFlags.m_hasExtendedSharedData = true;
    ex->m_arFuncPtr = nullptr;
    ex->m_nativeFuncPtr = nullptr;
    ex->m_sn = m_sn;
    ex->m_line2 = line2;
    ex->m_past = past;
    ex->m_dynCallSampleRate = dynCallSampleRate.value_or(-1);
    ex->m_allFlags.m_returnByValue = false;
    ex->m_allFlags.m_isMemoizeWrapper = false;
    ex->m_allFlags.m_isMemoizeWrapperLSB = false;

    if (!coeffectRules.empty()) {
      ex->m_coeffectRules = coeffectRules;
      f->shared()->m_allFlags.m_hasCoeffectRules = true;
    }
  }

  auto coeffects = StaticCoeffects::none();
  for (auto const& name : staticCoeffects) {
    f->shared()->m_staticCoeffectNames.push_back(name);
    coeffects |= CoeffectsConfig::fromName(name->toCppString());
  }
  f->m_staticCoeffects = coeffects;


  std::vector<Func::ParamInfo> fParams;
  for (unsigned i = 0; i < params.size(); ++i) {
    Func::ParamInfo pi = params[i];
    if (pi.isVariadic()) {
      pi.builtinType = RuntimeOption::EvalHackArrDVArrs
        ? KindOfVec : KindOfVArray;
    }
    f->appendParam(params[i].isInOut(), pi, fParams);
    auto const& fromUBs = params[i].upperBounds;
    if (!fromUBs.empty()) {
      auto& ub = f->extShared()->m_paramUBs[i];
      ub.resize(fromUBs.size());
      std::copy(fromUBs.begin(), fromUBs.end(), ub.begin());
      f->shared()->m_allFlags.m_hasParamsWithMultiUBs = true;
    }
  }

  auto const originalFullName =
    (!originalFilename ||
     !RuntimeOption::RepoAuthoritative ||
     FileUtil::isAbsolutePath(originalFilename->slice())) ?
    originalFilename :
    makeStaticString(RuntimeOption::SourceRoot +
                     originalFilename->toCppString());

  f->shared()->m_localNames.create(m_localNames);
  f->shared()->m_numLocals = m_numLocals;
  f->shared()->m_numIterators = m_numIterators;
  f->m_maxStackCells = maxStackCells;
  f->shared()->m_ehtab = ehtab;
  f->shared()->m_allFlags.m_isClosureBody = isClosureBody;
  f->shared()->m_allFlags.m_isAsync = isAsync;
  f->shared()->m_allFlags.m_isGenerator = isGenerator;
  f->shared()->m_allFlags.m_isPairGenerator = isPairGenerator;
  f->shared()->m_userAttributes = userAttributes;
  f->shared()->m_retTypeConstraint = retTypeConstraint;
  f->shared()->m_retUserType = retUserType;
  if (!retUpperBounds.empty()) {
    f->extShared()->m_returnUBs.resize(retUpperBounds.size());
    std::copy(retUpperBounds.begin(), retUpperBounds.end(),
              f->extShared()->m_returnUBs.begin());
    f->shared()->m_allFlags.m_hasReturnWithMultiUBs = true;
  }
  f->shared()->m_originalFilename = originalFullName;
  f->shared()->m_allFlags.m_isGenerated = isGenerated;
  f->shared()->m_repoReturnType = repoReturnType;
  f->shared()->m_repoAwaitedReturnType = repoAwaitedReturnType;
  f->shared()->m_allFlags.m_isMemoizeWrapper = isMemoizeWrapper;
  f->shared()->m_allFlags.m_isMemoizeWrapperLSB = isMemoizeWrapperLSB;
  f->shared()->m_allFlags.m_hasReifiedGenerics = hasReifiedGenerics;
  f->shared()->m_allFlags.m_isRxDisabled = isRxDisabled;

  if (hasReifiedGenerics) {
    auto tv = uait->second;
    assertx(tvIsHAMSafeVArray(tv));
    f->extShared()->m_reifiedGenericsInfo =
      extractSizeAndPosFromReifiedAttribute(tv.m_data.parr);
  }

  /*
   * What's going on is we're going to have a m_lineTable if this FuncEmitter
   * was loaded from the repo, and no m_sourceLocTab (it's demand-loaded by
   * func.cpp because it's only used for the debugger).  Don't bother creating
   * the line table here, because we can retrieve it from the repo later.
   *
   * On the other hand, if this unit was just created by parsing a php file (or
   * whatnot) which was not committed to the repo, we'll have a m_sourceLocTab.
   * In this case we should populate m_lineTable (otherwise we might lose line
   * info altogether, since it may not be backed by a repo).
   */
  if (m_sourceLocTab.size() != 0) {
    f->stashLineTable(createLineTable(m_sourceLocTab, past));
    // If the debugger is enabled, or we plan to dump hhas we will
    // need the extended line table information in the output, and if
    // we're not writing the repo, stashing it here is necessary for
    // it to make it through.
    if (needs_extended_line_table()) {
      f->stashExtendedLineTable(createSourceLocTable());
    }
  } else if (saveLineTable) {
    f->stashLineTable(m_lineTable);
  }

  if (isNative) {
    auto const ex = f->extShared();

    ex->m_hniReturnType = hniReturnType;

    auto const info = getNativeInfo();

    Attr dummy = AttrNone;
    auto nativeAttributes = parseNativeAttributes(dummy);
    Native::getFunctionPointers(
      info,
      nativeAttributes,
      ex->m_arFuncPtr,
      ex->m_nativeFuncPtr
    );

    if (ex->m_nativeFuncPtr) {
      if (info.sig.ret == Native::NativeSig::Type::MixedTV) {
        ex->m_allFlags.m_returnByValue = true;
      }
      int extra = isMethod() ? 1 : 0;
      assertx(info.sig.args.size() == params.size() + extra);
      for (auto i = params.size(); i--; ) {
        switch (info.sig.args[extra + i]) {
          case Native::NativeSig::Type::ObjectArg:
          case Native::NativeSig::Type::StringArg:
          case Native::NativeSig::Type::ArrayArg:
          case Native::NativeSig::Type::ResourceArg:
            fParams[i].setFlag(Func::ParamInfo::Flags::NativeArg);
            break;
          case Native::NativeSig::Type::MixedTV:
            fParams[i].setFlag(Func::ParamInfo::Flags::NativeArg);
            fParams[i].setFlag(Func::ParamInfo::Flags::AsTypedValue);
            break;
          case Native::NativeSig::Type::Mixed:
            fParams[i].setFlag(Func::ParamInfo::Flags::AsVariant);
            break;
          default:
            break;
        }
      }
    }
  }

  f->finishedEmittingParams(fParams);

  if (RuntimeOption::EvalEnableReverseDataMap && !preClass) {
    f->registerInDataMap();
  }
  return f;
}

String FuncEmitter::nativeFullname() const {
  return Native::fullName(name, m_pce ? m_pce->name() : nullptr,
                          (attrs & AttrStatic));
}

Native::NativeFunctionInfo FuncEmitter::getNativeInfo() const {
  return Native::getNativeFunction(
      m_ue.m_nativeFuncs,
      name,
      m_pce ? m_pce->name() : nullptr,
      (attrs & AttrStatic)
    );
}

///////////////////////////////////////////////////////////////////////////////
// Locals, iterators, and parameters.

void FuncEmitter::allocVarId(const StringData* name, bool slotless) {
  assertx(name != nullptr);
  UNUSED Id id;
  if (m_localNames.find(name) == m_localNames.end()) {
    // Slotless locals must come after all locals with slots.
    if (!slotless) {
      id = (m_numLocals++);
      assertx(id == (int)m_localNames.size());
    }
    m_localNames.add(name, name);
  }
}

Id FuncEmitter::allocIterator() {
  assertx(m_numIterators >= m_nextFreeIterator);
  Id id = m_nextFreeIterator++;
  if (m_numIterators < m_nextFreeIterator) {
    m_numIterators = m_nextFreeIterator;
  }
  return id;
}

Id FuncEmitter::allocUnnamedLocal() {
  m_localNames.addUnnamed(nullptr);
  ++m_numUnnamedLocals;
  return m_numLocals++;
}

///////////////////////////////////////////////////////////////////////////////
// Unit tables.

EHEnt& FuncEmitter::addEHEnt() {
  assertx(!m_ehTabSorted
    || "should only mark the ehtab as sorted after adding all of them");
  ehtab.emplace_back();
  ehtab.back().m_parentIndex = 7777;
  return ehtab.back();
}

namespace {

/*
 * Ordering on EHEnts where e1 < e2 iff
 *
 *    a) e1 and e2 do not overlap, and e1 comes first
 *    b) e1 encloses e2
 */
struct EHEntComp {
  bool operator()(const EHEnt& e1, const EHEnt& e2) const {
    if (e1.m_base == e2.m_base) {
      return e1.m_past > e2.m_past;
    }
    return e1.m_base < e2.m_base;
  }
};

}

void FuncEmitter::sortEHTab() {
  if (m_ehTabSorted) return;

  std::sort(ehtab.begin(), ehtab.end(), EHEntComp());

  for (unsigned int i = 0; i < ehtab.size(); i++) {
    ehtab[i].m_parentIndex = -1;
    for (int j = i - 1; j >= 0; j--) {
      if (ehtab[j].m_past >= ehtab[i].m_past) {
        // parent EHEnt better enclose this one.
        assertx(ehtab[j].m_base <= ehtab[i].m_base);
        ehtab[i].m_parentIndex = j;
        break;
      }
    }
  }

  setEHTabIsSorted();
}

void FuncEmitter::setEHTabIsSorted() {
  m_ehTabSorted = true;
  if (!debug) return;

  Offset curBase = 0;
  for (size_t i = 0; i < ehtab.size(); ++i) {
    auto& eh = ehtab[i];

    // Base offsets must be monotonically increasing.
    always_assert(curBase <= eh.m_base);
    curBase = eh.m_base;

    // Parent should come before, and must enclose this guy.
    always_assert(eh.m_parentIndex == -1 || eh.m_parentIndex < i);
    if (eh.m_parentIndex != -1) {
      auto& parent = ehtab[eh.m_parentIndex];
      always_assert(parent.m_base <= eh.m_base &&
                    parent.m_past >= eh.m_past);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Bytecode

void FuncEmitter::setBc(const unsigned char* bc, size_t bclen) {
  if (m_bc) {
    free(m_bc);
  }
  m_bc = (unsigned char*)malloc(bclen);
  m_bcmax = bclen;
  if (bclen) memcpy(m_bc, bc, bclen);
  m_bclen = bclen;
}

///////////////////////////////////////////////////////////////////////////////
// Complex setters.

/* <<__Native>> user attribute causes systemlib declarations
 * to hook internal (C++) implementation of funcs/methods
 *
 * The Native attribute may have the following sub-options
 *  "NoFCallBuiltin": Prevent FCallBuiltin optimization
 *      Effectively forces functions to generate an ActRec
 *  "NoInjection": Do not include this frame in backtraces
 *
 *  e.g.   <<__Native("NoFCallBuiltin")>> function foo():mixed;
 */
static const StaticString
  s_native("__Native"),
  s_nofcallbuiltin("NoFCallBuiltin"),
  s_noinjection("NoInjection"),
  s_opcodeimpl("OpCodeImpl");

int FuncEmitter::parseNativeAttributes(Attr& attrs_) const {
  int ret = Native::AttrNone;

  auto it = userAttributes.find(s_native.get());
  assertx(it != userAttributes.end());
  const TypedValue userAttr = it->second;
  assertx(isArrayLikeType(userAttr.m_type));
  for (ArrayIter it(userAttr.m_data.parr); it; ++it) {
    Variant userAttrVal = it.second();
    if (userAttrVal.isString()) {
      String userAttrStrVal = userAttrVal.toString();
      if (userAttrStrVal.get()->isame(s_nofcallbuiltin.get())) {
        attrs_ |= AttrNoFCallBuiltin;
      } else if (userAttrStrVal.get()->isame(s_noinjection.get())) {
        attrs_ |= AttrNoInjection;
      } else if (userAttrStrVal.get()->isame(s_opcodeimpl.get())) {
        ret |= Native::AttrOpCodeImpl;
      }
    }
  }
  return ret;
}

Attr FuncEmitter::fix_attrs(Attr a) const {
  if (RuntimeOption::RepoAuthoritative) return a;

  a = Attr(a & ~AttrInterceptable);

  if (RuntimeOption::EvalJitEnableRenameFunction) {
    return a | AttrInterceptable;
  }
  return a;
}

///////////////////////////////////////////////////////////////////////////////
// Serialization/Deserialization

template<class SerDe>
void FuncEmitter::serdeMetaData(SerDe& sd) {
  // NOTE: name and a few other fields currently handled outside of this.
  Offset past_delta;
  Attr a = attrs;

  if (!SerDe::deserializing) {
    past_delta = past;
    a = fix_attrs(attrs);
  }

  sd(line1)
    (line2)
    (past_delta)
    (a)
    (staticCoeffects)
    (hniReturnType)
    (repoReturnType)
    (repoAwaitedReturnType)
    (docComment)
    (m_numLocals)
    (m_numIterators)
    (maxStackCells)
    (m_repoBoolBitset)

    (params)
    (m_localNames, [](auto s) { return s; })
    (ehtab,
      [&](const EHEnt& prev, EHEnt cur) -> EHEnt {
        if (!SerDe::deserializing) {
          cur.m_handler -= cur.m_past;
          cur.m_past -= cur.m_base;
          cur.m_base -= prev.m_base;
        } else {
          cur.m_base += prev.m_base;
          cur.m_past += cur.m_base;
          cur.m_handler += cur.m_past;
        }
        return cur;
      }
    )
    (userAttributes)
    (retTypeConstraint)
    (retUserType)
    (retUpperBounds)
    (originalFilename)
    (coeffectRules)
    ;

  if (SerDe::deserializing) {
    repoReturnType.resolveArray(ue());
    repoAwaitedReturnType.resolveArray(ue());
    past = past_delta;
    attrs = fix_attrs(a);
  }
}

template<class SerDe>
void FuncEmitter::serde(SerDe& sd) {
  serdeMetaData(sd);

  sd(
    m_lineTable,
    [&] (const LineEntry& prev, const LineEntry& curDelta) {
      if (SerDe::deserializing) {
        return LineEntry {
          curDelta.pastOffset() + prev.pastOffset(),
          curDelta.val() + prev.val()
        };
      } else {
        return LineEntry {
          curDelta.pastOffset() - prev.pastOffset(),
          curDelta.val() - prev.val()
        };
      }
    }
  );

  sd(m_sourceLocTab);

  // Bytecode
  if constexpr (SerDe::deserializing) {
    size_t size;
    sd(size);
    FTRACE_MOD(Trace::tmp0, 1, "Size: %d %d", size, sd.remaining());
    assertx(sd.remaining() >= size);
    setBc(sd.data(), size);
    sd.advance(size);
  } else {
    sd(m_bclen);
    sd.writeRaw((const char*)m_bc, m_bclen);
  }
}

template void FuncEmitter::serde<>(BlobDecoder&);
template void FuncEmitter::serde<>(BlobEncoder&);

///////////////////////////////////////////////////////////////////////////////
// FuncRepoProxy.

FuncRepoProxy::FuncRepoProxy(Repo& repo)
    : RepoProxy(repo),
      insertFunc{InsertFuncStmt(repo, 0), InsertFuncStmt(repo, 1)},
      getFuncs{GetFuncsStmt(repo, 0), GetFuncsStmt(repo, 1)},
      getFuncLineTable{GetFuncLineTableStmt(repo, 0), GetFuncLineTableStmt(repo, 1)},
      insertFuncSourceLoc{InsertFuncSourceLocStmt(repo, 0), InsertFuncSourceLocStmt(repo, 1)},
      getSourceLocTab{GetSourceLocTabStmt(repo, 0), GetSourceLocTabStmt(repo, 1)}
{}

FuncRepoProxy::~FuncRepoProxy() {
}

void FuncRepoProxy::createSchema(int repoId, RepoTxn& txn) {
  {
    auto createQuery = folly::sformat(
      "CREATE TABLE {} "
      "(unitSn INTEGER, funcSn INTEGER, preClassId INTEGER, name TEXT, "
      " bc BLOB, extraData BLOB, lineTable BLOB, PRIMARY KEY (unitSn, funcSn));",
      m_repo.table(repoId, "Func"));
    txn.exec(createQuery);
  }
  {
    auto createQuery = folly::sformat(
      "CREATE TABLE {} "
      "(unitSn INTEGER, funcSn INTEGER, pastOffset INTEGER, line0 INTEGER,"
      " char0 INTEGER, line1 INTEGER, char1 INTEGER,"
      " PRIMARY KEY (unitSn, funcSn, pastOffset));",
      m_repo.table(repoId, "FuncSourceLoc"));
    txn.exec(createQuery);
  }
}

void FuncRepoProxy::InsertFuncStmt
                  ::insert(const FuncEmitter& fe,
                           RepoTxn& txn, int64_t unitSn, int funcSn,
                           Id preClassId, const StringData* name,
                           const unsigned char* bc, size_t bclen) {
  if (!prepared()) {
    auto insertQuery = folly::sformat(
      "INSERT INTO {} "
      "VALUES(@unitSn, @funcSn, @preClassId, @name, @bc, @extraData, @lineTable);",
      m_repo.table(m_repoId, "Func"));
    txn.prepare(*this, insertQuery);
  }

  BlobEncoder extraBlob{fe.useGlobalIds()};
  BlobEncoder lineTableBlob{false};

  RepoTxnQuery query(txn, *this);
  query.bindInt64("@unitSn", unitSn);
  query.bindInt("@funcSn", funcSn);
  query.bindId("@preClassId", preClassId);
  query.bindStaticString("@name", name);
  query.bindBlob("@bc", (const void*)bc, bclen);
  const_cast<FuncEmitter&>(fe).serdeMetaData(extraBlob);
  query.bindBlob("@extraData", extraBlob, /* static */ true);

  lineTableBlob(
    const_cast<LineTable&>(fe.lineTable()),
    [&](const LineEntry& prev, const LineEntry& cur) -> LineEntry {
      return LineEntry {
        cur.pastOffset() - prev.pastOffset(),
        cur.val() - prev.val()
      };
    }
  );
  query.bindBlob("@lineTable", lineTableBlob, /* static */ true);

  query.exec();
}

void FuncRepoProxy::GetFuncsStmt
                  ::get(UnitEmitter& ue) {
  auto txn = RepoTxn{m_repo.begin()};
  if (!prepared()) {
    auto selectQuery = folly::sformat(
      "SELECT funcSn, preClassId, name, bc, extraData, lineTable "
      "FROM {} "
      "WHERE unitSn == @unitSn ORDER BY funcSn ASC;",
      m_repo.table(m_repoId, "Func"));
    txn.prepare(*this, selectQuery);
  }
  RepoTxnQuery query(txn, *this);
  query.bindInt64("@unitSn", ue.m_sn);
  do {
    query.step();
    if (query.row()) {
      int funcSn;               /**/ query.getInt(0, funcSn);
      Id preClassId;            /**/ query.getId(1, preClassId);
      StringData* name;         /**/ query.getStaticString(2, name);
      const void* bc; size_t bclen; /**/ query.getBlob(3, bc, bclen);
      BlobDecoder extraBlob =   /**/ query.getBlob(4, ue.useGlobalIds());

      FuncEmitter* fe;
      if (preClassId < 0) {
        fe = ue.newFuncEmitter(name);
      } else {
        PreClassEmitter* pce = ue.pce(preClassId);
        fe = ue.newMethodEmitter(name, pce);
        bool added UNUSED = pce->addMethod(fe);
        assertx(added);
      }
      assertx(fe->sn() == funcSn);
      fe->setBc(static_cast<const unsigned char*>(bc), bclen);
      fe->serdeMetaData(extraBlob);
      if (!SystemLib::s_inited) {
        assertx(fe->attrs & AttrBuiltin);
        if (preClassId < 0) {
          assertx(fe->attrs & AttrPersistent);
          assertx(fe->attrs & AttrUnique);
        }
      }

      BlobDecoder lineTableBlob = query.getBlob(5, false);
      lineTableBlob(
        fe->m_lineTable,
        [&](const LineEntry& prev, const LineEntry& delta) -> LineEntry {
          return LineEntry {
            delta.pastOffset() + prev.pastOffset(),
            delta.val() + prev.val()
          };
        }
      );

      fe->setEHTabIsSorted();
      fe->finish(fe->past);
    }
  } while (!query.done());
  txn.commit();
}

void FuncRepoProxy::GetFuncLineTableStmt::get(int64_t unitSn, int64_t funcSn,
                                              LineTable& lineTable) {
  auto txn = RepoTxn{m_repo.begin()};
  if (!prepared()) {
    auto selectQuery = folly::sformat(
      "SELECT lineTable FROM {} WHERE unitSn == @unitSn AND funcSn == @funcSn;",
      m_repo.table(m_repoId, "Func"));
    txn.prepare(*this, selectQuery);
  }
  RepoTxnQuery query(txn, *this);
  query.bindInt64("@unitSn", unitSn);
  query.bindInt64("@funcSn", funcSn);
  query.step();
  if (query.row()) {
    BlobDecoder dataBlob = query.getBlob(0, false);
    dataBlob(
      lineTable,
      [&](const LineEntry& prev, const LineEntry& delta) -> LineEntry {
        return LineEntry {
          delta.pastOffset() + prev.pastOffset(),
          delta.val() + prev.val()
        };
      }
    );
  }
  txn.commit();
}

void FuncRepoProxy::InsertFuncSourceLocStmt
                  ::insert(RepoTxn& txn, int64_t unitSn, int64_t funcSn, Offset pastOffset,
                           int line0, int char0, int line1, int char1) {
  if (!prepared()) {
    auto insertQuery = folly::sformat(
      "INSERT INTO {} "
      "VALUES(@unitSn, @funcSn, @pastOffset, @line0, @char0, @line1, @char1);",
      m_repo.table(m_repoId, "FuncSourceLoc"));
    txn.prepare(*this, insertQuery);
  }
  RepoTxnQuery query(txn, *this);
  query.bindInt64("@unitSn", unitSn);
  query.bindInt64("@funcSn", funcSn);
  query.bindOffset("@pastOffset", pastOffset);
  query.bindInt("@line0", line0);
  query.bindInt("@char0", char0);
  query.bindInt("@line1", line1);
  query.bindInt("@char1", char1);
  query.exec();
}

RepoStatus
FuncRepoProxy::GetSourceLocTabStmt::get(int64_t unitSn, int64_t funcSn,
                                        SourceLocTable& sourceLocTab) {
  try {
    auto txn = RepoTxn{m_repo.begin()};
    if (!prepared()) {
      auto selectQuery = folly::sformat(
        "SELECT pastOffset, line0, char0, line1, char1 "
        "FROM {} "
        "WHERE unitSn == @unitSn AND funcSn = @funcSn "
        "ORDER BY pastOffset ASC;",
        m_repo.table(m_repoId, "FuncSourceLoc"));
      txn.prepare(*this, selectQuery);
    }
    RepoTxnQuery query(txn, *this);
    query.bindInt64("@unitSn", unitSn);
    query.bindInt64("@funcSn", funcSn);
    do {
      query.step();
      if (!query.row()) {
        return RepoStatus::error;
      }
      Offset pastOffset;
      query.getOffset(0, pastOffset);
      SourceLoc sLoc;
      query.getInt(1, sLoc.line0);
      query.getInt(2, sLoc.char0);
      query.getInt(3, sLoc.line1);
      query.getInt(4, sLoc.char1);
      SourceLocEntry entry(pastOffset, sLoc);
      sourceLocTab.push_back(entry);
    } while (!query.done());
    txn.commit();
  } catch (RepoExc& re) {
    return RepoStatus::error;
  }
  return RepoStatus::success;
}

///////////////////////////////////////////////////////////////////////////////
}
