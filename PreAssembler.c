#include "PreAssembler.h"
#include "Constants.h"
#include "Utils.h"
/*
    This file contains the Pre Assmbler code
    In this code we go through the inputs files and parse all the macros
    And detect minor Errors
*/
    int parse_macro(FILE *inputFile,FILE *outputFile)
    {
        macroList *macros =NULL; /*Will contain the current macro*/
        macroList *head = macros;
        enum macro_status currentState = copy;
        char *orignLine = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
        char *workingLine = orignLine; /*This line will get edited and worked with so we can process it without damaging the Orign line*/
        int macroSize=0,errorAmount=0,macroStartLine=0,macroEndLine=0,numOfLinesFromParseLine = 0;
        int currentLine=0;
            while(fgets(workingLine,MAX_LINE_LENGTH,inputFile)!= NULL)
            {
                int error = 0;
                macroList *macroFound =NULL;
                if(currentState != count )
                {
                    currentLine++;
                }

                if(is_empty_string(workingLine) || pointer_to_first_char(workingLine)[0] == ';')
                {
                  continue;
                }

                workingLine = pointer_to_first_char(workingLine);
/*                printf("wokring line: %s\n length %lu \n : eend mark len: %lu ",workingLine,strlen(workingLine),strlen(MACRO_END_MARK));*/
                if((macroFound = check_if_macro_exist(head,workingLine)) != NULL ) /*macro exist replace the name with the code*/
                {
                    fprintf(outputFile,"%s", macroFound->data);
                    /*if the macro data is either nothing or '\n'*/
                    if (*macroFound->data != '\n' && *macroFound->data != '\0')
                        fprintf(outputFile, "\n");
                }
                else if((strncmp(workingLine,MACRO_START_MARK,strlen(MACRO_START_MARK)) == 0) && (strlen(format_string(workingLine)) == strlen(MACRO_START_MARK)))
                {
                   /*we entered macro declaration */

                    char *macroName;
                    if(check_if_macro_exist(head,workingLine+strlen(MACRO_START_MARK)) != NULL)
                    {
                      printf("ERROR | cannot declare macro twice | Line: %d\n",currentLine);
                      errorAmount++;
                    }

                    currentState = count;
                    macroStartLine = ftell(inputFile);

                    if(macros== NULL)
                    {
                        macros= (macroList*)malloc(sizeof(macroList));
                        if(head==NULL)
                        {
                          head = macros;
                        }
                    }
                    else
                    {
                        macros->next = (macroList*)malloc(sizeof(macroList));
                        macros=macros->next;
                    }

                    macroName = (char*)malloc((strlen(workingLine)+1)*sizeof(char));
                    strcpy(macroName , pointer_to_first_char(workingLine + strlen(MACRO_START_MARK)));

                    macroName[strcspn(macroName,"\n")] = '\0'; /*remove newline char from macroName*/
                    macros->name = macroName;
                    macros->data = NULL;
                    macros->next = NULL;

                    if(is_empty_string(workingLine))
                    {/*macro name is empty illegal declaration*/
                        printf("ERROR | cannot set empty macro name | Line: %d\n",currentLine);
                        errorAmount++;
                        continue;
                    }
                    else if(return_opertaion(macros -> name).operationName != none_oper)
                    {/*check if the macro name is operation name illegal declaration*/
                        printf("ERROR | cannot set macro name same as a command | Line: %d\n",currentLine);
                        errorAmount++;
                        continue;
                    }
                    else if(return_register_name(macros->name)!= -1)
                    {
                      	printf("ERROR | cannot set macro name same as a register | Line: %d\n",currentLine);
                        errorAmount++;
                        continue;
                    }
                    else if (return_direct_value(macros->name)!= -1)
                    {
                        printf("ERROR | cannot set macro name same as a direct number | Line: %d\n",currentLine);
                        errorAmount++;
                        continue;
                    }


                }
                    /*                                                  plus +2 because of ניסוי וטעיה  plus +1 for \n */
                else if((strncmp(workingLine,MACRO_END_MARK,strlen(MACRO_END_MARK)) == 0) &&  (strlen(workingLine) == strlen(MACRO_END_MARK) + 2|| strlen(workingLine) == strlen(MACRO_END_MARK) + 1))
                {/*+1 because of the new line \n char*/
                  	if(currentState==count)
                   {
                    	macroSize=MAX(macroEndLine-macroStartLine,0);
                        fseek(inputFile,macroStartLine,SEEK_SET);
                        currentState = save;
                        macros ->data= (char *)malloc(macroSize+1);
                        *(macros->data)=0;
                   }
                   else
                   {
                    	if (macros->data[strlen(macros->data) - 1] == '\n')
                		{/*if the end of the line is a new line char removes it*/
                    		macros->data[strlen(macros->data) - 1] = '\0';
                    	}
                   		currentState = copy;
                   }
                }
                else if(currentState == count)
                {
                  	macroEndLine = ftell(inputFile);
                }
                else if(currentState == save)
                {
                    strcat(macros->data,workingLine);
                    free(parse_line(workingLine,&numOfLinesFromParseLine,&error));
                }
                else if (currentState == copy)
                {
                    fprintf(outputFile,"%s",workingLine);
                    free(parse_line(workingLine,&numOfLinesFromParseLine,&error));
                }

                if (error ==-1)
                {
                    printf("ERROR | Missing info | Line: %d\n",currentLine);
                    errorAmount++;
                }
                else if (error==-2 || error ==-3)
                {
                    printf("ERROR | Invalid line | Line: %d\n",currentLine);
                    errorAmount++;
                }
                else if (error == -4)
                {
                    printf("ERROR | Invalid operand | Line: %d\n",currentLine);
                    errorAmount++;
                }

            }

        /*free(orignLine);*/
        /*free_macro_list(head);*/
        return 0;
    }



    void print_macro_list(macroList *macros)
    {/*Function that Prints the macro list*/
        /*this method used for debugging*/
        puts("------MACRO-----");
        while(macros != NULL)
        {
          printf("name: %s data: %s\n", macros->name, macros->data);
          macros=macros->next;
        }
        puts("------MACRO-----");
    }

    void free_macro_list(macroList *macros)
    {/*Function that realse all the Macroes */
        macroList *tmp;
        while(macros != NULL)
        {
            tmp = macros;
            macros = macros->next;

            if(tmp->data != NULL)
                free(tmp->data);
            if(tmp->next != NULL)
                free(tmp->next);
            free(tmp);
        }

    }

    macroList *check_if_macro_exist(macroList *head,char *name)
    {
        macroList *tmp = head ;
        char *macroNameCopy = format_string(name);

            while (tmp != NULL)
            {
                if ((strncmp(tmp->name, macroNameCopy, strlen(tmp->name)) == 0) && (strlen(tmp->name) == strlen(macroNameCopy)))
                {
                    free(macroNameCopy);
                    return tmp;
                }
                tmp = tmp->next;
            }
        free(macroNameCopy);
        return NULL;
    }


