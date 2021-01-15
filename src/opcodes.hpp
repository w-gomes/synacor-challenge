#pragma once

namespace synacor {

enum Opcode {
  HALT = 0,
  SET,
  PUSH,
  POP,
  EQ,
  GT,
  JMP,
  JT,
  JF,
  ADD,
  MULT,
  MOD,
  AND,
  OR,
  NOT,
  RMEM,
  WMEM,
  CALL,
  RET,
  OUTPUT,  // OUT
  INPUT,   // INT
  NOOP,
};

enum Register { R0 = 0, R1, R2, R3, R4, R5, R6, R7 };

enum class Type { Register, Literal, Invalid };
}  // namespace synacor
