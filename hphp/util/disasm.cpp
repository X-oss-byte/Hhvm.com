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
#include "hphp/util/disasm.h"

#include <iomanip>
#include <stdlib.h>

#include <boost/algorithm/string.hpp>

#include <folly/Format.h>

#include "hphp/util/abi-cxx.h"
#include "hphp/util/text-color.h"

namespace HPHP {

static uintptr_t excludeLow, excludeLen;

#if defined(__x86_64__)

// XED callback function to get a symbol from an address
static int addressToSymbol(xed_uint64_t address, char* symbolBuffer,
                           xed_uint32_t bufferLength, xed_uint64_t* offset,
                           void* /*context*/) {
  if (address - excludeLow < excludeLen) return 0;

  auto name = boost::trim_copy(getNativeFunctionName((void*)address));
  if (boost::starts_with(name, "0x")) {
    return 0;
  }
  strncpy(symbolBuffer, name.c_str(), bufferLength - 1);
  symbolBuffer[bufferLength - 1] = '\0';
  *offset = 0;
  return 1;
}
#endif /* __x86_64__ */



void Disasm::ExcludedAddressRange(void* low, size_t len) {
  excludeLow = uintptr_t(low);
  excludeLen = len;
}

Disasm::Disasm(const Disasm::Options& opts)
    : m_opts(opts)
{
#if defined(__x86_64__)
  xed_state_init(&m_xedState, XED_MACHINE_MODE_LONG_64,
                 XED_ADDRESS_WIDTH_64b, XED_ADDRESS_WIDTH_64b);
  xed_tables_init();
#endif // __x86_64__
}

#if defined(__x86_64__)

#define MAX_INSTR_ASM_LEN 128

static const xed_syntax_enum_t s_xed_syntax =
  getenv("HHVM_INTEL_DISAS") ? XED_SYNTAX_INTEL : XED_SYNTAX_ATT;
#endif // __x86_64__

void Disasm::disasm(std::ostream& out, uint8_t* codeStartAddr,
                    uint8_t* codeEndAddr, uint64_t adjust) {

#if defined(__x86_64__)
  auto const endClr = m_opts.m_color.empty() ? "" : ANSI_COLOR_END;
  char codeStr[MAX_INSTR_ASM_LEN];
  xed_uint8_t *frontier;
  xed_decoded_inst_t xedd;
  uint64_t codeBase = uint64_t(codeStartAddr);
  uint64_t ip;

  // Decode and print each instruction
  for (frontier = codeStartAddr, ip = (uint64_t)codeStartAddr;
       frontier < codeEndAddr; ) {
    for (int i = 0; i < m_opts.m_indentLevel; ++i) {
      out << ' ';
    }

    xed_decoded_inst_zero_set_mode(&xedd, &m_xedState);
    xed_decoded_inst_set_input_chip(&xedd, XED_CHIP_INVALID);
    xed_error_enum_t xed_error = xed_decode(&xedd, frontier, 15);
    if (xed_error != XED_ERROR_NONE) {
      out << folly::format("xed_decode failed at address {}\n", frontier);
      return;
    }

    // Get disassembled instruction in codeStr
    auto const syntax = m_opts.m_forceAttSyntax ? XED_SYNTAX_ATT
                                                : s_xed_syntax;
    if (!xed_format_context(syntax, &xedd, codeStr,
                            MAX_INSTR_ASM_LEN, ip - adjust, nullptr,
                            addressToSymbol)) {
      out << folly::format("xed_format_context failed at address {}\n",
                           frontier);
      return;
    }
    uint32_t instrLen = xed_decoded_inst_get_length(&xedd);

    // If it's a jump, we're printing relative offsets, and the dest
    // is within the range we're printing, add the dest as a relative
    // offset.
    std::string jmpComment;
    auto const cat = xed_decoded_inst_get_category(&xedd);
    if (cat == XED_CATEGORY_COND_BR || cat == XED_CATEGORY_UNCOND_BR) {
      if (m_opts.m_relativeOffset) {
        auto disp = uint64_t(frontier + instrLen +
                             xed_decoded_inst_get_branch_displacement(&xedd) -
                             codeBase);
        if (disp < uint64_t(codeEndAddr - codeStartAddr)) {
          jmpComment = folly::format(" # {:#x}", disp).str();
        }
      }
    }

    out << m_opts.m_color;
    if (m_opts.m_addresses) {
      const char* fmt = m_opts.m_relativeOffset ? "{:3x}: " : "{:#10x}: ";
      out << folly::format(
        fmt,
        m_opts.m_relativeOffset ? (ip - codeBase) : (ip - adjust)
      );
    }
    if (m_opts.m_printEncoding) {
      // print encoding, like in objdump
      unsigned posi = 0;
      for (; posi < instrLen; ++posi) {
        out << folly::format("{:02x} ", (uint8_t)frontier[posi]);
      }
      for (; posi < 16; ++posi) {
        out << "   ";
      }
    }
    out << codeStr << jmpComment << endClr << '\n';
    frontier += instrLen;
    ip       += instrLen;
  }
#else
  out << "This binary was compiled without disassembly support\n";
#endif // __x86_64__
}

} // namespace HPHP
