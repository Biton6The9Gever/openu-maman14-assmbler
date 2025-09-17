#ifndef SECONDSCAN_H
#define SECONDSCAN_H
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "FirstScan.h"
int second_scan(FILE *outputFile, char *fileName,labelsList *head,int instructionLength,int dataLength);

int print_entry_exist(labelsList *head,FILE *entryFile);

char *parse_to_hex(int num, int size);
#endif /*SECONDSCAN_H*/
