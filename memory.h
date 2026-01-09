#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "assert.h"
#include "uarray.h"
#include "seq.h"
#include "bitpack.h"
#include "mem.h"

typedef struct Memory *Memory;

extern Memory Memory_new();
extern void Memory_free(Memory *mem);
extern void Memory_read(Memory mem, int fileSize, FILE *fp);

/* instruction helper functions */
extern uint32_t map_seg(Memory mem, int numWords);
extern void unmap_seg(Memory mem, int ID);
extern void store_val(Memory mem, int ID, int offset, uint32_t word);
extern uint32_t load_val(Memory mem, int ID, int offset);
extern uint32_t load_program(Memory mem, int ID, int counter);

#endif
