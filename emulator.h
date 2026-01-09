#ifndef EMULATOR_INCLUDED
#define EMULATOR_INCLUDED

#include "memory.h"

typedef struct Emulator *Emulator;

extern Emulator Emulator_new( int fileSize, FILE *fp);
extern void Emulator_free(Emulator *emulator);
extern bool halt(Emulator emulator);
extern uint32_t get_instruction(Emulator emulator);

/* instruction functions */
extern void op_move(Emulator emulator, uint32_t word);
extern void op_load(Emulator emulator, uint32_t word);
extern void op_store(Emulator emulator, uint32_t word);
extern void op_add(Emulator emulator, uint32_t word);
extern void op_mult(Emulator emulator, uint32_t word);
extern void op_divide(Emulator emulator, uint32_t word);
extern void op_nand(Emulator emulator, uint32_t word);
extern void op_halt(Emulator emulator, uint32_t word);
extern void op_map(Emulator emulator, uint32_t word);
extern void op_unmap(Emulator emulator, uint32_t word);
extern void op_output(Emulator emulator, uint32_t word);
extern void op_input(Emulator emulator, uint32_t word);
extern void op_program(Emulator emulator, uint32_t word);
extern void op_value(Emulator emulator, uint32_t word);

#endif
