#ifndef FIRSTSCAN_H
#define FIRSTSCAN_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Utils.h"

enum firstScanState
{
    defining,
    none
};

enum labelType
{
    code,
    data,
    string,
    external,
    entry,
    entry_data
};


typedef struct labelsLinkedList
{
    struct labelsLinkedList *next;
    char *name;
    int location;
    int type;
} labelsList;

labelsList *first_scan(FILE *inputFile,int* lenInstruction,int* lenData, int *error);

void is_valid_name(char *name,int *errorAmount);

enum labelType what_label_type(char *str, int labelLength,enum firstScanState currentState);

labelsList *check_if_label_exist(labelsList *head, char *name);

int data_label_size(char *str);

char *data_to_binary(char *str,int numOfData, int labelLength);

int check_valid_dot(char  *str, int labelLength);

int size_of_data_Label(char *str);

char *string_to_binary(char *str,int strLength ,int labelLength);

void set_DC(labelsList *head,int IC);

void freeLabelsList(labelsList *head);
#endif /*FIRSTSCAN_H*/
