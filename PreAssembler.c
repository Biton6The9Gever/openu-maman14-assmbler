#include "PreAssembler.h"
#include "Constants.h"
#include "Utils.h"
/*
    This file contains the Pre Assmbler code
    In this code we go trought the inputs files and parse all the macroes
    And dedct minor Errors
*/
    int parse_macro(FILE *inputFile,FILE *outputFile)
    {
        macroList *macros =NULL; /*Will contain the current macro*/
        macroList *head = macros;
        enum macro_status currentState = copy;
        char *orignLine = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
        char *workingLine = orignLine; /*This line will get edited and worked with so we can process it without demaging the Orign line*/
        int macroSize=0,errorAmount=0,macroStartLine=0,macroEndLine=0,numOfLinesFromParseLine = 0;
        int currentLine=0;

            while(fgets(workingLine,MAX_LINE_LENGTH,inputFile)!= NULL)
            {
                int error = 0;

                char *temp=NULL;
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

                if((macroFound = check_if_macro_exist(head,workingLine)) != NULL ) /*macro exist replace the name with the code*/
                {
                    fprintf(outputFile,"%s", macroFound->data);

                    /*if the macro data is either nothing or '\n'*/
                    if (*macroFound->data != '\n' && *macroFound->data != '\0')
                        fprintf(outputFile, "\n");
                }
                else if((strncmp(workingLine,MACRO_START,strlen(MACRO_START)) == 0) && (strlen(temp = format_string(workingLine)) == strlen(MACRO_START)))
                {
                   /*we entered macro declartion */

                    char *macroName;
                    if(currentState==copy)
                    {
                      printf("ERROR | cannot declare macro inside a macro | Line: %d\n",currentLine);
                      errorAmount++;
                    }
                    if(check_if_macro_exist(head,workingLine+strlen(MACRO_START)) != NULL)
                    {
                      printf("ERROR | cannot declare macro twice | Line: %d\n",currentLine);
                      errorAmount++;
                    }

                    currentState = copy;
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



                    if(is_empty_string(workingLine))
                    {/*macro name is empty illegal declartion*/
                        printf("ERROR | cannot set empty macro name | Line: %d\n",currentLine);
                        errorAmount++;
                    }
                    if(return_opertaion(macros -> name).operationNum != none_oper)
                    {/*check if the macro name is operation name illegal declartion*/
                        printf("ERROR | cannot set macro name same as a command | Line: %d\n",currentLine);
                        errorAmount++;
                    }
                    if(return_register_name(macros->name)!= -1)
                    {
                      	printf("ERROR | cannot set macro name same as a register | Line: %d\n",currentLine);
                        errorAmount++;
                    }
                    macroName = (char*)malloc((strlen(workingLine)+1)*sizeof(char));
                    strcpy(macroName , pointer_to_first_char(workingLine + strlen(MACRO_START)));

                    macroName[strcspn(macroName,"\n")] = '\0'; /*remove newline char from macroName*/
                    macros->name = macroName;
                    macros->data = NULL;
                    macros->next = NULL;


                }
                else if((strncmp(workingLine,MACRO_END,strlen(MACRO_END)) == 0) &&strlen(workingLine) == strlen(MACRO_END) + 1)
                {/*+1 because of the new line \n char*/
                  	if(currentState==count)
                   {
                    	macroSize=MAX(macroEndLine-macroStartLine,0);
                        fseek(inputFile,macroStartLine,SEEK_SET);
                        currentState = copy;
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
                else if(currentState == copy)
                {
                  	macroEndLine = ftell(inputFile);
                }
                else if(currentState == save)
                {
                    /*TODO*/
                }
                else if (currentState == copy)
                {
                    /*TODO*/
                }
            }
        return 69;
    }



    void print_macro_list(macroList *macros)
    {/*Function that Prints the macro list*/
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
        macroList *temp;
        while(macros != NULL)
        {
            temp = macros;
            macros = macros->next;

            if(temp->data != NULL)
                free(temp->data);
            if(temp->next != NULL)
                free(temp->next);
            free(temp);
        }

    }

    macroList *check_if_macro_exist(macroList *head,char *name)
    {
        macroList *temp = head ;
        char *macroName = format_string(name);

        while(temp != NULL)
        {
            while (temp != NULL)
            {
                if ((strncmp(temp->name, macroName, strlen(temp->name)) == 0) && (strlen(temp->name) == strlen(macroName)))
                {
                    free(macroName);
                    return temp;
                }
                temp = temp->next;
            }
        }
    }