#include "FirstScan.h"
#include "Constants.h"
#include "Utils.h"

labelsList *first_scan(FILE *inputFile,int* lengthInstruction,int* lengthData, int *error)
{
      /*instruction counter and data counter*/
      int IC=100,DC =0;

      /*line number and error amount in this file*/
      int lineNumber=0,errorAmount=0;

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

        while (fgets(workingLine, MAX_LINE_LENGTH, inputFile) != NULL)
        {
            char *parsedLine =NULL;
            int labelNameLength=0;
            char *checkLabel;
            lineNumber++;
            labelNameLength = is_label_line(workingLine);/*contain label Length if not its 0*/
            if (labelNameLength != 0) {
                currentState = defining;
                checkLabel =(char *)calloc(labelNameLength+1,sizeof(char));
                strncpy(checkLabel,workingLine,labelNameLength);
                if(return_opertaion(checkLabel).operationName != none_oper)
                {
                    printf("ERROR |  cannot set label name same as a command  | Label: %s\n",checkLabel);
                    errorAmount++;
                }
                else if(return_register_name(checkLabel) != -1) /*label cant have operation name and register name*/
                {
                    printf("ERROR |  cannot set label name same as a register  | Label: %s\n",checkLabel);
                    errorAmount++;
                }
                else if(return_direct_value(checkLabel) != -1)
                {
                    printf("ERROR |  cannot set label name same as a direct number  | Label: %s\n",checkLabel);
                    errorAmount++;
                }
                free(checkLabel);
            }
            /*if there is a label .data or .string*/
            if ((dataType = what_label_type(workingLine ,errorAmount,currentState))) {
                if (currentState == defining) {
                    char *labelName;
                    labelsList *labelFound =NULL;
                    labelName = (char *)malloc((labelNameLength+1)*sizeof(char));
                    strncpy(labelName,workingLine,labelNameLength);
                    labelName[labelNameLength]='\0';
                    labelName[strcspn(labelName,":")]='\0';
                    if ((labelFound=check_if_label_exist(head,labelName))!=NULL){
                        /*add label as entry*/
                        if (labelFound->type ==entry && labelFound->location ==-1) {
                            labelFound ->location =DC +MEMORY_START;
                            labelFound->type =entry_data;

                            if (dataType == data) {
                                int amountOfData =size_of_data_Label(workingLine);
                                char *parseData;

                                if (count_char_in_string(workingLine,',')!=amountOfData) {
                                    printf("ERROR |  invalid Data declartion  | Label: %s\n",labelName);
                                    errorAmount++;
                                }
                                parseData  =data_to_binary(workingLine,amountOfData,labelNameLength);
                                fprintf(outputFile,"%s",parseData);
                                free(parseData);
                                DC+=amountOfData;
                            }
                            else if (dataType==string) {
                                if (check_valid_dot(workingLine,labelNameLength)==1) {
                                    int strLength =size_of_string_label(workingLine);
                                    char *parsedData=string_to_binary(workingLine,strLength,labelNameLength);
                                    fprintf(outputFile,"%s\n",parsedData);
                                    free(parsedData);
                                    DC+=strLength;
                                }
                                else if (check_valid_dot(workingLine,labelNameLength)==-1) {
                                    printf("ERROR |  invalid String declartion  | Label: %s\n",labelName);
                                    errorAmount++;
                                }
                                else if(check_valid_dot(workingLine,labelNameLength)==-2) {
                                    printf("ERROR |  string contains invalid letters  | Label: %s\n",labelName);
                                    errorAmount++;
                                }
                            }
                            currentState = none;
                        }
                        else {
                            printf("ERROR | cannot define same label twice | Label: %s\n",labelName);
                            errorAmount++;
                        }
                        if (labelName !=NULL) {
                            free(labelName);
                        }
                        continue;
                    }
                    /*adding label to data base*/
                    if (labels == NULL) {
                        labels =(labelsList *)malloc(sizeof(labelsList));
                        if (head == NULL) {
                            head =labels;
                        }
                    }
                    else {
                        labels->next = (labelsList *)malloc(sizeof(labelsList));
                        labels = labels->next;
                    }
                    labels->name =labelName;

                    labels->location =DC +MEMORY_START;
                    labels->type =data;
                    labels->next = NULL;
                }

                if (dataType == data) {
                    char *parsedData;
                    int amonutOfData=size_of_data_Label(workingLine);
                    if (count_char_in_string(workingLine,',')!=amonutOfData-1) {
                        printf("ERROR | Invalid data label declaration\n");
                        errorAmount++;
                    }
                    parsedData =data_to_binary(workingLine,amonutOfData,labelNameLength);
                    fprintf(outputFile,"%s",parsedData);
                    free(parsedData);
                    DC +=amonutOfData;
                }
                else if (dataType==string) {
                    if (check_valid_dot(workingLine,labelNameLength)==1) {
                        int strLength =size_of_string_label(workingLine);
                        char *parsedData=string_to_binary(workingLine,strLength,labelNameLength);
                        fprintf(outputFile,"%s\n",parsedData);
                        free(parsedData);
                        DC +=strLength;
                    }
                    else if (check_valid_dot(workingLine,labelNameLength)==-1) {
                        printf("ERROR |  invalid String declartion  | Label: %s\n",workingLine);
                        errorAmount++;
                    }
                    else if(check_valid_dot(workingLine,labelNameLength)==-2) {
                        printf("ERROR |  string contains invalid letters  | Label: %s\n",workingLine);
                        errorAmount++;
                    }
                }
                currentState = none;
                continue;
            }
            else if (strncmp(workingLine,ENTRY_MARK,strlen(ENTRY_MARK)) == 0 || strncmp(workingLine,EXTERN_MARK,strlen(EXTERN_MARK)) == 0) {
                char *labelName=NULL;

                labelsList *labelFound=NULL;
                if (strncmp(workingLine,ENTRY_MARK,strlen(ENTRY_MARK)) == 0) {
                   labelName=format_string(workingLine+strlen(ENTRY_MARK));

                    if ((labelFound=check_if_label_exist(head,labelName))!=NULL) {
                        if (labelFound->type ==external) {
                            printf("ERROR | cannot make label extern and entern  | Label: %s\n",labelName);
                            *error =1 ;
                            errorAmount++;
                        }
                        if (labelFound->type ==data) {
                            labelFound->type=entry_data;
                        }
                        else if (labelFound->type ==code) {
                            labelFound->type =entry;
                        }

                        if (labelName !=NULL) {
                            free(labelName);
                        }
                        continue;
                    }
                }
                else if (strncmp(workingLine,EXTERN_MARK,strlen(EXTERN_MARK)) == 0) {
                    labelName=format_string(workingLine+strlen(EXTERN_MARK));
                    if ((labelFound=check_if_label_exist(head,labelName))!=NULL) {
                        printf("ERROR | cannot define label more then once | Label: %s\n",labelName);
                        free(labelName);
                        errorAmount++;
                        continue;
                    }
                }

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
                labels->location = -1; /*-1 means no value*/
                labels->next = NULL;

                if (strncmp(workingLine, ENTRY_MARK, strlen(ENTRY_MARK)) == 0)
                { /*if the new label is .entry*/
                    labels->type = entry;
                }
                else
                {
                    /*if the new label is .extern*/
                    labels->type = external;
                }

                /*check errors in labelName*/
                if (is_empty_string(labelName))
                {
                    printf("\nERROR | cannot define empty label | \n");
                    errorAmount++;
                }
                if (return_opertaion(labels->name).operationNumber != none_oper_op)
                {
                    printf("\nERROR | cannot define label as a command | Label: %s\n", labels->name);
                    errorAmount++;
                }

                if (return_register_name(labels->name) != -1)
                {
                    printf("\nERROR | cannot define label as a register | Label: %s\n", labels->name);
                    errorAmount++;
                }

                continue;
            }

            if (currentState == defining) {
                char *labelName;
                labelsList *labelFound=NULL;

                labelName =(char *)malloc((labelNameLength+2)*sizeof(char));
                strncpy(labelName,workingLine,labelNameLength+1);
                labelName[strcspn(labelName,":")]='\0';

                if ((labelFound =check_if_label_exist(head,labelName)) != NULL) {
                    if (labelFound->type ==entry && labelFound->location == -1) {
                        labelFound ->location=IC+MEMORY_START;
                    }
                    else {
                        printf("\nERROR | cannot define label more then once| Label: %s\n", labelName);
                        errorAmount++;
                    }

                    if (labelName != NULL) {
                        free(labelName);
                    }
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

            parsedLine =parse_line(workingLine,&numberOfOperationsLines,error);

            if (parsedLine != NULL) {
                fprintf(outputFile,"%s\n",parsedLine);
                free(parsedLine);
            }
            IC+= numberOfOperationsLines;
      }
    if (errorAmount>0) {
        remove(FIRSTSCAN_TEMP_FILE_NAME);
        free(workingLine);
        fclose(outputFile);
        free_labels_list(head);
        *error =1;
        return NULL;
    }

    set_DC(head,IC);
    *lengthInstruction=IC;
    *lengthData=DC;
    free(workingLine);
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

    /*free(labelNameCopy);*/
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

int  check_valid_dot(char *str, int labelLength)
{/* Function that receive a string and check if its valid*/
    char *copyStr=(char *)malloc((strlen(str)+1)*sizeof(char));
    char *copyStart =copyStr;
    int  i,flag;
    strcpy(copyStr,str);

    if (pointer_to_first_char(copyStr)[0] == '.') {
        copyStr=pointer_to_first_char(copyStr+strlen(STRING_MARK));
    }

    else
    {
        copyStr=pointer_to_first_char(copyStr+labelLength+1);
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
            break;
        }
        else {
            flag =0;
            break;
        }
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

void printLabelsList(labelsList *head)
{ /*This function prints the labels list*/
    puts("---labels---");
    while (head != NULL)
    {
        printf("name: %s count: %d\n", head->name, head->location);
        head = head->next;
    }
    puts("---labels---");
}

void trim_carriage_return(char *str) {
    int start = 0,i;
    int end = strlen(str) - 1;

    while (str[start] == 13) {
        start++;
    }

    while (str[end] == 13) {
        end--;
    }

    for (i = start; i <= end; i++) {
        str[i - start] = str[i];
    }
    str[end - start + 1] = '\0';
}

