#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_LINE_LENGTH 80+1 /* +1 for the \n char in the end of the line*/

#define MACRO_START "mcro"
#define MACRO_END   "mcroend"

#define ORIGINAL_EXT ".as"
#define AFTER_PREASSEMBLER_EXT ".am"
#define AFTER_SECOND_PASS_EXT ".ob"
#define EXTERNAL_EXT ".ext"
#define ENTRY_EXT ".ent"


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#endif //CONSTANTS_H
