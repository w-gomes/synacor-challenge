#pragma once

#include <array>
#include <stack>

#include "opcodes.hpp"
#include "tl/numeric-aliases.hpp"

namespace synacor {

class Computer {
  // all math is modulo 32768;
  // e.g. 32758 + 15 = 5
  static constexpr auto MOD = 32768;

 public:
  auto run(tl::u16 start) -> void;
  auto read_file(const char *file) -> void;

 private:
  // methods
  auto reg_index(tl::u16 value) const -> tl::usize;
  auto type(tl::u16 value) const -> Type;
  auto write_to(tl::u16 dest, tl::u16 value) -> void;
  auto read_from(tl::u16 src) const -> tl::u16;

  std::array<tl::u16, 65536> memory_{};
  std::array<tl::u16, 8> register_{};
  std::stack<tl::u16> stack_{};
  mutable bool running_{false};
};
}  // namespace synacor
