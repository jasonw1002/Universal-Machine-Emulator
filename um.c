/**************************************************************
 *
 *                              um.c
 *
 *     The main function which reads in the binary instructions
 *     file and runs the UM Emulator.
 *
 **************************************************************/

#include "instructions.h"

int main(int argc, char *argv[])
{
        FILE *fp = NULL;
        char *filename = "";
        struct stat fileInfo; 

        /* Check if correct number of arguments given */
        if(argc != 2) {
                fprintf(stderr, "Incorrect number of arguments\n");
                exit(EXIT_FAILURE);
        } else {
                filename = argv[1];
                fp = fopen(filename, "r");
                stat(filename, &fileInfo);
        } 
 
        /* Raise error if file does not exist or stdin not provided */
        if(fp == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", filename);
                exit(EXIT_FAILURE);
        }

        /* initialize the emulator with the file pointer*/
        Emulator emulator = Emulator_new((int)fileInfo.st_size, fp);
        
        /* Close file */
        fclose(fp);

        /* Run emulator, processing all instructions */
        run_emulator(emulator);
        
        /* Free memory*/
        Emulator_free(&emulator);
}
