#include "FirstScan.h"
#include "Constants.h"
#include "Utils.h"

labelsList *first_scan(FILE *inputFile,int* lengthInstruction,int* lengthData, int *error)
{
      /*instruction counter and data counter*/
      int IC=100,DC =0;

      /*line number and error amount in this file*/
      int lineNumber,errorAmount=0;

      char *orignLine = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
      /*This line will get edited and worked with so we can process it without damaging the Orign line*/
      char *workingLine = orignLine;

      /*contains the number of words for this line. this var will act the role of (L) */
      int numberOfOperationsLines;

      enum labelType dataType;
      enum firstScanState currentState = none;

      labelsList *labels = NULL; /* current label.*/
      labelsList *head = labels; /* head */

      FILE *outputFile;
      outputFile = fopen(FIRSTSCAN_TEMP_FILE_NAME,"w");
      while(fgets(workingLine,MAX_LINE_LENGTH,inputFile) != NULL)
      {
          char *binaryWord =NULL;
          int labelNameLength =0;
          char *checkedLabel;
          lineNumber++;
          labelNameLength = is_label_line(workingLine);

          if(labelNameLength != 0)
          {
              currentState = defining;
              checkedLabel =(char *)calloc(labelNameLength+1,sizeof(char));
              strncpy(checkedLabel,workingLine,labelNameLength);
              is_valid_name(checkedLabel,&errorAmount);
              free(checkedLabel);
          }

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
                      /*add label as a entry*/
                      if (labelFound->type == entry && labelFound->location == -1)
                      {
                          labelFound -> location = DC + MEMORY_START;
                          labelFound -> type = entry_data;
                          if (dataType == data)
                          {
                              int amountOfData= data_label_size(workingLine);
                              char* binaryData;
                              if(count_char_in_string(workingLine,',') != amountOfData-1)
                              {
                                  printf("ERROR |  invalid data label declaration | Label: %s\n",labelName);
                                  errorAmount++;
                              }
                              binaryData = data_to_binary(workingLine,amountOfData,labelNameLength);
                              fprintf(outputFile,"%s",binaryData);

                              free(binaryData);
                              DC += amountOfData;
                          }
                          else if (dataType == string)
                          {
                              if(check_valid_dot(workingLine,labelNameLength)==1)
                              {
                                  int strLength =size_of_string_label(workingLine);
                                  char *binaryStr=string_to_binary(workingLine,strLength,labelNameLength);
                                  fprintf(outputFile,"%s",binaryStr);
                                  DC += strLength;
                              }
                              else if (check_valid_dot(workingLine,labelNameLength) ==-1)
                              {
                                  printf("ERROR | string declared illegally | Label: %s\n",labelName);
                                  errorAmount++;
                              }
                              else if (check_valid_dot(workingLine,labelNameLength) ==-2)
                              {
                                  printf("ERROR | string contained invalid characters | Label: %s\n",labelName);
                                  errorAmount++;
                              }
                          }
                          currentState = none;
                      }
                      else
                      {
                          printf("ERROR | cannot define label twice | Label: %s\n",labelName);
                          errorAmount++;
                      }
                      if (labelName != NULL)
                      {
                          free(labelName);
                      }
                      continue;
                  }
                  if (labels ==NULL)
                  {
                      labels =(labelsList *)malloc(sizeof(labelsList));
                      if (head == NULL)
                      {
                          head = labels;
                      }
                  }
                  else
                  {
                      labels->next=(labelsList *)malloc(sizeof(labelsList));
                      labels->next=labels;
                  }
                  labels->name =labelName;
                  labels->type =data;
                  labels->location =DC + MEMORY_START;
                  labels->next =NULL;
              }
              if (dataType==data)
              {
                  char* binaryData;
                  int amountOfData= data_label_size(workingLine);
                  if (count_char_in_string(workingLine,',') != amountOfData-1)
                  {
                      printf("ERROR |  invalid data label declaration \n");
                      errorAmount++;
                  }
                  binaryData = data_to_binary(workingLine,amountOfData,labelNameLength);
                  fprintf(outputFile,"%s",binaryData);

                  DC += amountOfData;
                  free(binaryData);
              }
              else if (dataType == string)
              {
                  if (check_valid_dot(workingLine,labelNameLength)== 1)
                  {
                      int strLength =size_of_string_label(workingLine);
                      char *binaryString=string_to_binary(workingLine,strLength,labelNameLength);
                      fprintf(outputFile,"%s",binaryString);

                      free(binaryString);
                      DC += strLength;
                  }
                  else if (check_valid_dot(workingLine,labelNameLength) ==-1)
                  {
                      *(workingLine +labelNameLength)=0;
                      printf("ERROR | string declared illegally | Label: %s\n",workingLine);
                      errorAmount++;
                  }
                  else if (check_valid_dot(workingLine,labelNameLength) ==-2)
                  {
                      *(workingLine +labelNameLength)=0;
                      printf("ERROR | string contained invalid characters | Label: %s\n",workingLine);
                      errorAmount++;
                  }
              }
              currentState = none;
              continue;
          }
          /*Check if caught entry or extern declaration */
          if (strncmp(workingLine,ENTRY_MARK,strlen(ENTRY_MARK)) == 0 || strncmp(workingLine,EXTERN_MARK,strlen(EXTERN_MARK)) == 0)
          {
              char *labelName =NULL;
              labelsList *labelFound=NULL;
              if (strncmp(workingLine,ENTRY_MARK,strlen(ENTRY_MARK)) == 0)
              {
                  labelName =format_string(workingLine+strlen(ENTRY_MARK));
                  labelFound =check_if_label_exist(head,labelName);
                  if (labelFound != NULL)
                  {
                      if (labelFound->type ==external)
                      {
                          printf("ERROR | label cannot be extern and entry at the same time | Label: %s\n",labelName);
                          *error = 1;
                          errorAmount++;
                      }
                      if (labelFound->type ==data)
                      {
                          labelFound->type =entry_data;
                      }
                      else if (labelFound->type ==code)
                      {
                          labelFound->type =entry;
                      }

                      if (labelName != NULL)
                      {
                          free(labelName);
                      }
                      continue;
                  }

              }
              else if ( strncmp(workingLine,EXTERN_MARK,strlen(EXTERN_MARK)) == 0)
              {
                  labelName =format_string(workingLine+strlen(EXTERN_MARK));
                  labelFound =check_if_label_exist(head,labelName);
                  if (labelFound != NULL)
                  {
                      printf("ERROR | cannot define label more the one time | Label: %s\n",labelName);
                      free(labelName);
                      errorAmount++;
                      continue;
                  }
              }
              /*we went threw the cases as when the label DO exist
               * else we have to create it */
              if (labels ==NULL)
              {
                  labels =(labelsList *)malloc(sizeof(labelsList));
                  if (head == NULL)
                  {
                      head = labels;
                  }
              }
              else
              {
                  labels->next=(labelsList *)malloc(sizeof(labelsList));
                  labels->next=labels;
              }
              labels->name = labelName;
              labels->location = -1; /* -1 stands for | no value |*/
              labels->next = NULL;

              if (strncmp(workingLine,ENTRY_MARK,strlen(ENTRY_MARK)) == 0)
              {
                  labels->type=entry;
              }
              else
              {
                  labels->type=external;
              }
              is_valid_name(labelName,&errorAmount);
              continue;
          }
          /*Normal label without and .mark and append it into the table*/
          if (currentState == defining)
          {
              char *labelName =NULL;
              labelsList *labelFound=NULL;
              labelName= (char *)malloc((labelNameLength+2)*sizeof(char));
              strncpy(labelName,workingLine,labelNameLength+1);
              labelName[strspn(labelName,":")]='\0';
              labelFound =check_if_label_exist(head,labelName);
              if (labelFound != NULL)
              {
                  /*if its entry label we set IC */
                  if (labelFound->type ==entry && labelFound->location == -1)
                  {
                      labelFound->location = IC+MEMORY_START;
                  }
                  else
                  {
                      printf("ERROR | cannot define label more the one time | Label: %s\n",labelName);
                      errorAmount++;
                  }
                  if (labelName != NULL)
                      free(labelName);
                  continue;
              }
              /*adding the current label to the linked list*/
              if (labels == NULL)
              {
                  labels = (labelsList *)malloc(sizeof(labelsList));
                  if (head == NULL)
                  {
                      head = labels;
                  }
              }
              else
              {
                  labels->next = (labelsList *)malloc(sizeof(labelsList));
                  labels = labels->next;
              }
              labels->name = labelName;
              labels->location = IC + MEMORY_START;
              labels->type = code;
              labels->next = NULL;
              currentState = none;
          }
          binaryWord =parse_line(workingLine,&numberOfOperationsLines,error);
          if (binaryWord != NULL)
          {
              fprintf(outputFile,"%s\n",binaryWord);
              free(binaryWord);
          }
          /* IC + L*/
          IC += numberOfOperationsLines;

      }
      if (errorAmount>0)
      {
          remove(FIRSTSCAN_TEMP_FILE_NAME);
          free(orignLine);
          fclose(outputFile);
          free_labels_list(head);
          *error=1;
          return NULL;
      }
      set_DC(head,IC);
      *lengthData=DC;
      *lengthInstruction=IC;
      free(orignLine);
      fclose(outputFile);
      return head;
}

