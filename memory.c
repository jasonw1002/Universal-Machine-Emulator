/**************************************************************
 *
 *                            memory.c
 *
 *     This file contains the implementation of the Memory
 *     struct, which consists of the address space (program 
 *     memory) and unmapped segment IDs. This file contains 
 *     functions that handle the address space by reading
 *     input to memory, mapping/unmapping memory, storing and
 *     loading values, and freeing memory.
 *     
 **************************************************************/

#include "memory.h"


/* Memory struct containing the memory segments and unmapped IDs. This struct
 * represents the memory of the UM. 
 */
struct Memory {
        Seq_T segMemory;
        Seq_T unmappedIDs;
};

/* Segment struct containing the segment words, number of words, and check if 
 * the segment is mapped. This struct represents a memory segment.
 */
typedef struct Segment {
        uint32_t *arr;
        int length;
        bool isMapped;
} *Segment;

/* 
*       Memory_new
*       Purpose: 
*               Initializes a new Memory struct by allocating memory.
*      Arguments:
*               none
*       Returns:
*               A new Memory struct
*/
Memory Memory_new()
{
        Memory mem;
        NEW(mem);
        mem->segMemory = Seq_new(100);
        mem->unmappedIDs = Seq_new(100);
        return mem;
}

/* 
*       Memory_free
*       Purpose:
*               Frees the memory allocated for the Memory struct
*       Arguments:
*               Memory *mem:       The address of the Memory struct being freed
*       Returns:
*               none              
*/
void Memory_free(Memory *mem) 
{
        assert(mem && *mem);
        
        /* Free all words in word segments */
        for (int i = 0; i < Seq_length((*mem)->segMemory); i++) {
                struct Segment *seg = 
                        (struct Segment*) Seq_get((*mem)->segMemory, i);
                FREE(seg->arr);
                FREE(seg);
        } 
        Seq_free(&((*mem)->segMemory));
        Seq_free(&((*mem)->unmappedIDs));
        FREE(*mem);
}

/* 
*       Memory_read
*       Purpose:
*               Reads in the 32 bit word instructions from the given file in 
*               big-endian order. Memory is allocated for m[0] of the Memory
*               struct, where the instructions are stored, based on fileSize
*       Arguments:
*               Memory mem:        The memory struct that instructions
*                                  are being added to
*               int fileSize:      The size of the file referenced by *fp in
                                   bytes
*               FILE *fp:          The pointer to the file contanining binary 
*                                  UM instructions
*       Returns:
*               none              
*/
void Memory_read(Memory mem, int fileSize, FILE *fp)
{
        assert(mem);
        assert(fp);
        
        /* Calculate number of instructions to initialize segment 0 */
        int length = fileSize / 4;
        struct Segment *seg0 = malloc(sizeof(struct Segment));
        assert(seg0);
        seg0->arr = (uint32_t*)malloc(length*sizeof(uint32_t));
        assert(seg0->arr);
        seg0->length = length;
        seg0->isMapped = true;
        
        /* Add instruction words to word segment 0 */
        uint32_t word = 0;
        for(int instructNum = 0; instructNum < length; instructNum++) {  
                /* Packs each instructions */
                for (int bitNum = 24; bitNum >= 0; bitNum -= 8) {
                        word = Bitpack_newu(word, 8, bitNum, getc(fp));
                }
                seg0->arr[instructNum] = word;
        }
        Seq_addhi(mem->segMemory, seg0);
}

/* 
*       map_seg
*       Purpose:
*               Maps a new segment containing the given number of words.
*               Initializes each word to 0 and generates an ID
*               for the segment that was previously unmapped
*       Arguments:
*               Memory mem:        The memory struct where the new segment will
*                                  be mapped inside
*               int numWords:      The number of words the new segment contains
*       Returns:
*               An unsigned integer representing the ID of the newly 
*               mapped segment            
*/
uint32_t map_seg(Memory mem, int numWords)
{
        assert(mem); 
            
        /* Initialize new word segment and check that resources exist */
        struct Segment *newSegment = malloc(sizeof(struct Segment));
        assert(newSegment);
        newSegment->arr = (uint32_t*)malloc(numWords*sizeof(uint32_t));
        assert(newSegment->arr);
        newSegment->length = numWords;
        newSegment->isMapped = true;
        for (int i = 0; i < numWords; i++) {
               newSegment->arr[i] = 0;
        }     
        
        /* Check if there is an unmapped ID that can be mapped to */
        if (Seq_length(mem->unmappedIDs) != 0) {
                uint32_t ID = (uint32_t) (uintptr_t) Seq_remlo(
                                                             mem->unmappedIDs);
                struct Segment *oldSegment = (struct Segment *) Seq_put(
                                                                mem->segMemory,
                                                                ID, 
                                                                newSegment);
                FREE(oldSegment->arr);
                FREE(oldSegment);
                return ID;
        } 
        else {
                /* Add new segment to next index if no unmapped IDs */
                uint32_t ID = Seq_length(mem->segMemory);
                Seq_addhi(mem->segMemory, newSegment); 
                return ID;  
        }
}

