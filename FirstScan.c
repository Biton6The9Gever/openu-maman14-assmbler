#include "FirstScan.h"
#include "Constants.h"
#include "Utils.h"

labelsList *first_scan(FILE *inputFile,int* lenInstruction,int* lenData, int *error)
{
      int IC=100,DC =0,lineNumber,errorAmount=0;

      char *orignLine = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
      char *workingLine = orignLine; /*This line will get edited and worked with so we can process it without damaging the Orign line*/

      enum labelType dataType;
      enum firstScanState currentState = none;

      labelsList *labels = NULL; /* current label.*/
      labelsList *head = labels; /* head */

      FILE *outputFile;
      outputFile = fopen(FIRSTSCAN_TEMP_FILE_NAME,"w");
      while(fgets(workingLine,MAX_LINE_LENGTH,inputFile) != NULL)
      {
          char *encodedWord =NULL;
          int labelNameLength =0;
          char *checkedLabel;
          lineNumber++;
          labelNameLength = is_label_line(workingLine);
          currentState = defining;
          checkedLabel =(char *)calloc(labelNameLength+1,sizeof(char));
          strncpy(checkedLabel,workingLine,labelNameLength);
          if(labelNameLength != 0)
          {
              is_valid_name(checkedLabel,&errorAmount);
          }
          free(checkedLabel);
          dataType=what_label_type(workingLine,labelNameLength,currentState);
          if(dataType)
          {
              if(currentState == defining)
              {
                 char *labelName =(char *)malloc((labelNameLength+1)*sizeof(char));
                 labelsList *labelFound =NULL;
                 strncpy(labelName,workingLine,labelNameLength);
                 labelName[labelNameLength] = '\0';
                 labelName[strcspn(labelName,":")] = '\0';
                 labelFound= check_if_label_exist(head,labelName);
                 if (labelFound != NULL)
                 {
                     if (labelFound->type == entry && labelFound->location == -1)
                     {
                         labelFound -> location = DC + MEMORY_START;
                         labelFound -> type = entry_data;
                         int amountOfData;/*= todo*/
                         char encodedData;
                         if(count_char_in_string(workingLine,",") != amountOfData)
                         {
                             printf("ERROR |  invalid data label declaration | Label: %s\n",labelName);
                             errorAmount++;
                         }

                     }
                 }
              }
          }
      }
      return NULL;
}

void is_valid_name(char *name,int *errorAmount)
{

        if(return_opertaion(name).operationNum != none_oper)
        {
            printf("ERROR |  cannot set label name same as a command  | Label: %s\n",name);
            (*errorAmount)++;
        }
        else if(return_register_name(name) != -1) /*label cant have operation name and register name*/
        {
            printf("ERROR |  cannot set label name same as a register  | Label: %s\n",name);
            (*errorAmount)++;
        }
        else if(return_direct_value(name) != -1)
        {
            printf("ERROR |  cannot set label name same as a direct number  | Label: %s\n",name);
            (*errorAmount)++;
        }
}

enum labelType what_label_type(char *str, int labelLength,enum firstScanState currentState)
{
     char *currentLine,*instructLine;
     if(currentState == defining)
     {
          currentLine = pointer_to_first_char(str + labelLength + 1);
     }
     else
     {
          currentLine = pointer_to_first_char(str);
     }
    /*point to the first word after the label to check for the type*/
     instructLine = format_string(currentLine);
     /*caught in .string mark */
     if(strcmp(instructLine,STRING_MARK) == 0)
     {
         free(instructLine);
         return string;
     }
    /*caught in .data mark */
     if(strcmp(instructLine,DATA_MARK) == 0)
     {
       free(instructLine);
       return data;
     }
     /*else its just code*/
     free(instructLine);
     return code;
}

labelsList *check_if_label_exist(labelsList *head, char *name)
{
    labelsList *tmp = head;

    char *labelNameCopy = format_string(name);

    while (tmp != NULL && tmp->name != NULL)
    {
        if (strcmp(tmp->name, labelNameCopy) == 0)
        {
            free(labelNameCopy);
            return tmp;
        }
        tmp = tmp->next;
    }

    free(labelNameCopy);
    return NULL;
}