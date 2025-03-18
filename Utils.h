#ifndef UTILS_H
#define UTILS_H
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum address_type
{
    address_1,
    address_0_1_3,
    address_1_2,
    address_1_3,
    none_address
};

enum op_code
{

	mov_op = 0,

	cmp_op = 1,

 /* ----- two -----*/
	add_op = 2,
	sub_op = 2,
 /* ----- two -----*/

	lea_op = 4,

 /* ----- five -----*/
	clr_op = 5,
	not_op = 5,
	inc_op = 5,
	dec_op = 5,
 /* ----- five -----*/

   /* ----- nine -----*/
	jmp_op = 9,
	bne_op = 9,
	jsr_op = 9,
   /* ----- nine -----*/
	red_op = 12,

	prn_op = 13,

	rts_op = 14,

	stop_op = 15,

	none_oper_op = -1
};

enum operation_cmd
{
    mov,
    cmp,
    add,
    sub,
    lea,
    clr,
    not,
    inc,
    dec,
    jmp,
    bne,
    jsr,
    red,
    prn,
    rts,
    stop,
    none_oper
};
 enum funct {
	/* opcode 2 */
	add_funct = 1,
	sub_funct = 2,

	/* opcode 5 */
	clr_funct = 1,
	not_funct = 2,
	inc_funct = 3,
	dec_funct = 4,

	/* opcode 9 */
	jmp_funct = 1,
	bne_funct = 2,
	jsr_funct = 3,

	/* Default Erorr or function dont need funct*/
	none_oper_funct = 0
};

typedef struct op_struct
{
    enum operation_cmd operationNum;
    enum address_type allowedOrigTypes;
    enum address_type allowedDstTypes;
	enum funct operationFunct;
    enum op_code numOfOperators;
} operation_info;

extern operation_info operations[];

int is_empty_string(char *str);

char *pointer_to_first_char(char *str);

char *format_string(char *str);

char *parse_line(char *line, int *amountOflines, int *error);

operation_info return_opertaion(char *macroName);

int return_register_name(char *macroName);

char *add_ext_to_file(char *fileName,char *extension);
#endif //UTILS_H