void is_valid_name(char *name,int *errorAmount)
{/*Function that check if the label name is valid*/

        if(return_opertaion(name).operationName != none_oper)
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
{/*Function that check what is the label type*/
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
{/*Function that check if label already exist by name searching*/
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

int data_label_size(char *str)
{/*Function that count the amount of sub-strings bordered with commas*/
    int counter =0;

    /*strtok is changing the string so we are going to work with a copy*/
    char *copy =malloc((strlen(str)+1)*sizeof(char));
    /*pointer to copy so we could free its memory space*/
    char *copyStart =copy;
    char *token;
    strcpy(copyStart,str);
    token = strtok(copyStart,",");
    while (token != NULL)
    {
        counter++;
        token = strtok(NULL,",");
    }
    free(copyStart);
    return counter;
}

char *data_to_binary(char *str,int numOfData, int labelLength)
{/*Function that get data and transfer it into binary*/
    char *copy=(char *)malloc((strlen(str)+1)*sizeof(char));
    char *copyStart =copy;
    char *placeHolder;
    /*this will contain the string as binary which we will return*/
    char *_return=(char *)calloc((numOfData * (ASSEMBELED_LINE_LENGTH+1))+1,sizeof(char));
    char *token, *reFormatedToken;
    strcpy(copy,str);
    token = strtok(copy,",");

    if (pointer_to_first_char(token)[0] != '.')
    {
        token = pointer_to_first_char(str);
        token += strlen(DATA_MARK);
    }
    else
    {
        token = pointer_to_first_char(str+labelLength+1);
        token += strlen(DATA_MARK);
    }
    reFormatedToken = strtok(token,",");
    while (reFormatedToken!=NULL)
    {
        placeHolder = parse_to_binary(atoi(reFormatedToken),ASSEMBELED_LINE_LENGTH);
        _return = strncat(_return,placeHolder,ASSEMBELED_LINE_LENGTH);
        _return =strcat(_return,"\n");
        reFormatedToken = strtok(NULL,",");
        free(placeHolder);
    }
    free(copyStart);
    return _return;
}

int check_valid_dot(char *str, int labelLength)
{/* Function that receive a string and check if its valid*/
    char *copyStr=(char *)malloc((strlen(str)+1)*sizeof(char));
    char *copyStart =copyStr;
    int  i,flag;
    strcpy(copyStr,str);

    if (pointer_to_first_char(copyStr)[0] != '.')
        copyStr=pointer_to_first_char(copyStr+strlen(STRING_MARK));
    else
    {
        copyStr=pointer_to_first_char(str+labelLength+1);
        copyStr = pointer_to_first_char(copyStr+strlen(STRING_MARK));
    }

    if (copyStr == NULL)
        return -1;

    if (copyStr[0] != '"')
    {
        free(copyStart);
        return -1;
    }

    flag =1;
    for (i=1;copyStr[i];i++) {
        if (copyStr[i]!='\n'&& copyStr[i] != '"')
            continue;

        if (copyStr[i] == '"' && (copyStr[i+1] == '\0' || is_empty_string(copyStr +i +1))) {
            if (i<=1) {
                free(copyStart);
                return -1;
            }
        }

        flag =0;
        break;
    }

    if (flag ==0) {
        free(copyStart);
        return -2;
    }
    free(copyStart);
    return 1;
}

int size_of_data_Label(char *str)
{/*Function that check the size of data label and returns it*/
    int counter =0;
    char *strCopy=(char *)malloc((strlen(str)+1)*sizeof(char));
    char *strCopyStart =strCopy;
    char *token;
    strcpy(strCopy,str);
    token = strtok(strCopy,",");

    while (token != NULL)
    {
        counter++;
        token = strtok(NULL,",");
    }
    free(strCopyStart);
    return counter;
}
int size_of_string_label(char *str) {
    char *start =strchr(str,'"')+1;
    char *end =strchr(start,'"')-1;
    return end-start+2;
}
char *string_to_binary(char *str,int strLength ,int labelLength)
{/*Function that parse string to a binary one*/
    char *_return =(char *)calloc(strLength*(ASSEMBELED_LINE_LENGTH+1)+1,sizeof(char));
    char *nullLine =parse_to_binary(0,ASSEMBELED_LINE_LENGTH);
    int i=0;
    char *start=strchr(str,'"')+1;

    for (;start[i]!='"';i++)
    {
        char *binaryData =parse_to_binary(start[i],ASSEMBELED_LINE_LENGTH);
        _return=strncat(_return,binaryData,strlen(binaryData));
        _return =strcat(_return,"\n");
        free(binaryData);
    }
    _return =strncat(_return,nullLine,strlen(nullLine));/*for \0 */
    free(nullLine);
    return _return;

}

void set_DC(labelsList *head,int const IC)
{/*Function that update DC */
    while(head!=NULL)
    {
        if (head->type ==data || head->type ==entry_data)
            head->location +=IC;
        head = head->next;
    }
}

void free_labels_list(labelsList *head)
{ /*Function that free the label list*/
    labelsList *tmp;
    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        if (tmp->name != NULL)
        {
            free(tmp->name);
        }
        free(tmp);
    }
}
