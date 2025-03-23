#include "Utils.h"

#include "Constants.h"
#include "FirstScan.h"

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
};/*we can call it cmds data base*/
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

enum labelType return_dot_type(char * str);

char *parse_line(char *line, int *amountOflines, int *status)
{
	char *workingStr = pointer_to_first_char(line);
    char *strForToken = malloc((strlen(workingStr) + 1) * sizeof(char));
    char *token,*parsedStr=NULL;
    int labelNameLength = is_label_line(workingStr);
    operation_info operation;
    *status =0;
    *amountOflines = 0;
    strcpy(strForToken, workingStr+labelNameLength);
    token = strtok(strForToken, "\n\t: ");
    if (token == NULL)
    {
        *status =-1; /*missing info*/
        /*free(strForToken);*/
        return parsedStr;
    }
    if (return_dot_type(token)!= code)
    {
        *status = -2; /*Invalid operation*/
        free(strForToken);
        return parsedStr;
    }
    operation = return_opertaion(token);
    /*Function as funct*/
    if (operation.operationNumber == none_oper_op)
    {
        *status =-2; /*Invalid operation*/
        return parsedStr;
    }
    if (operation.operationName == jmp || operation.operationName == bne || operation.operationName == jsr)
        parsedStr = parse_JMP_BNE_JSR(amountOflines,operation,status,workingStr);
    else {
        parsedStr = parse_cmd(amountOflines,operation,status,workingStr);

    }
    if (*status <0) {
        free(strForToken);
        return parsedStr;
    }

    return parsedStr; /*won't get here*/
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
{/*function that return the value of a direct number like #3*/
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
{/*function that count the amount of chars in the line*/
    int counter=0;
    while (*str != '\0')
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
{/*function that get number and a size and parse the number in this long char size*/
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
{/*function that parse the first word of a line*/
    char *_return = (char *)calloc( (ASSEMBELED_LINE_LENGTH+1)*MAX_NUM_OF_CMDS +1 , sizeof(char));
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
{/*function that parse the cmd based on the 23 bits model*/
    int destinationRegister=0, destinationAddress=0,  originRegister=0, originAddress =0;
    char *operStr=(char*)calloc((ASSEMBELED_LINE_LENGTH+1)*MAX_NUM_OF_CMDS+1,sizeof(char));
    char *parsedLine=(char*)calloc((ASSEMBELED_LINE_LENGTH+1)*(MAX_NUM_OF_CMDS-1)+2,sizeof(char));
    char *token;
    *(amountOfLines) = 0;

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
        return parsedLine;

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
            char *temp =parse_to_binary(0,12);
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
            char *temp =parse_to_binary(0,18);
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
        char *firstWord,*firstOper,*secondOper,*secondToken;
        int temp=count_char_in_string(wholeStr,',');
        if (temp !=1)
        {
            *status = -3; /* invalid line */
        }
        token =strtok(NULL," ,\n");
        if (token ==NULL)
        {
            *status = -4; /*Invalid operand*/
            if (parsedLine != NULL)
                free(parsedLine);
            if (operStr !=NULL)
                free(operStr);

            return NULL;
        }
        if (parse_attribute_string(token,&originAddress,&originRegister)==-1)
        {
            *status = -4; /*Invalid operand*/
        }
        secondToken=strtok(NULL," ,\n");
        if (secondToken ==NULL)
        {
            *status = -4; /*Invalid operand*/
            if (parsedLine != NULL)
                free(parsedLine);
            if (operStr !=NULL)
            {
                free(operStr);
            }

            return NULL;
        }
        if (parse_attribute_string(secondToken,&destinationAddress,&destinationRegister)==-1)
        {
            *status = -4; /*Invalid operand*/
        }
        *amountOfLines = 3;
        firstWord =parse_first_word(operation.operationFunct,destinationRegister,destinationAddress,originRegister,originAddress, operation.operationNumber);
        strcat(parsedLine,firstWord);
        free(firstWord);
        firstOper=NULL;
        secondOper=NULL;
        if (originAddress == 3)
        {
            if (destinationAddress ==3)
            {
                char *temp =parse_to_binary(0,12);
                (*amountOfLines)--;
                firstOper=parse_to_binary(originRegister,6);
                secondOper=parse_to_binary(destinationRegister,6);

                strcat(operStr,"\n");
                strcat(operStr,firstOper);
                strcat(operStr,secondOper);
                strcat(operStr,temp);
                strcat(parsedLine,operStr);

                free(temp);
                free(firstOper);
                free(secondOper);
            }
            else if (destinationAddress ==1)
            {
                char *temp =parse_to_binary(0,8);
                firstOper=parse_to_binary(originRegister,6);
                secondOper=secondToken;

                strcat(operStr,"\n");
                strcat(operStr,firstOper);
                strcat(operStr,temp);
                strcat(operStr,"\n");
                strcat(operStr,LABEL_SIGN);
                strcat(operStr,secondOper);
                strcat(parsedLine,operStr);

                free(temp);
                free(firstOper);
            }
            else if (destinationAddress ==0)
            {
                char *temp =parse_to_binary(0,8);
                firstOper=parse_to_binary(originRegister,6);
                secondOper=parse_to_binary(destinationRegister,ASSEMBELED_LINE_LENGTH-2);
                strcat(firstOper,temp);
                free(temp);

                temp = parse_to_binary(0,12);
                strcat(secondOper,temp);
                free(temp);

                strcat(operStr,"\n");
                strcat(operStr,firstOper);
                strcat(operStr,"\n");
                strcat(operStr,secondOper);
                strcat(parsedLine,operStr);

                free(firstOper);
                free(secondOper);
            }
        }

        else if (originAddress ==1)
        {
            if (destinationAddress ==3)
            {
                char *temp =parse_to_binary(0,6);
                secondOper=parse_to_binary(destinationAddress,6);

                strcat(operStr,"\n");
                strcat(operStr,LABEL_SIGN);
                strcat(operStr,token);
                strcat(operStr,"\n");
                strcat(operStr,temp);
                free(temp);

                temp = parse_to_binary(0,12);
                strcat(operStr,secondOper);
                strcat(operStr,temp);
                free(temp);
                strcat(parsedLine,operStr);

                free(secondToken);
            }
            else if (destinationAddress ==1)
            {
                strcat(operStr,"\n");
                strcat(operStr,LABEL_SIGN);
                strcat(operStr,token);
                strcat(operStr,"\n");
                strcat(operStr,LABEL_SIGN);
                strcat(operStr,secondToken);
                strcat(parsedLine,operStr);
            }
            else if (destinationAddress ==0)
            {
                char *temp =parse_to_binary(0,2);
                secondOper=parse_to_binary(destinationAddress,ASSEMBELED_LINE_LENGTH-2);

                strcat(secondOper,temp);
                free(temp);
                strcat(operStr,"\n");
                strcat(operStr,LABEL_SIGN);
                strcat(operStr,token);
                strcat(operStr,"\n");
                strcat(operStr,secondOper);
                strcat(parsedLine,operStr);

                free(secondOper);
            }
        }
        else if (originAddress ==0)
        {
            if (destinationAddress ==3)
            {
                char *temp =parse_to_binary(0,2);
                firstOper=parse_to_binary(originRegister,ASSEMBELED_LINE_LENGTH-2);
                secondOper=parse_to_binary(destinationRegister,6);

                strcat(firstOper,temp);
                free(temp);

                temp = parse_to_binary(0,6);
                strcat(operStr,"\n");
                strcat(operStr,firstOper);
                strcat(operStr,"\n");
                strcat(operStr,temp);
                free(temp);

                temp=parse_to_binary(0,12);
                strcat(operStr,secondOper);
                strcat(operStr,temp);
                strcat(parsedLine,operStr);

                free(temp);
                free(firstOper);
                free(secondOper);
            }
            else if (destinationAddress ==1)
            {
                char *temp =parse_to_binary(0,12);
                firstOper=parse_to_binary(originAddress,ASSEMBELED_LINE_LENGTH-2);

                strcat(firstOper,temp);
                free(temp);

                strcat(operStr,"\n");
                strcat(operStr,firstOper);
                strcat(operStr,"\n");
                strcat(operStr,LABEL_SIGN);
                strcat(operStr,secondToken);
                strcat(parsedLine,operStr);

                free(firstOper);
            }
            else if (destinationAddress ==0)
            {
                char *temp =parse_to_binary(0,12);

                firstOper=parse_to_binary(originAddress,ASSEMBELED_LINE_LENGTH-2);
                secondOper=parse_to_binary(destinationAddress,ASSEMBELED_LINE_LENGTH-2);

                strcat(firstOper,temp);
                strcat(secondOper,temp);

                free(temp);

                strcat(operStr,"\n");
                strcat(operStr,firstOper);
                strcat(operStr,"\n");
                strcat(operStr,secondOper);
                strcat(parsedLine,operStr);

                free(firstOper);
                free(secondOper);
            }
        }
    }
    free(operStr);
    return parsedLine;
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


char *parse_JMP_BNE_JSR(int *amountOfLines, operation_info operation, int *status, char *wholeStr)
{/*FUNCTION THAT PARSE JMP BNE AND JSR (oops caps) because of there model*/
    int destinationAddress=1,firstAttributeVal=0,firstAttributeType =0,secondAttributeVal=0,secondAttributeType=0;

    char *attributeStr=(char *)calloc((ASSEMBELED_LINE_LENGTH+1)*(MAX_NUM_OF_CMDS-1)+1,sizeof(char));

    char *parsedStr = (char *)calloc((ASSEMBELED_LINE_LENGTH+1)*MAX_NUM_OF_CMDS+2,sizeof(char));

    char *token=strtok(NULL, " :,()\n");
    int attributeType=0;
    int attributeVal=0;
    char *emptyLine= parse_to_binary(0,2);
    char *firstWord,*secondToken=NULL;

    if (token ==NULL)
    {
        *status= -4; /*invalid attribute*/
        if (emptyLine != NULL)
            free(emptyLine);
        if (parsedStr != NULL)
            free(parsedStr);
        if (attributeStr != NULL)
            free(attributeStr);
        return NULL;
    }

    if (parse_attribute_string(token,&attributeType,&attributeVal)==- 1)
    {
        *status= -4; /*invalid attribute*/
    }
    if (attributeType!=1)
    {
        *status= -4; /*invalid attribute*/
        if (emptyLine != NULL)
            free(emptyLine);
        if (parsedStr != NULL)
            free(parsedStr);
        if (attributeStr != NULL)
            free(attributeStr);
        return NULL;
    }
    else
    {
        (*amountOfLines)++;
        strcat(attributeStr,"\n");
        strcat(attributeStr,LABEL_SIGN);
        strcat(attributeStr,token);
        token =strtok(NULL, ",)");
    }
    if (token !=NULL) {
        if (count_char_in_string(wholeStr,',')!=1)
        {
            *status =- 3; /*invalid Line*/
        }
        destinationAddress =2;
        if (parse_attribute_string(token,&firstAttributeType,&firstAttributeVal)==- 1)
            *status= -4; /*invalid attribute*/
        secondToken=strtok(NULL, ",)");
        if (secondToken !=NULL)
        {
            if (parse_attribute_string(secondToken,&secondAttributeType,&secondAttributeVal)==- 1)
                *status= -4; /*invalid attribute*/
        }
        else
            *status = -5; /*invalid attribute */

        if (firstAttributeType==-1 && secondAttributeType==-1)
            *status = -5; /*invalid attribute */
        else
        {
            char *firstStr=NULL;
            char *secondStr=NULL;

            if (firstAttributeType==3 && secondAttributeType==3)
            {
                char *temp =parse_to_binary(0,12);
                (*amountOfLines)++;
                firstStr=parse_to_binary(firstAttributeVal,6);
                secondStr=parse_to_binary(secondAttributeVal,6);

                strcat(attributeStr,"\n");
                strcat(attributeStr,firstStr);
                strcat(attributeStr,secondStr);
                strcat(attributeStr,temp);
                free(temp);
            }
            else
            {
                (*amountOfLines)++;
                if (firstAttributeType !=1)
                {
                    char *temp =parse_to_binary(0,12);
                    strcat(attributeStr,"\n");
                    firstStr=parse_to_binary(firstAttributeVal,ASSEMBELED_LINE_LENGTH-2);
                    strcat(attributeStr,firstStr);
                    strcat(attributeStr,temp);
                    free(temp);
                }
                else
                {
                    strcat(attributeStr,"\n");
                    strcat(attributeStr,LABEL_SIGN);
                    strcat(attributeStr,token);
                }
                if (secondAttributeType !=1)
                {
                    char *temp =parse_to_binary(0,12);
                    strcat(attributeStr,"\n");
                    secondStr=parse_to_binary(secondAttributeVal,ASSEMBELED_LINE_LENGTH-2);
                    strcat(attributeStr,secondStr);
                    strcat(attributeStr,temp);
                    free(temp);
                }
                else
                {
                    strcat(attributeStr,"\n");
                    strcat(attributeStr,LABEL_SIGN);
                    strcat(attributeStr,secondToken);
                }
            }
            if (secondStr !=NULL)
                free(secondStr);
            if (firstStr !=NULL)
                free(firstStr);
        }
    }
    else
        if (count_char_in_string(wholeStr,',')!=0)
            *status= -3; /*invalid Line*/

    (*amountOfLines)++;
    firstWord = parse_first_word(operation.operationFunct,MAX(firstAttributeVal,0),destinationAddress,MAX(secondAttributeVal,0),0,operation.operationNumber);
    strcat(parsedStr,firstWord);
    free(firstWord);
    strcat(parsedStr,attributeStr);

    if (emptyLine != NULL)
        free(emptyLine);
    if (attributeStr != NULL)
        free(attributeStr);
    return parsedStr;
}

enum labelType return_dot_type(char *str) {
    str = pointer_to_first_char(str);

     if (strcmp(str, DATA_MARK) == 0) /* there is a .data */
        return data;
     if (strcmp(str, STRING_MARK) == 0) /* there is a .string */
        return string;
     if (strcmp(str, EXTERN_MARK) == 0) /* there is a .data */
        return external;
     if (strcmp(str, ENTRY_MARK) == 0) /* there is a .string */
        return entry;
    return code;
}
