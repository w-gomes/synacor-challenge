#include "computer.hpp"

#include <stdio.h>  // std::FILE, std::getchar()

#include <bitset>
#include <cassert>

#include "fmt/format.h"
#include "tl/numeric-aliases.hpp"

namespace synacor {
auto Computer::run(tl::u16 start) -> void {
  // program counter
  auto pc        = tl::u16{start};
  this->running_ = true;

  while (this->running_) {
    auto instruction = this->memory_[pc++];

    switch (instruction) {
      case Opcode::HALT: {
        fmt::print("{}\n", "computer is halted.");
        this->running_ = false;
        break;
      }

      case Opcode::SET: {
        const auto dest  = this->memory_[pc++];
        const auto value = this->read_from(this->memory_[pc++]);
        this->write_to(dest, value);
        break;
      }

      case Opcode::PUSH: {
        const auto value = this->read_from(this->memory_[pc++]);
        this->stack_.push(value);
        break;
      }

      case Opcode::POP: {
        if (this->stack_.empty()) {
          fmt::print(stderr, "{}\n", "error: empty stack");
          this->running_ = false;
          break;
        }

        const auto value = this->stack_.top();
        const auto dest  = this->memory_[pc++];
        this->write_to(dest, value);
        this->stack_.pop();
        break;
      }

      case Opcode::EQ: {
        auto value_a       = this->memory_[pc++];
        const auto value_b = this->read_from(this->memory_[pc++]);
        const auto value_c = this->read_from(this->memory_[pc++]);
        if (value_b == value_c) {
          this->write_to(value_a, 1);
        } else {
          this->write_to(value_a, 0);
        }
        break;
      }

      case Opcode::GT: {
        auto value_a       = this->memory_[pc++];
        const auto value_b = this->read_from(this->memory_[pc++]);
        const auto value_c = this->read_from(this->memory_[pc++]);
        if (value_b > value_c) {
          this->write_to(value_a, 1);
        } else {
          this->write_to(value_a, 0);
        }
        break;
      }

      case Opcode::JMP: {
        pc = this->memory_[pc];
        break;
      }

      case Opcode::JT: {
        const auto value_a = this->read_from(this->memory_[pc++]);
        const auto value_b = this->read_from(this->memory_[pc++]);
        if (value_a != 0) { pc = value_b; }
        break;
      }

      case Opcode::JF: {
        const auto value_a = this->read_from(this->memory_[pc++]);
        const auto value_b = this->read_from(this->memory_[pc++]);
        if (value_a == 0) { pc = value_b; }
        break;
      }

      case Opcode::ADD: {
        const auto value_a = this->memory_[pc++];
        const auto value_b = this->read_from(this->memory_[pc++]);
        const auto value_c = this->read_from(this->memory_[pc++]);
        this->write_to(value_a, (value_b + value_c) % Computer::MOD);
        break;
      }

      case Opcode::MULT: {
        const auto value_a = this->memory_[pc++];
        const auto value_b = this->read_from(this->memory_[pc++]);
        const auto value_c = this->read_from(this->memory_[pc++]);
        this->write_to(value_a, (value_b * value_c) % Computer::MOD);
        break;
      }

      case Opcode::MOD: {
        const auto value_a = this->memory_[pc++];
        const auto value_b = this->read_from(this->memory_[pc++]);
        const auto value_c = this->read_from(this->memory_[pc++]);
        this->write_to(value_a, value_b % value_c);
        break;
      }

      case Opcode::AND: {
        const auto value_a = this->memory_[pc++];
        const auto value_b = this->read_from(this->memory_[pc++]);
        const auto value_c = this->read_from(this->memory_[pc++]);
        this->write_to(value_a, value_b & value_c);
        break;
      }

      case Opcode::OR: {
        const auto value_a = this->memory_[pc++];
        const auto value_b = this->read_from(this->memory_[pc++]);
        const auto value_c = this->read_from(this->memory_[pc++]);
        this->write_to(value_a, value_b | value_c);
        break;
      }

      case Opcode::NOT: {
        const auto value_a = this->memory_[pc++];
        const auto value_b = this->read_from(this->memory_[pc++]);
        auto bit           = std::bitset<15>(value_b);
        bit                = ~bit;
        this->write_to(value_a, static_cast<tl::u16>(bit.to_ulong()));
        break;
      }

      case Opcode::RMEM: {
        const auto dest = this->memory_[pc++];
        const auto mem  = this->read_from(this->memory_[pc++]);
        this->write_to(dest, this->memory_[mem]);
        break;
      }

      case Opcode::WMEM: {
        const auto mem     = this->read_from(this->memory_[pc++]);
        const auto value   = this->read_from(this->memory_[pc++]);
        this->memory_[mem] = value;
        break;
      }

      case Opcode::CALL: {
        this->stack_.push(pc + 1);
        pc = this->read_from(this->memory_[pc++]);
        break;
      }

      case Opcode::RET: {
        if (this->stack_.empty()) {
          fmt::print(stderr, "{}\n", "error: empty stack");
          this->running_ = false;
          break;
        }
        const auto ret = this->stack_.top();

        pc = ret;
        this->stack_.pop();
        break;
      }

      case Opcode::OUTPUT: {
        const auto chr = this->read_from(this->memory_[pc++]);
        fmt::print("{}", static_cast<char>(chr));
        break;
      }

      case Opcode::INPUT: {
        const auto chr = std::getchar();
        const auto reg = this->memory_[pc++];
        this->write_to(reg, static_cast<tl::u16>(chr));
        break;
      }

      case Opcode::NOOP: {
        // no operation
        break;
      }
    }
  }
}

auto Computer::read_file(const char *file) -> void {
  std::FILE *in;
  fopen_s(&in, file, "rb");

  if (in) {
    fread(this->memory_.data(),
          sizeof(tl::u16),
          std::numeric_limits<tl::u16>::max(),
          in);
  } else {
    fmt::print(stderr, "{} {}\n", "cannot open file", file);
  }
}

auto Computer::reg_index(tl::u16 value) const -> tl::usize {
  assert(value >= Computer::MOD && value < (Computer::MOD + 8));
  return value - Computer::MOD;
}

auto Computer::type(tl::u16 value) const -> Type {
  if (value >= 0 && value < Computer::MOD) {
    // 0 ... 32767
    return Type::Literal;
  } else if (value >= Computer::MOD && value < (Computer::MOD + 8)) {
    // 32768 ... 32775
    return Type::Register;
  }
  return Type::Invalid;
}

auto Computer::write_to(tl::u16 dest, tl::u16 value) -> void {
  const auto ty = this->type(dest);
  switch (ty) {
    case Type::Register:
      this->register_[this->reg_index(dest)] = value;
      break;
    case Type::Literal:
      fmt::print(stderr,
                 "{} dest: {} value: {}\n",
                 "error: can only write to register.",
                 dest,
                 value);
      this->running_ = false;
      break;
    default:
      fmt::print(stderr, "{}", "error: wrong type.");
      this->running_ = false;
      break;
  }
}

auto Computer::read_from(tl::u16 src) const -> tl::u16 {
  const auto ty = this->type(src);
  switch (ty) {
    case Type::Register: {
      const auto value = this->register_[this->reg_index(src)];
      return value;
    }
    case Type::Literal:
      return src;
    case Type::Invalid:
      fmt::print(stderr, "{}", "error: wrong type.");
      this->running_ = false;
      return 0;
  }
}
}  // namespace synacor
