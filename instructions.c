/**************************************************************
 *
 *                         instructions.c
 *
 *     Contains the run function which computes a series of
 *     instructions in the Emulator struct. This function runs
 *     until the halt instruction is processed.
 *
 **************************************************************/

#include "instructions.h"

/* 
*       run_emulator
*       Purpose: 
*               Processes the all the instructions given to the UM
*               and calls the appropriate function from the 
*               Emulator until halt is called
*       Arguments:
*               Emulator emulator: The Emulator struct that
*                                  computations are performed in
*       Returns:
*               none
*       Notes:
*               Throws an error if the end of the instructions
*               is reached without encountering a halt 
*               instruction
*/
void run_emulator(Emulator emulator) 
{
        /* Array of function pointers to each instruction function */
        void (*op[14]) (Emulator emulator, uint32_t word) = {op_move, op_load,
                                        op_store, op_add, op_mult, op_divide, 
                                        op_nand, op_halt, op_map, op_unmap, 
                                        op_output, op_input, op_program, 
                                        op_value};

        /* Process each instruction until halt is called */
        while(!halt(emulator)) {
                uint32_t word = get_instruction(emulator);
                int code = Bitpack_getu(word, 4, 28);
                assert(code >= 0 && code < 14);
                op[code](emulator, word);
        }
}
