#ifndef OPCODE_H
#define OPCODE_H

extern const char* OpcodeNames[];

typedef enum Opcode {
#define OPCODE(opcode, _) opcode,
#include "opcode.def"
} Opcode;

int opcodeArgsNumber(Opcode op);

#endif
