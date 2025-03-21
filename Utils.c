#include "Utils.h"

#include "Constants.h"

operation_info operations[] = {
    {mov, address_0_1_3, address_1_3   , none_oper_funct ,mov_op ,operand_2},
    {cmp, address_0_1_3, address_0_1_3 , none_oper_funct ,cmp_op ,operand_2},
    {add, address_0_1_3, address_1_3   , add_funct       ,add_op ,operand_2},
    {sub, address_0_1_3, address_1_3   , sub_funct       ,sub_op ,operand_2},
    {lea, address_1    , address_1_3   , none_oper_funct ,lea_op ,operand_2},
    {clr, none_address , address_1_3   , clr_funct       ,clr_op ,operand_1},
    {not, none_address , address_1_3   , not_funct       ,not_op ,operand_1},
    {inc, none_address , address_1_3   , inc_funct       ,inc_op ,operand_1},
    {dec, none_address , address_1_3   , dec_funct       ,dec_op ,operand_1},
    {jmp, none_address , address_1_2   , jmp_funct       ,jmp_op ,operand_1},
    {bne, none_address , address_1_2   , bne_funct       ,bne_op ,operand_1},
    {jsr, none_address , address_1_2   , jsr_funct       ,jsr_op ,operand_1},
    {red, none_address , address_1_3   , none_oper_funct ,red_op ,operand_1},
    {prn, none_address , address_0_1_3 , none_oper_funct ,prn_op ,operand_1},
    {rts, none_address , none_address  , none_oper_funct ,rts_op ,operand_0},
    {stop, none_address, none_address  , none_oper_funct ,stop_op,operand_0},
};
operation_info emptyOper = {none_oper, none_address, none_address, none_oper_funct, none_oper_op,operand_0};

