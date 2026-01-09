#include "emulator.h"

/* Emulator struct containing the register array, Memory struct
 * and trackers for the current instruction counter, the total
 * number of instructions, and a bool halt checking when to 
 * stop computation
 */
struct Emulator {
        uint32_t registers[8];
        Memory mem;
        uint32_t counter;
        uint32_t numInstructions;
        bool halt;
};

/* 
*       Emulator_new
*       Purpose:
*               Initializes a new Emulator by storing words from the given
*               file in memory.
*       Arguments:
*               int fileSize:           The number of bytes in the file
*               FILE *fp:               The pointer to the file
*       Returns:
*               Initialized emulator
*/
Emulator Emulator_new(int fileSize, FILE *fp) 
{
        /* Allocate space for new Emulator */
        Emulator emulator;
        NEW(emulator);
        
        /* Initialize all registers as 0 and other Emulator variables */
        for(int i = 0; i < 8; i++) {
                emulator->registers[i] = 0;
        }
        emulator->counter = 0;
        if(fileSize == 0) {
                /* covers case when no instructions are given */
                emulator->halt = true;
        } else {
                emulator->halt = false;
        }
        emulator->mem = Memory_new();
        Memory_read(emulator->mem, fileSize, fp);
        emulator->numInstructions = fileSize/4;
        return emulator;
}

/* 
*       Emulator_free
*       Purpose:
*               Free all memory allocated on the heap by the program.
*       Arguments:
*               Emulator *emulator: The address of the Emulator struct 
*                                   being freed
*       Returns:
*               none
*/
void Emulator_free(Emulator *emulator)
{
        assert(*emulator && emulator);

        Memory_free(&((*emulator)->mem));
        FREE(*emulator);
}

bool halt(Emulator emulator)
{
        return emulator->halt;
}

/* 
*       get_instruction
*       Purpose:
*               Retrives the next word from the Emulator's memory in order
*               to perform the given computation  
*       Arguments:
*               Emulator emulator: The Emulator struct where the
*                                  instructions are stored
*       Returns:
*               The current instruction as a 32-bit unsigned int   
*/
/* also increments counter by 1 */
uint32_t get_instruction(Emulator emulator)
{
        assert(emulator->counter < emulator->numInstructions);
        
        /* get next word */
        uint32_t word = load_val(emulator->mem, 0, emulator->counter);
        emulator->counter++;
        return word;
}

/* 
*       op_move
*       Purpose:
*               If r[C] != 0, sets the value of r[A] to the value in r[B] 
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified   
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none             
*/
void op_move(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set values in registers */
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);
        int a = Bitpack_getu(word, 3, 6);

        /* Check if value in register[c] is 0 */
        if (emulator->registers[c] != 0) {
                emulator->registers[a] = emulator->registers[b];
        }
}

/* 
*       op_load
*       Purpose:
*               Loads the value of the word in the segment with ID r[B] and
*               at the offset r[C] onto r[A]
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified 
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none               
*/
void op_load(Emulator emulator, uint32_t word)
{
        assert(emulator);

        /* Set values in registers */
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);
        int a = Bitpack_getu(word, 3, 6);

        /* Load value into register[a] */
        emulator->registers[a] = load_val(emulator->mem, 
                        emulator->registers[b], emulator->registers[c]);
}

/* 
*       op_store
*       Purpose:
*               Stores the value of r[C] in the segment wth ID r[A] at the 
*               offset r[B]
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified   
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none           
*/
void op_store(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set values in registers */
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);
        int a = Bitpack_getu(word, 3, 6);

        /* Store value */
        store_val(emulator->mem, emulator->registers[a], 
                        emulator->registers[b], emulator->registers[c]);
}

/* 
*       op_add
*       Purpose:
*               Stores the result of r[B] + r[C] in r[A]
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified 
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none             
*/
void op_add(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set registers */
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);
        int a = Bitpack_getu(word, 3, 6);

        /* Set register[a] to sum of register[b] and register[c] */
        emulator->registers[a] = emulator->registers[b] + 
                                        emulator->registers[c];
}

/* 
*       op_mult
*       Purpose:
*               Stores the result of r[B] * r[C] in r[A]
*       Arguments:
*               Emulator emulator: the emulator struct containing the 
*                                  registers/memory that's being modified  
*               uint32_t word:     the word containing the current instruction
*       Returns:
*               none             
*/
void op_mult(Emulator emulator, uint32_t word)
{
        assert(emulator);

        /* Set registers */
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);
        int a = Bitpack_getu(word, 3, 6);

        /* Set register[a] to the product of register[b] and register[c] */
        emulator->registers[a] = emulator->registers[b] * 
                                        emulator->registers[c];   
}

