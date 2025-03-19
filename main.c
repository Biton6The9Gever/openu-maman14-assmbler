#include <stdio.h>
#include "PreAssembler.h"
#include "Constants.h"
#include "Utils.h"

int main(int argc, char **argv) {
    int fileNumber=1;

    while(fileNumber <argc)
    {/*the first element of argv is the file name with the program*/
        char *currentFileName =add_ext_to_file(argv[fileNumber],ORIGINAL_EXT);
        FILE *currentFile = fopen(currentFileName,"r");
        char *afterPreAssmblerFileName;
        FILE *afterPreAssmblerFile;
        int errorMacro;
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
        fclose(currentFile);
        printf("%d \n",errorMacro);

    }
    return 1;
}