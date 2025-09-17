#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Length Barriers */
#define MAX_LINE_LENGTH 80+1 /* +1 for the \n char in the end of the line*/
#define MAX_LABEL_LENGTH 31
#define MAX_NUM_OF_CMDS 4
#define ASSEMBELED_LINE_LENGTH 24 /*amount of bits*/

/* Macro Marks */
#define MACRO_START_MARK "mcro"
#define MACRO_END_MARK   "mcroend"

/* Label Marks*/
#define DATA_MARK ".data"
#define STRING_MARK ".string"
#define ENTRY_MARK ".entry"
#define EXTERN_MARK ".extern"

/* File Extensions */
#define ORIGINAL_EXT ".as"
#define AFTER_PREASSEMBLER_EXT ".am"
#define AFTER_SECOND_PASS_EXT ".ob"
#define EXTERNAL_EXT ".ext"
#define ENTRY_EXT ".ent"
#define FIRSTSCAN_TEMP_FILE_NAME "first_scan.temp"

/* Macro Methods */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/* Bits Sizes */
#define A_SIZE 1
#define R_SIZE 1
#define E_SIZE 1
#define FUNCT_SIZE 5
#define REGISTER_DESTINATION_SIZE 3
#define REGISTER_ORIGIN_SIZE 3
#define ADDRESS_DESTINATION_SIZE 2
#define ADDRESS_ORIGIN_SIZE 2
#define OPCODE_SIZE 6

/* Other */
#define MEMORY_START 100
#define ONE_BINARY '1'
#define ZERO_BINARY '0'
#define ARE_FIRST_WORD "100"
#define LABEL_SIGN "&"
#endif /*CONSTANTS_H*/