/* 
*       op_map
*       Purpose:
*               Stores the result of r[B] / r[C] in r[A]
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified  
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none   
*       Notes:
*               Fails if the value stored in r[C] is zero            
*/
void op_divide(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set registers*/
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);
        int a = Bitpack_getu(word, 3, 6);
        
        /* Check that divisor is not 0 */
        assert(emulator->registers[c] != 0);

        /* Set register[a] to the quotient of register[b] and register[c] */
        emulator->registers[a] = emulator->registers[b] / 
                                        emulator->registers[c];
}

/* 
*       op_nand
*       Purpose:
*               Stores the result of ~(r[B] & r[C]) in r[A]
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified  
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none             
*/
void op_nand(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set registers */
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);
        int a = Bitpack_getu(word, 3, 6);

        /* Perform NAND operation */
        emulator->registers[a] = ~(emulator->registers[b] & 
                                                emulator->registers[c]);
}

/* 
*       op_halt
*       Purpose:
*               Stops the UM from reading future instructions
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified  
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none               
*/
void op_halt(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        (void) word;
        emulator->halt = true;
}

/* 
*       op_map
*       Purpose:
*               Maps a new segment in memory with r[C] words and stores the ID
*               in r[B] 
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified 
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none               
*/
void op_map(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set registers */
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);

        /* Map segment */
        emulator->registers[b] = map_seg(emulator->mem, 
                                                emulator->registers[c]);
}

             
/* 
*       op_unmap
*       Purpose:
*               Unmaps the segment with ID r[C] in memory. r[C] may be reused
*               as an ID by a future segment  
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified  
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none         
*/
void op_unmap(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set register */
        int c = Bitpack_getu(word, 3, 0);

        /* Unmap segment */
        unmap_seg(emulator->mem, emulator->registers[c]);
}

/* 
*       op_output
*       Purpose:
*               Prints the value in r[C] to stdout 
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified   
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none
*       Notes:
*               Fails if r[C] holds a value greater than 255
*/
void op_output(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set register */
        int c = Bitpack_getu(word, 3, 0);

        /* Check that value within bounds and print */
        assert(emulator->registers[c] <= 255);
        printf("%c", emulator->registers[c]);
}

/* 
*       op_input
*       Purpose:
*               Takes in a single char input from stdin and stores the given
*               value in r[C] 
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified    
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none
*       Notes:
*               If end of input is signaled, r[C] is loaded
*               with a full 32-bit word        
*/
void op_input(Emulator emulator, uint32_t word)
{
        assert(emulator);
       
        int c = Bitpack_getu(word, 3, 0);
        emulator->registers[c] = getc(stdin); 
        if(feof(stdin)) {
                /* sets register[c] to full 32-bit value if EOF is reached */
                emulator->registers[c] = ~0;
        } else {
                /* check assertion, secondary cover for EOF situation */
                assert(emulator->registers[c] <= 255);
        }
}

/* 
*       op_program
*       Purpose:
*               Loads the program using the instructions stored in m[r[B]]
*               and sets the counter to r[C]
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified    
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none               
*/
void op_program(Emulator emulator, uint32_t word)
{
        assert(emulator);
        
        /* Set registers */
        int c = Bitpack_getu(word, 3, 0);
        int b = Bitpack_getu(word, 3, 3);

        /* Load program starting at m[reg[B]][reg[C]] and
         * updates number of total instructions in m[0]
         */
        emulator->numInstructions = load_program(emulator->mem, 
                                                emulator->registers[b], 
                                                emulator->registers[c]);
       
       /* counter is set to value given in register[c] */
        emulator->counter = emulator->registers[c];
}

/* 
*       op_value
*       Purpose:
*               Sets the value in r[A] to the value provided in instruction
*       Arguments:
*               Emulator emulator: The emulator struct containing the 
*                                  registers/memory that's being modified 
*               uint32_t word:     The word containing the current instruction
*       Returns:
*               none              
*/
void op_value(Emulator emulator, uint32_t word)
{ 
        assert(emulator);
        
        /* Set register and value */
        int a = Bitpack_getu(word, 3, 25);
        int val = Bitpack_getu(word, 25, 0);
        
        /* Set regster[a] to value */
        emulator->registers[a] = val;
}