int is_empty_string(char *str)
{/*Function that Check if the whole line is white chars*/
      int i=0;
      while(str[i]!='\0')
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

operation_info return_opertaion(char *name)
{ /*Function that check if a given macro/label name has a command name*/
    int i;
    char operatorArray[16][6] = {
        "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
        "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"
    };

    for (i = 0; i < sizeof(operatorArray); i++)
    {
        if (strcmp(name, operatorArray[i]) == 0)
        {
            return operations[i];
        }
    }
    return emptyOper;
}

int return_register_name(char *name)
{/*Function that check if a given label/macro name has a register name*/
  	if(name[0]== 'r')
  	{
    	if(strlen(name+1)==1 &&(name[1]>='0' && name[1]<='7'))
    	{
      		return name[1]-'0';
    	}
  	}
	return -1;
}

char *parse_line(char *line, int *amountOflines, int *status)
{
	char *workingStr = pointer_to_first_char(line);
    char *strForToken = malloc((strlen(workingStr) + 1) * sizeof(char));
    char *token,*tmp,*parsedStr=NULL,*operStr=NULL;
    int labelNameLength = is_label_line(workingStr);
    operation_info operation;
    *status =0;
    *amountOflines = 0;
    strcpy(strForToken, workingStr+labelNameLength);
    token = strtok(strForToken, "\n\t");
    if (token == NULL)
    {
        *status =-1; /*missing info*/
        free(strForToken);
        return parsedStr;
    }
    tmp =pointer_to_first_char(token);
    if (strcmp(tmp, DATA_MARK) == 0 || strcmp(tmp, STRING_MARK) == 0
        || strcmp(tmp, ENTRY_MARK) == 0 || strcmp(tmp, EXTERN_MARK) == 0)
    {
        *status = -2; /*Invalid operation*/
        free(strForToken);
        return parsedStr;
    }
    free(tmp);
    operation = return_opertaion(token);
    operStr =token;
    /*Function as funct*/
    if (operation.operationNumber == none_oper)
    {
        *status =-2; /*Invalid operation*/
        free(strForToken);
        return parsedStr;
    }
    parsedStr = parse_cmd();
    return "skibidi";
}

char *add_ext_to_file(char *fileName,char *extension)
{/*Function that add extenstion to a file*/
	  char *newFileName;
      newFileName=(char *)malloc((strlen(fileName)+strlen(extension)+1)*sizeof(char));
	  strcpy(newFileName, fileName); /*copy the new*/
      strcat(newFileName, extension); /*add extension*/
      return newFileName;
}


int is_label_line(char *str)
{/*Function that check if there is label in the line */
    int i,labelLength =0;
    if (!isalpha(str[0]))/*first char in label name must be a letter*/
    {
        return 0;
    }
    for (i=0; i <MAX_LABEL_LENGTH;i++)
    {
        if(str[i] == ':')
        {
            return labelLength;
        }
        if(isspace(str[i]))
        {
            return 0;
        }
        if(isalpha(str[i]) || isdigit(str[i]))
        {
            labelLength++;
        }
        else /*There is !  @ or other Special char in the name*/
        {
            return 0;
        }
    }
    if(str[i]==':')/*In case that the label length is 31*/
    {
        return labelLength;
    }
    return 0;
}

int return_direct_value(char *name)
{
    /*Function that check if a given label/macro name has a direct number #{num}*/
    if(name[0]== '#')
    {
        if(strlen(name+1)==1 &&(name[1]>='0' && name[1]<='9'))
        {
            return name[1]-'0';
        }
    }
    return -1;
}

int count_char_in_string(char *str, char ch)
{
    int counter=0;
    while (*str != '0')
    {
        if (*str==ch)
        {
            counter++;
        }
        str++;
    }
    return counter;
}

char *parse_to_binary(int num, int size)
{
    char *fullStr = (char *)calloc(size + 1, sizeof(char));
    int i=size-1;
    for(;i>=0;i--)
    {
        fullStr[i] = (num & 1) ? ONE_BINARY: ZERO_BINARY;
        num >>= 1; /*Right shift*/
    }

    return fullStr;
}

char *parse_first_word(int funct,int destinationRegister,int destinationAddress, int originRegister,int originAddress , int opcode)
{
    char *_return = (char *)calloc( (ASSEMBELED_LINE_LENGTH+1)*MAX_CMD_AMOUNT +1 , sizeof(char));
    char *temp;

    temp = parse_to_binary(opcode, OPCODE_SIZE);
    strcat(_return, temp);
    free(temp);

    temp = parse_to_binary(originAddress, ADDRESS_ORIGIN_SIZE);
    strcat(_return, temp);
    free(temp);

    temp = parse_to_binary(originRegister, REGISTER_ORIGIN_SIZE);
    strcat(_return, temp);
    free(temp);

    temp = parse_to_binary(destinationAddress, ADDRESS_DESTINATION_SIZE);
    strcat(_return, temp);
    free(temp);

    temp = parse_to_binary(destinationRegister, REGISTER_DESTINATION_SIZE);
    strcat(_return, temp);
    free(temp);

    temp = parse_to_binary(funct, FUNCT_SIZE);
    strcat(_return, temp);
    free(temp);

    strcat(_return,"100");
    return _return;
}

char *parse_cmd(int *amountOfLines, operation_info operation, int *status, char *wholeStr)
{
    int destinationRegister=0, destinationAddress=0,  originRegister=0, originAddress =0;
    char *operStr=(char*)calloc((ASSEMBELED_LINE_LENGTH+1)*MAX_CMD_AMOUNT+1,sizeof(char));
    char *parsedLine=(char*)calloc((ASSEMBELED_LINE_LENGTH+1)*(MAX_CMD_AMOUNT-1)+2,sizeof(char));
    *(amountOfLines) = 0;
    char *token;
    if (operation.attributeAmount == operand_0 )
    {
        char *firstWord = parse_first_word(operation.operationFunct,0,0,0,0,operation.operationNumber);
        if (count_char_in_string(wholeStr, ',') != 0)
        {
            *status = -3; /* invalid line */
        }
        (*amountOfLines)++;
        strcat(parsedLine,firstWord);
        free(firstWord);
        if (operStr !=NULL)
            free(operStr);

    }
    if (operation.attributeAmount == operand_1)
    {
        char *operandStr;
        if (count_char_in_string(wholeStr, ',') != 0)
        {
            *status = -3; /* invalid line */
        }
        token = strtok(wholeStr," ,\n");
        if (token == NULL)
        {
            *status = -1;/*missing info*/
            if (parsedLine != NULL)
                free(parsedLine);
            if (operStr !=NULL)
                free(operStr);
            return NULL;
        }
        if (parse_attribute_string(token,&destinationAddress,&destinationRegister)==-1)
        {
            *status = -4; /*Invalid operand*/
        }
        *amountOfLines = 2;
        operandStr =NULL;
        if ( destinationAddress ==3)/*we also pass a register*/
        {
            char *firstWord = parse_first_word(operation.operationFunct,destinationRegister,destinationAddress,0,0,operation.operationNumber);
            strcat(operandStr,firstWord);
            free(firstWord);
        }
        else if (destinationAddress ==1 || destinationAddress ==0)
        {
            char *firstWord = parse_first_word(operation.operationFunct,0,destinationAddress,0,0,operation.operationNumber);
            strcat(operandStr,firstWord);
            free(firstWord);
        }
        if (destinationAddress ==0)
        {
            char *temp =parse_to_binary(0,2);
            operandStr =parse_to_binary(destinationRegister,ASSEMBELED_LINE_LENGTH-2);
            strcat(operStr,"\n");
            strcat(operStr,operandStr);
            strcat(operStr,temp);
            free(temp);
            strcat(parsedLine,operStr);
        }
        else if (destinationAddress ==1)
        {
            strcat(operStr, "\n");
            strcat(operStr, LABEL_SIGN);
            strcat(operStr, token);
            strcat(parsedLine, operStr);
        }
        else if (destinationAddress ==3 )
        {
            char *temp =parse_to_binary(0,8);
            operandStr =parse_to_binary(destinationRegister,6);
            strcat(operStr,"\n");
            strcat(operStr,operandStr);
            strcat(operStr,temp);
            free(temp);
            strcat(parsedLine,operStr);
        }
        if (operandStr !=NULL)
            free(operandStr);
    }
    else if (operation.attributeAmount == operand_2)
    {
        char *firstWord,firstOper,*secondOper,*secondToken;
        int temp=count_char_in_string(wholeStr,',');

        if (temp !=1)
        {
            *status = -3; /* invalid line */
        }
        token =strtok(NULL," ,\n");
        if (token !=NULL)
        {
            *status = -4; /*Invalid operand*/
            if (parsedLine != NULL)
                free(parsedLine);
            if (operStr !=NULL)
                free(operStr);

            return NULL;
        }
        if (parse_attribute_string(token,&destinationAddress,&destinationRegister)==-1)
        {
            *status = -4; /*Invalid operand*/
        }
        secondToken=strtok(NULL," ,\n");
        if (secondToken !=NULL)
        {
            *status = -4; /*Invalid operand*/
            if (parsedLine != NULL)
                free(parsedLine);
            if (operStr !=NULL)
                free(operStr);

            return NULL;
        }
        if (parse_attribute_string(secondToken,&destinationAddress,&destinationRegister)==-1)
        {
            *status = -4; /*Invalid operand*/
        }
        *amountOfLines = 3;
        firstWord =parse_first_word(operation.operationFunct,destinationRegister,);
    }

}

int parse_attribute_string(char *str, int *addressType,int *regOrNum)
{/*function that return the type of the attribute*/
    if (str[0]=='#')
    {
        if (strlen(str)<2)
        {
            *addressType = -1;
            return -1;
        }
        *addressType = 0;
        *regOrNum =strtol(str+1,NULL,10);
        return 0;
    }
    if (str[0]=='r')
    {
        if (strlen(str)<2)
        {
            *addressType = -1;
            return -1;
        }
        *addressType =3;
        *regOrNum =strtol(str+1,NULL,10);
        if (*regOrNum>7 || *regOrNum<0) {
            *addressType = -1;
            return -1;
        }
        return 0;
    }
    /*label or invalid*/
    *addressType =1;
    return 0;
}

