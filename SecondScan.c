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

    /* print instruction and data length in hex */
    fprintf(outputFile, "\t\t%s %s\n", parse_to_hex(instructionLength, 7), parse_to_hex(dataLength, 7));

    while (fgets(workingLine, MAX_LINE_LENGTH, inputFile) != NULL)
    {
        char *IC_hex = parse_to_hex(IC, 7);
        workingLine = pointer_to_first_char(workingLine);

        /* converts IC to hex */


        /* if the line isn't a label, print it directly */
        if (strncmp(workingLine, LABEL_SIGN, strlen(LABEL_SIGN)) != 0)
        {
            fprintf(outputFile, "%s\t  %s", IC_hex, workingLine);
        }
        else /*  label found add the label address to the ob file */
        {
            char *labelAddress = NULL, *labelToSearch = workingLine + strlen(LABEL_SIGN);
            labelsList *label;

            if (strlen(workingLine) > strlen(LABEL_SIGN))
            {
                labelToSearch[strcspn(labelToSearch, "\n")] = '\0';
                label = check_if_label_exist(head, labelToSearch);
            }

            if (label == NULL)
            {
                printf("ERROR | Undefined label: %s\n", labelToSearch);
                error = 1;
                free(IC_hex);
                break;
            }

            if (label->type != external)
            {
                char *temp = parse_to_hex(2, 6);
                labelAddress = parse_to_hex(label->location, 6);
                fprintf(outputFile, "%s\t  %s%s\n", IC_hex, labelAddress, temp);
                free(temp);

                /* if label is .entry, add it to .ent file */
                if (label->type == entry || label->type == entry_data)
                {
                    isEnt = 1;
                    fprintf(entFile, "%s\t  %s\n", labelToSearch, labelAddress);
                }
            }
            else if (label->type == external)
            {
                char *temp = parse_to_hex(1, 6);
                labelAddress = parse_to_hex(0, 6);
                fprintf(outputFile, "%s\t  %s%s\n", IC_hex, labelAddress, temp);
                free(temp);

                fprintf(extFile, "%s\t  %s\n", labelToSearch, IC_hex);
                isExt = 1;
            }
            if (labelAddress != NULL)
                free(labelAddress);
        }
        free(IC_hex);
        IC++;
    }

    isEnt = print_entry_exist(head, entFile);
    fclose(entFile);
    fclose(extFile);
    fclose(inputFile);

    if (isExt == 0 || error != 0)
        remove(fileExtName);
    if (isEnt == 0 || error != 0)
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
            fprintf(entryFile, "%s\t  %s\n", head->name, parse_to_hex(head->location, 6));
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
