#include "Utils.h"

operation_info operations[] = {
    {mov, address_0_1_3, address_1_3   , none_oper_funct ,mov_op },
    {cmp, address_0_1_3, address_0_1_3 , none_oper_funct ,cmp_op },
    {add, address_0_1_3, address_1_3   , add_funct       ,add_op },
    {sub, address_0_1_3, address_1_3   , sub_funct       ,sub_op },
    {lea, address_1    , address_1_3   , none_oper_funct ,lea_op },
    {clr, none_address , address_1_3   , clr_funct       ,clr_op },
    {not, none_address , address_1_3   , not_funct       ,not_op },
    {inc, none_address , address_1_3   , inc_funct       ,inc_op },
    {dec, none_address , address_1_3   , dec_funct       ,dec_op },
    {jmp, none_address , address_1_2   , jmp_funct       ,jmp_op },
    {bne, none_address , address_1_2   , bne_funct       ,bne_op },
    {jsr, none_address , address_1_2   , jsr_funct       ,jsr_op },
    {red, none_address , address_1_3   , none_oper_funct ,red_op },
    {prn, none_address , address_0_1_3 , none_oper_funct ,prn_op },
    {rts, none_address , none_address  , none_oper_funct ,rts_op },
    {stop, none_address, none_address  , none_oper_funct ,stop_op},
};
operation_info emptyOper = {none_oper, none_address, none_address, none_oper_funct, none_oper_op};

int is_empty_string(char *str)
{/*Function that Check if the whole line is white chars*/
      int i=0;
      while(str[i]=='\0')
      {
          if(!isspace(str[i]))
          {
              return 0;
          }
          i++;
      }
      return 1;
}

char *pointer_to_first_char(char *str)
{/*Function that return pointer to the first none white char in a string*/
  while(*str != '\0' && (isspace(*str) || !isprint(*str)))
  {
    str++;
  }
  return str;
}

char *format_string(char *str)
{/* Function that splits the at the first space or newline. */
    char *newString = (char*)malloc((strlen(str)+1)*sizeof(char));
    strcpy(newString, pointer_to_first_char(str));
    newString[strcspn(newString, " ")] = '\0';
    newString[strcspn(newString, "\n")] = '\0';
    return newString;
}

operation_info return_opertaion(char *macroName)
{ /*Function that check if a given macro name has a command name*/
    int i;
    char operatorArray[16][6] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};

    for (i = 0; i < sizeof(operatorArray); i++)
    {
        if (strcmp(macroName, operatorArray[i]) == 0)
        {
            return operations[i];
        }
    }
    return emptyOper;
}

int return_register_name(char *macroName)
{/*Function that check if a given macro name has a register name*/
  	if(macroName[0]== 'r')
  	{
    	if(strlen(macroName+1)==1 &&((macroName[1]>='0') && (macroName[1]<='7')))
    	{
      		return macroName[1]-'0';
    	}
  	}
	return -1;
}

char *parse_line(char *line, int *amountOflines, int *error)
{
	/*TODO*/
}