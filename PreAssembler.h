#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum macro_status{copy,save,count};

int parse_macro(FILE *inputAsFile,FILE *outputAmFile);

typedef struct macrosLinkedList
{
    struct macrosLinkedList *next;
    char *name;
    char *data;
}macroList;

void print_macro_list(macroList *macros);

void free_macro_list(macroList *macros);

macroList *check_if_macro_exist(macroList *head,char *name);

#endif //PREASSEMBLER_H
