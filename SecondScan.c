#include "SecondScan.h"
#include "Constants.h"
#include "Utils.h"
#include "FirstScan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int second_scan(FILE *outputFile, char *fileName, labelsList *head, int instructionLength, int dataLength)
{
    FILE *inputFile = fopen(FIRSTSCAN_TEMP_FILE_NAME, "r");
    char *fileExtName = add_ext_to_file(fileName, EXTERNAL_EXT);
    char *fileEntName = add_ext_to_file(fileName, ENTRY_EXT);
    FILE *entFile = fopen(fileEntName, "w+");
    FILE *extFile = fopen(fileExtName, "w+");

    int IC = MEMORY_START;
    int error = 0, isExt = 0, isEnt = 0;
    char *originLine = (char *)malloc(MAX_LINE_LENGTH * sizeof(char));
    char *workingLine = originLine;

    printLabelsList(head);
    printLabelsList(head);
    /* print instruction and data length in hex */
    fprintf(outputFile, "\t\t%d %d\n", instructionLength, dataLength);

    while (fgets(workingLine, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        workingLine = pointer_to_first_char(workingLine);
        /*not label add address of the current line to .ob*/
        if (strncmp(workingLine,LABEL_SIGN,strlen(LABEL_SIGN)) != 0) {
            char *IC_hex =parse_to_hex(IC, 7);
            fprintf(outputFile,"%s\t  %s",IC_hex ,workingLine);
        }
        else {
            char labelAddres[8],*labelNameSearch=workingLine+strlen(LABEL_SIGN);
            char *IC_hex=NULL;
            labelsList *label;
            printf("%s \n",labelNameSearch);
            if (strlen(workingLine)>strlen(LABEL_SIGN)) {
                labelNameSearch[strcspn(labelNameSearch,"\n")]='\0';
                label = check_if_label_exist(head,labelNameSearch);
            }

            if (label == NULL) {
                printf(" ERROR | there isn't dec for label %s \n",labelNameSearch);
                error = 1;
                break;
            }
            if (label->type !=external) {
                IC_hex=parse_to_hex(IC,7);
                sprintf(labelAddres, "%07d", label->location);
                fprintf(outputFile,"%s\t  %p\n",IC_hex ,labelAddres);

                if (label->type == entry_data || label->type==entry)
                    isEnt = 1;
            }
            else if (label->type ==external) {
                IC_hex=parse_to_hex(IC,7);
                sprintf(labelAddres, "%07d", label->location);
                fprintf(outputFile,"%s\t  %p\n",IC_hex ,labelAddres);

                fprintf(extFile,"%s\t  %07d \n",labelNameSearch ,IC);
                isExt = 1;
            }
            if (IC_hex != NULL) {
                free(IC_hex);
            }
        }
        IC ++;
    }

    isEnt = print_entry_exist(head, entFile);
    fclose(entFile);
    fclose(extFile);
    fclose(inputFile);

    if (isExt == 0 || error != 0)/*if there are no .extern labels*/
        remove(fileExtName);
    if (isEnt == 0 || error != 0)/*if there are no .entry labels*/
        remove(fileEntName);
    free(originLine);
    free(fileEntName);
    free(fileExtName);
    remove(FIRSTSCAN_TEMP_FILE_NAME);
    return error;
}

int print_entry_exist(labelsList *head, FILE *entryFile)
{/* this function prints all entries to the file */

    int isExists = 0; /* initialize to false */
    while (head != NULL)
    {
        if (head->type == entry || head->type == entry_data)
        {
            isExists = 1; /* true */
            fprintf(entryFile, "%s\t  %07d\n", head->name, head->location);
        }
        head = head->next;
    }
    return isExists;
}


char *parse_to_hex(int num, int size)
{/* Function that converts a number to a hex */
    char *fullStr = (char *)calloc(size + 1, sizeof(char));
    int i = size - 1;
    for (; i >= 0; i--)
    {
        int digit = num & 0xF; /* gets the last hex digit */
        fullStr[i] = (digit < 10) ? ('0' + digit) : ('A' + (digit - 10));
        num >>= 4; /* right shift by 4 bits */
    }
    return fullStr;
}
