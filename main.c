#include <stdio.h>
#include "PreAssembler.h"
#include "Constants.h"
#include "FirstScan.h"
#include "Utils.h"

int main(int argc, char **argv) {
    int fileNumber=1;

    while(fileNumber <argc)
    {/*the first element of argv is the file name with the program*/
        char *currentFileName =add_ext_to_file(argv[fileNumber],ORIGINAL_EXT);
        FILE *currentFile = fopen(currentFileName,"r");
        char *afterPreAssmblerFileName;
        FILE *afterPreAssmblerFile;
        char *fileFirstScanName;
        FILE *fileFirstScan;
        labelsList *labelTable;
        int instructionLength;
        int dataLength,error =0,errorMacro;
        if(currentFile == NULL)
        {
            printf("\n ERROR | error while opening a file | FILE: %s\n", currentFileName);
            fileNumber++;
            continue; /*file is null print error and proceed to the next file*/
        }
        afterPreAssmblerFileName=add_ext_to_file(argv[fileNumber],AFTER_PREASSEMBLER_EXT);
        afterPreAssmblerFile = fopen(afterPreAssmblerFileName,"w+");
        errorMacro= parse_macro(currentFile,afterPreAssmblerFile);
        fclose(afterPreAssmblerFile);
        afterPreAssmblerFile = fopen(afterPreAssmblerFileName,"r");
        labelTable = first_scan(afterPreAssmblerFile,&instructionLength,&dataLength,&error);
        fclose(afterPreAssmblerFile);
        fclose(currentFile);
        printf("%d \n",errorMacro);

        if (error > 0 || errorMacro < 0)
        {
            printf("\n ERROR | error while opening a file * skipped *| FILE: %s\n", afterPreAssmblerFileName);
            fileNumber++;

            if (labelTable != NULL)
            {
                free_labels_list(labelTable);
                labelTable = NULL;
            }
            fclose(currentFile);
            remove(afterPreAssmblerFileName);
            free(afterPreAssmblerFileName);
            free(currentFileName);

           /* continue;*/
        }
    }
    return 1;
}