#include "SecondScan.h"
#include "Constants.h"
#include "Utils.h"
#include "FirstScan.h"

/*
    This file contains the second pass and writes everything from the temporary file to the ob file and replaces all the label names with their addresses.
    we also create the .ext file and .ent file if needed and check for errors.
*/

int second_scan(FILE *outputFile, char *fileName,labelsList *head,int instructionLength,int dataLength)
{
      FILE *inputFile=fopen(FIRSTSCAN_TEMP_FILE_NAME,"r");
      char *fileExtName = add_ext_to_file(fileName,EXTERNAL_EXT);
      char *fileEntName = add_ext_to_file(fileName,ENTRY_EXT);
      FILE *entFile =fopen(fileEntName,"w+");
      FILE *extFile =fopen(fileExtName,"w+");

      int IC =MEMORY_START;
      int error =0,isExt=0,isEnt=0;
      char *originLine =(char*)malloc(MAX_LINE_LENGTH*sizeof(char));
      char *workingLine = originLine;
      fprintf(outputFile,"\t\t%d %d\n",instructionLength,dataLength);
      while(fgets(workingLine,MAX_LINE_LENGTH,inputFile)!=NULL)
      {
          workingLine =pointer_to_first_char(workingLine);
            /*If the line aint label put it in the ob*/
          if (strncmp(workingLine,LABEL_SIGN,strlen(LABEL_SIGN))!=0)
          {
              fprintf(outputFile,"%04d\t  %s",IC,workingLine);

          }
          else/*label! add tje label adress to the ob file*/
          {
                char *labelAddress =NULL,*labelToSearch =workingLine+strlen(LABEL_SIGN);
              labelsList *label;

              if (strlen(workingLine)>strlen(LABEL_SIGN)) {
                  labelToSearch[strcspn(labelToSearch,"\n")]='\0';
                  label =check_if_label_exist(head,labelToSearch);
              }

              if (label == NULL) {
                  printf("ERROR | there isnt label def for  %s\n | LABEL:",labelToSearch);
                  error =1;
                  break;
              }

              if (label -> type !=external)
              {
                    char *temp =parse_to_binary(2,2);
                  labelAddress= parse_to_binary(label->location,ASSEMBELED_LINE_LENGTH-2);
                  fprintf(outputFile, "%04d\t  %s%s\n", IC, labelAddress, temp);
                  free(temp);

                  /*if label is .entry add it to .ent file*/
                  if (label->type == entry || label->type == entry_data)
                  {
                      isEnt = 1;
                  }


              }
              else if (label->type ==external)
              {
                  char *temp =parse_to_binary(1,2);
                  labelAddress= parse_to_binary(0,ASSEMBELED_LINE_LENGTH-1);
                  fprintf(outputFile, "%04d\t  %s%s\n", IC, labelAddress, temp);
                  free(temp);

                  fprintf(extFile, "%s\t  %d\n", labelToSearch, IC);
                  isExt = 1;
              }
              if (labelToSearch !=NULL)
                  free(labelAddress);
          }
          IC++;
      }
     isEnt = print_entry_exist(head,entFile);
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

int print_entry_exist(labelsList *head,FILE *entryFile)
{/*this function print all entry to file*/
    int isExists=0;/*intial to false*/
    while (head != NULL) {
        if (head->type == entry || head->type == entry_data) {
            isExists = 1;/*true*/
            fprintf(entryFile, "%s\t  %d\n", head->name, head->location);
        }
        head = head->next;
    }
    return isExists;
}