/* 
*       unmap_seg
*       Purpose:
*               Unmaps the segment in memory with the given ID. This 
*               ID can be reused by a future segment
*       Arguments:
*               Memory mem:        The memory struct where the segment being
*                                  unmapped lives
*               int ID:            The ID of the segment being unmapped
*       Returns:
*               none       
*/
void unmap_seg(Memory mem, int ID)
{
        assert(mem);    
        
        /* Check that ID is in range */
        assert(ID != 0 && ID < Seq_length(mem->segMemory));
        struct Segment *seg = (struct Segment*) Seq_get(mem->segMemory, ID);
        
        /* Check that the ID represents a mapped segment */
        assert(seg->isMapped);
        
        /* Add unmapped ID to sequence for later usage */
        Seq_addhi(mem->unmappedIDs, (void *)(uintptr_t)ID);
        seg->isMapped = false;  
}

/* 
*       store_val
*       Purpose:
*               Stores the a word in the segment of memory at the given ID 
*               and offset
*       Arguments:
*               Memory mem:        The Memory struct where the segment the 
*                                  word will be stored in live
*               int ID:            The ID of the segment the word will be 
*                                  stored in 
*               int offset:        The index within the segment that the 
*                                  word will be stored in
*               uint32_t word:     The word being stored in memory   
*       Returns:
*              none               
*/
void store_val(Memory mem, int ID, int offset, uint32_t word)
{
        assert(mem);   
        assert(ID >= 0 && ID < Seq_length(mem->segMemory));
        
        /* Get desired segment */
        struct Segment *seg = (struct Segment *) Seq_get(mem->segMemory, ID);
        assert(offset < seg->length);
        assert(seg->isMapped);
        
        /* Store word */
        seg->arr[offset] = word;
}

/* 
*       load_val
*       Purpose:
*               Returns the value of the word stored in the segment of memory
*               with the given ID and offset
*       Arguments:
*               Memory mem:        The Memory struct where the segment the 
*                                  word wll be loaded from lives
*               int ID:            The ID of the segment the word is stored in
*               int offset:        The index within the segment that the 
*                                  word is stored at
*       Returns:
*               none             
*/
uint32_t load_val(Memory mem, int ID, int offset)
{
        /* Raise error if arguments are NULL or invalid */
        assert(mem);
        assert(ID >= 0 && ID < Seq_length(mem->segMemory));

        /* Get segment at given ID in memory */
        struct Segment *seg = (struct Segment *) Seq_get(mem->segMemory, ID);
        assert(offset < seg->length);
        assert(seg->isMapped);

        /* Return desired word in segment */
        return seg->arr[offset];
}

/* 
*       load_program
*       Purpose:
*               Maps m[0] to a copy of the segment with the given ID to begin
*               running the instructions, starting from the given counter
*       Arguments:
*               Memory mem:        The Memory struct where the segment the 
*                                  program will be loaded from live
*               int ID:            The ID of the segment that will be
*                                  duplicated
*               int counter:       The index within the segment where the
*                                  instructions are being read from
*       Returns: 
*               Number of instructions in new m[0]             
*/
uint32_t load_program(Memory mem, int ID, int counter)
{
        assert(mem);
        assert(ID < Seq_length(mem->segMemory));

        /* Get desired segment */
        struct Segment *seg = (struct Segment*) Seq_get(mem->segMemory, ID);
        assert(counter < seg->length && seg ->isMapped); 
        if (ID != 0) {
                /* Create new segment 0 */
                struct Segment *newSeg0 = malloc(sizeof(struct Segment));
                assert(newSeg0);
                newSeg0->arr = 
                        (uint32_t*)malloc((seg->length)*sizeof(uint32_t));
                assert(newSeg0->arr);
                newSeg0->length = seg->length;
                newSeg0->isMapped = true;

                /* Copy over words from seg to newSeg0 */
                for(int i = 0; i < seg->length; i++) {
                        newSeg0->arr[i] = seg->arr[i];
                }

                /* replace m[0] with contents of m[ID] */
                struct Segment *oldSeg0 = 
                        (struct Segment *) Seq_put(mem->segMemory, 0, newSeg0);
                FREE(oldSeg0->arr);
                FREE(oldSeg0);
        }
        return seg->length;
}
