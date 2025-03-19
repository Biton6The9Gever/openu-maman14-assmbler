#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Length Barriers */
#define MAX_LINE_LENGTH 80+1 /* +1 for the \n char in the end of the line*/
#define MAX_LABEL_LENGTH 31
#define MEMORY_START

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

#endif /*CONSTANTS_H*/
