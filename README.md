# Universal-Machine-Emulator

The program uses a file called "bitpack.h" made specifically to store instructions for the Universal Machine into 32-bit words. \newline
  3-bit integers a, b, and c are used to determine which of the 8 registers to manipulate
  The UM operation being called is encoded in a 4-bit integer retrived from the beginning of the word with Bitpack_getu(word, 4, 28)
  25-bit values to store in the register can be obtained from Bitpack_getu(word, 25, 0)
