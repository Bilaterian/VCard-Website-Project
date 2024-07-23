/*
*Name: Lance Paje
*Student #: 0956905
*/

#include "VCParser.h"
static const char *PROPERTY_NAMES[] = {"BEGIN", "END", "SOURCE", "KIND", "XML", "FN", "N", "NICKNAME", 
"PHOTO", "BDAY", "ANNIVERSARY", "GENDER", "ADR", "TEL", "EMAIL", "IMPP", "LANG", "TZ", "GEO", "TITLE", 
"ROLE", "LOGO", "ORG", "MEMBER", "RELATED", "CATEGORIES", "NOTE", "PRODID", "REV", "SOUND", "UID", 
"CLIENTPIDMAP", "URL", "VERSION", "KEY", "FBURL", "CALADRURI", "CALURI"};

/****************helper functions*******************/
bool checkNullPointer(void *ptr){
	if(!ptr){
		return true;
	}
	return false;
}

/*for Date Time*/
bool hasLetters(char *string){ //fully test this please
	bool letter = false;
	
	int i = 0;
	
	while(string[i] != '\0'){
		if((string[i] >64 && string[i] < 91) || (string[i] >96 && string[i] < 123)){
			if((string[i + 1] != '\0') && ((string[i + 1] >64 && string[i + 1] < 91) || (string[i + 1] >96 && string[i + 1] < 123))){
				letter = true;
				break;
			}
		}
		i++;
	}
	
	return letter;
}

bool checkUTC(char *string){
	int i = 0;
	
	while(string[i] != '\0'){
		i++;
	}
	if(string[i - 1] == 'Z'){
		return true;
	}
	
	return false;
}

int checkForChar(char *string, char T){
	int i = 0;
	
	while(string[i] != '\0'){
		if(string[i] == T){
			return i;
		}
		i++;
	}
	
	return -1;
}

DateTime *createDt(char *fromFile){ //make a string checker for valid input before making a new Dt
	DateTime *newDt = (DateTime *)malloc(sizeof(DateTime));
	int whereTIs = 0;
	int i = 0;

	newDt->UTC = false;
	newDt->isText = false;
	
	newDt->text = (char *)malloc(sizeof(char) * 1000);
	newDt->date = (char *)malloc(sizeof(char) * 1000);
	newDt->time = (char *)malloc(sizeof(char) * 1000);
	strcpy(newDt->text, "");
	strcpy(newDt->date, "");
	strcpy(newDt->time, "");
	
	if(checkForChar(fromFile, 'Z') != -1){
		newDt->UTC = true;
	}
	else{
		newDt->UTC = false;
	}
	
	newDt->isText = hasLetters(fromFile);
	if(newDt->isText){
		strcpy(newDt->text, fromFile);
	}
	else{
		whereTIs = checkForChar(fromFile, 'T');
		if(whereTIs != -1){
			if(fromFile[whereTIs + 1] != '\0'){
				if(whereTIs != 0){
					newDt->date = strtok(fromFile, "T");
					newDt->time = strtok(NULL, "");
				}
				else{
					while(fromFile[i + 1] != 0){
						newDt->time[i] = fromFile[i + 1];
						i++;
					}
					newDt->time[i] = '\0';
				}
			}
		}
		else{
			strcpy(newDt->date, fromFile);
		}
	}	
	return newDt;
}

/*for Parameter*/
Parameter *createParam(char *input){
	Parameter *newParam = (Parameter *)malloc(sizeof(Parameter));
	newParam->name = (char *)malloc(sizeof(char *));
	newParam->value = (char *)malloc(sizeof(char *));
	
	newParam->name = strtok(input, "=");
	newParam->value = strtok(NULL, "");
	return newParam;
}


/*for Property*/
Property *createProperty(char **fromFile1, int length1, char **fromFile2, int length2){
	int i = 1;
	int j = 0;
	int k = 0;
	int iter = 0;
	bool periodFound = false;
	
	Property *newProperty = (Property *)malloc(sizeof(Property));
	newProperty->name = (char *)malloc(sizeof(char *));
	newProperty->group = (char *)malloc(sizeof(char *));

	while(fromFile1[0][iter] != '\0'){
		if(fromFile1[0][iter] == '.'){
			newProperty->group = strtok(fromFile1[0], ".");
			for(k = 0; k < strlen(newProperty->group); k++){
				if(newProperty->group[k] < 32 || newProperty->group[k] > 126){
					newProperty->group[k] = '\0';
					break;
				}
			}
			newProperty->name = strtok(NULL, "");
			for(k = 0; k < strlen(newProperty->name); k++){
				if(newProperty->name[k] < 32 || newProperty->name[k] > 126){
					newProperty->name[k] = '\0';
					break;
				}
			}
			periodFound = true;
			break;
		}
		iter++;
	}

	if(!periodFound){
		strcpy(newProperty->name, fromFile1[0]);
		for(k = 0; k < strlen(newProperty->name); k++){
				if(newProperty->name[k] < 32 || newProperty->name[k] > 126){
					newProperty->name[k] = '\0';
					break;
				}
			}
		strcpy(newProperty->group, "");
	}

	newProperty->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
	newProperty->values = initializeList(&valueToString, &deleteValue, &compareValues);

	if(length1 > 1){
		for(i = 1; i < length1; i++){
			for(k = 0; k < strlen(fromFile1[i]); k++){
				if(fromFile1[i][k] < 32 || fromFile1[i][k] > 126){
					fromFile1[i][k] = '\0';
					break;
				}
			}
			insertBack(newProperty->parameters, createParam(fromFile1[i]));
		}
	}
		
	if(fromFile2 && fromFile2[0]){
		for(j = 0; j < length2; j++){
			for(k = 0; k < strlen(fromFile2[j]); k++){
				if(fromFile2[j][k] < 32 || fromFile2[j][k] > 126){
					fromFile2[j][k] = '\0';
					break;
				}
			}
			insertBack(newProperty->values, fromFile2[j]);
		}
	}
	
	return newProperty;
}

void addToProperty(Property **storageAddress, char **toAdd, int length){
	Property *storage = (*storageAddress);
	int i = 0;
	for(i = 0; i < length; i++){
		insertBack(storage->values, toAdd[i]);
	}
	
}

VCardErrorCode checkIfParametersAreLegal(char **propertyAndParameters, int length){
	int i = 1; //set to one since the first element is the property
	int j = 0;
	bool foundEquals = false;
	
	if(!propertyAndParameters){
		return INV_FILE;
	}
	
	for(i = 1; i < length; i++){
		j = 0;
		while(propertyAndParameters[i][j] != '\0'){
			if(propertyAndParameters[i][j] == '='){
				foundEquals = true;
				break;
			}
			j++;
		}
		if(!foundEquals){
			return INV_PROP;
		}
		else if(propertyAndParameters[i][j + 1] == '\0'){
			return INV_PROP;			
		}
		else if(propertyAndParameters[i][0] == '='){ 
			return INV_PROP;
		}
		foundEquals = false;
	}
	return OK;
}

void printPropertyParameter(char **toPrint, int length){
	int i = 0;
	
	for(i = 0; i < length; i++){
		printf("\"%s\"", toPrint[i]);
	}
}

int semicolonCounter(char *input){
	int i = 0;
	int semicolonCount = 0;
	
	if(!input){
		return 0;
	}
	
	while(input[i]){
		if(input[i] == ';'){
			
			semicolonCount = semicolonCount + 1;
		}
		i++;
	}
	return semicolonCount;
}

char **substringParser(char *input){
	char **output;
	char *inputCopy; //since string tokenization edits the source string this copy instead keeps the source pure
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int semicolonCount = semicolonCounter(input);
	
	if(!input){
		output = (char **)malloc(sizeof(char *));
		output[0] = (char *)malloc(1000);
		strcpy(output[0], "");
		return NULL;
	}
	
	inputCopy = (char *)malloc(1000);
	strcpy(inputCopy, input);
	output = (char **)malloc(sizeof(char *));

	
	for(i = 0; i < semicolonCount + 2; i++){
		output = (char **)realloc(output, sizeof(char *) * (i + 1));
		output[i] = (char *)malloc(1000);
		while(inputCopy[j] != ';' && inputCopy[j] != '\0'){
			j++;
		}
		l = 0;
		while(k < j){
			output[i][l] = inputCopy[k];
			l++;
			k++;
		}
		k++; //removes these from touching the semicolon
		j++;
	}
	return output;
}

char *toUpper(char *input){
	/*int i = 0;
	
	for(i = 0; i < strlen(input); i++){
		if((input[i] > 96) && (input[i] < 123)){
			input[i] = input[i] - 32;
		}
	}*/
	return input;
}

void printCharArray(char **toPrint, int length){
	int i = 0;
	
	for(i = 0; i < length; i++){
		printf("%s\n", toPrint[i]);
	}
}

/**************************************************/
VCardErrorCode errorChecker(char **stringArray, int length){
	int i = 0;
	bool versionFound = false;
	char *line = (char *)malloc(sizeof(char) * 1000);
	char *hold1 = (char *)malloc(sizeof(char) * 1000);
	char *hold2 = (char *)malloc(sizeof(char) * 1000);
	char *property = (char *)malloc(sizeof(char) * 1000);
	char *value = (char *)malloc(sizeof(char) * 1000);
	
	if(strcmp(stringArray[0], "INV_CARD") == 0){
		return INV_CARD;
	}
	else if(strcmp(stringArray[0], "INV_FILE") == 0){
		return INV_FILE;
	}
	
	for(i = 0; i < length; i++){
		strcpy(line, stringArray[i]);
		hold1 = strtok(line, ":");
		hold2 = strtok(NULL, "");

		property = strtok(hold1, ";");
		value = strtok(hold2, ";");
		if((strcmp(toUpper(property), "BEGIN") != 0) && (i == 0) && (strcmp(toUpper(value), "VCARD") != 0)){
			return INV_CARD;
		}
		if(strcmp(toUpper(property), "VERSION") == 0){
			versionFound = true;
			if(strcmp(value,"4.0") != 0){
				return INV_CARD;
			}
		}
		if((!property)||(!value)){ //checks for null values in the string
			return INV_PROP;
		}
	}
	
	if(strcmp(toUpper(property), "END") != 0 && (strcmp(toUpper(value), "VCARD") != 0)){
		return INV_CARD;
	}
	if(!versionFound){
		return INV_CARD;
	}
	
	return OK;
}

VCardErrorCode createCard(char* fileName, Card** newCardObject){ 
	VCardErrorCode error = OK;
	Card *cardModified;
	FILE *fp;
	char **entireFile;
	char *lineFromFile;
	char *lineToArray;
	
	char *line;
	char *hold;
	char *property;
	char *everythingElse;
	char *value;
	
	char *birthdayString = NULL;
	char *anniversaryString = NULL;
	
	int i = 0;
	int j = 0;
	int numOfLines = 0;
	
	bool bDaySet = false;
	bool annivSet = false;
	
	///****************the file parser******************///
	if(checkNullPointer(fileName)){
		return INV_FILE;
	}

	entireFile = (char **)malloc(sizeof(char *));
	entireFile[0] = (char *)malloc(sizeof(char) * 1000);
	lineFromFile = (char *)malloc(sizeof(char) * 1000);
	lineToArray = (char *)malloc(sizeof(char) * 1000);
	
	fp = fopen(fileName, "r");
	
	if(checkNullPointer(fp)){
		fclose(fp);
		return INV_FILE;
	}
	
	while(fgets(lineFromFile, 1000, fp)){
		if(lineFromFile[strlen(lineFromFile)-1] == '\n' && lineFromFile[strlen(lineFromFile) - 2] == '\r'){
			lineToArray = strtok(lineFromFile, "\r\n");
			strcpy(entireFile[i], lineToArray);
			i++;
			
			entireFile = (char **)realloc(entireFile, sizeof(char *) * (i + 1));
			entireFile[i] = (char *)malloc(sizeof(char)* 1000);
		}
		else{//invalid line ending
			fclose(fp);
			return INV_CARD;
		}
	}
	fclose(fp);
	
	numOfLines = i + 1;
	
	for(i = 0; i < numOfLines; i++){
		if(entireFile[i][0] == ' ' || entireFile[i][0] == '\t'){
			j = 0;
			while(entireFile[i][j + 1] != '\0'){
				entireFile[i][j] = entireFile[i][j + 1];
				entireFile[i][j + 1] = '\0';
				j++;
			}
			j = 0;
			strcat(entireFile[i - 1], entireFile[i]);
			strcpy(entireFile[i], "");
			for(j = i; j < numOfLines - 1; j ++){
				strcpy(entireFile[j], entireFile[j + 1]);
			}
			numOfLines = numOfLines - 1;
		}
	}
	numOfLines = numOfLines - 1;

	error = errorChecker(entireFile, numOfLines);
	if(error != OK){
		return error;
	}
	
	///*************************transfers to card object*****************************///
	
	(*newCardObject) = (Card *)malloc(sizeof(Card));
	cardModified = (*newCardObject);
	cardModified->fn = NULL;
	cardModified->birthday = NULL;
	cardModified->anniversary = NULL;
	
	line = (char *)malloc(sizeof(char) * 1000);
	hold = (char *)malloc(sizeof(char) * 1000);
	property = (char *)malloc(sizeof(char) * 1000);
	everythingElse = (char *)malloc(sizeof(char) * 1000);
	value = (char *)malloc(sizeof(char) * 1000);
	
	for(i = 0; i < numOfLines; i++){
		strcpy(line, entireFile[i]);
		
		everythingElse = strtok(line, ":");
		value = strtok(NULL, "");
		error = checkIfParametersAreLegal(substringParser(everythingElse), semicolonCounter(everythingElse) + 1); //checks if property parameter parser works
		if(error != OK){
			deleteCard(cardModified);
			return error;
		}
		strcpy(hold, everythingElse); 
		property = strtok(hold, ";");
		if((strcmp(toUpper(property), "BEGIN") != 0) && (strcmp(toUpper(property), "VERSION") != 0) && (strcmp(toUpper(property), "END") != 0)){
			if((strcmp(toUpper(property), "BDAY") == 0) && (!bDaySet)){
				birthdayString = (char *)malloc(1000);
				strcpy(birthdayString, value);
				bDaySet = true;
			}
			else if((strcmp(toUpper(property), "ANNIVERSARY") == 0) && (!annivSet)){
				anniversaryString = (char *)malloc(1000);
				strcpy(anniversaryString, value);
				annivSet = true;
			}
			else{
				if(!(cardModified->fn)){
					cardModified->fn = createProperty(substringParser(everythingElse), semicolonCounter(everythingElse) + 1, substringParser(value), semicolonCounter(value) + 1);
					cardModified->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);
				}
				else{
					insertBack(cardModified->optionalProperties, createProperty(substringParser(everythingElse), semicolonCounter(everythingElse) + 1, substringParser(value), semicolonCounter(value) + 1));
				}
			}
		}	
	}
	if(bDaySet){
		cardModified->birthday = createDt(birthdayString);
	}
	if(annivSet){
		cardModified->anniversary = createDt(anniversaryString);
	}
	return error;
}

void deleteCard(Card* obj){
	if(obj){
		if(obj->fn){
			deleteProperty(obj->fn);
			freeList(obj->optionalProperties);
		}
		if(obj->birthday){
			deleteDate(obj->birthday);
		}
		if(obj->anniversary){
			deleteDate(obj->birthday);
		}
		free(obj);
	}
}

char* cardToString(const Card* obj){ 
	char *stringCard = (char *)malloc(1000000);
	strcpy(stringCard, "NULL");
	return stringCard;
	/*if(!obj){
		strcpy(stringCard, "NULL");
		return stringCard;
	}
	else{
		if(obj->fn){
			strcpy(stringCard, propertyToString(obj->fn));
		}
		if(obj->optionalProperties){
			node = obj->optionalProperties->head;
			while(node){
				strcat(stringCard, propertyToString(node->data));
				node = node->next;
			}
		}
		if(obj->birthday){
			strcat(stringCard, "BDAY:");
			strcat(stringCard, dateToString(obj->birthday));
			strcat(stringCard, "\n");
		}
		if(obj->anniversary){
			strcat(stringCard, "ANNIVERSARY:");
			strcat(stringCard, dateToString(obj->anniversary));
			strcat(stringCard, "\n");
		}
	}
	return stringCard;*/
}

char* errorToString(VCardErrorCode err){
	char *errorCode;
	errorCode = (char *)malloc(sizeof(char) * 12);
	if(err == OK){
		strcpy(errorCode, "OK");
	}
	else if(err == INV_FILE){
		strcpy(errorCode, "INV_FILE");
	}
	else if(err == INV_CARD){
		strcpy(errorCode, "INV_CARD");
	}
	else if(err == INV_PROP){
		strcpy(errorCode, "INV_PROP");
	}
	else if(err == INV_DT){
		strcpy(errorCode, "INV_DT");
	}
	else if(err == WRITE_ERROR){
		strcpy(errorCode, "WRITE_ERROR");
	}
	else{
		strcpy(errorCode, "OTHER_ERROR");
	}
	return errorCode;
}

// *************************************************************************

// ************* List helper functions - MUST be implemented *************** 
void deleteProperty(void* toBeDeleted){	
	Property *deleteThis = (Property *)malloc(sizeof(void *));
	deleteThis = (Property *)toBeDeleted;
	free(deleteThis->name);
	free(deleteThis->group);
	freeList(deleteThis->parameters);
	freeList(deleteThis->values);
	free(deleteThis);
}

int compareProperties(const void* first,const void* second){
	Property *firstProperty = (Property *)malloc(sizeof(Property*));
	Property *secondProperty = (Property *)malloc(sizeof(Property*));
	
	firstProperty = (Property *)first;
	secondProperty = (Property *)second;
	if(strcmp(firstProperty->name, secondProperty->name) != 0){
		return(strcmp(firstProperty->name, secondProperty->name));
	}
	else if(firstProperty->parameters->length - secondProperty->parameters->length != 0){
		return(firstProperty->parameters->length - secondProperty->parameters->length);
	}
	else if(firstProperty->values->length - secondProperty->values->length != 0){
		return(firstProperty->values->length - secondProperty->values->length);
	}
	
	return 0;
}

char* propertyToString(void* prop){
	Node *node;
	Property *propPrint = (Property *)malloc(sizeof(Property));
	propPrint = (Property *)prop;
	char *str = (char *)malloc(1000);
	
	if(strcmp(propPrint->group, "") != 0){
		strcpy(str, propPrint->group);
		strcat(str,".");
		strcat(str, propPrint->name);
	}
	else{
		strcpy(str, propPrint->name);
	}
	
	node = propPrint->parameters->head;
	while(node){
		strcat(str, ";");
		strcat(str, parameterToString(node->data));
		node = node->next;
	}
	strcat(str,":");
	
	node = propPrint->values->head;
	while(node){
		strcat(str, valueToString(node->data));
		if(node->next){
			strcat(str, ";");
		}
		node = node->next;
	}
	return str;
}

void deleteParameter(void* toBeDeleted){ //parameter function
	Parameter *deleteThis = (Parameter *)malloc(sizeof(void *));
	deleteThis = (Parameter *)toBeDeleted;
	free(deleteThis->name);
	free(deleteThis->value);
	free(deleteThis);
}

int compareParameters(const void* first,const void* second){ //void pointer to parameters
	Parameter *param1 = (Parameter *)malloc(sizeof(Parameter *));
	Parameter *param2 = (Parameter *)malloc(sizeof(Parameter *));
	param1 = (Parameter *)first;
	param2 = (Parameter *)second;
	
	if(strcmp(param1->name, param2->name) != 0){
		return strcmp(param1->name, param2->name);
	}
	else{
		return strcmp(param1->value, param2->value);
	}
}


char* parameterToString(void* param){ //parameter function
	Parameter *paramString = (Parameter *)malloc(sizeof(Parameter *));
	paramString = (Parameter *)param;
	char *copy = (char *)malloc(1000);
	char *output = (char *)malloc(1000);
	
	strcpy(output, paramString->name);
	strcat(output, "=");
	strcpy(copy, paramString->value);
	strcat(output, copy);
	return output;
}


void deleteValue(void* toBeDeleted){ //parameter function
	char *a = (char *)malloc(sizeof(char *));
	a = (char *)toBeDeleted;
	free(a);
}

int compareValues(const void* first,const void* second){ //parameter function
	char *a = (char *)malloc(sizeof(char *));
	char *b = (char *)malloc(sizeof(char *));
	int i = 0;
	a = (char *)first;
	b = (char *)second;
	
	i = strcmp(a ,b);
	
	free(a);
	free(b);
	
	return i;
}

char* valueToString(void* val){ //parameter function
	char *a;
	a = (char *)val;
	return a;
}

/*uses DateTime*/

void deleteDate(void* toBeDeleted){
	DateTime *newDate;
	newDate = (DateTime *)toBeDeleted;

	free(newDate->date);
	free(newDate->time);
	free(newDate->text);
	free(newDate);
}

int compareDates(const void* first,const void* second){
	DateTime *newDate1 = (DateTime *)malloc(sizeof(DateTime *));
	DateTime *newDate2 = (DateTime *)malloc(sizeof(DateTime *));
	newDate1 = (DateTime *)first;
	newDate2 = (DateTime *)second;
	
	if((newDate1->isText)&&(newDate2->isText)){
		return strcmp(newDate1->text,newDate2->text);
	}
	return 0;
}

char* dateToString(void* date){
	DateTime *newDate;
	newDate = (DateTime *)date;
	char *copy = (char *)malloc(1000);
	char *dateString = (char *)malloc(1000);

	if(newDate->isText){
		strcpy(copy, newDate->text);
		return(copy);
	}
	else{
		if(strcmp(newDate->date,"") != 0){
			strcpy(dateString, newDate->date);
		}
		if(strcmp(newDate->time,"") != 0){
			strcat(dateString, "T");
			strcpy(copy, newDate->time);
			strcat(dateString, copy);
		}
	}
	
	return(dateString);
}

///***********************A2 code************************///
VCardErrorCode writeCard(const char* fileName, Card** object){
	VCardErrorCode error = OK;
	Node *node = (Node *)malloc(sizeof(Node));
	char *toPrint = (char *)malloc(1000);
	FILE *fp;

	//some error checking
	//check for valid filename
	
	if((fileName == NULL) || (object == NULL)){ //checks if either the card is set to null or filename is set to null
		error = WRITE_ERROR;
		return error;
	}
	
	if(strcmp(fileName, "") == 0){
		error = WRITE_ERROR;
		return error;
	}
	
	if(strlen(fileName) < 5){
		error = WRITE_ERROR;
		return error;
	}
	
	if((fileName[strlen(fileName) - 1] != 'f') || (fileName[strlen(fileName) - 2] != 'c') || (fileName[strlen(fileName) - 3] != 'v') || (fileName[strlen(fileName) - 4] != '.')){
		error = WRITE_ERROR;
		return error;
	}
	
	/*fp = fopen(fileName, "r");
	if(fp != NULL){	//attempt to open file and see if it already exists
		fprintf(stderr, "error occured in third statement\n");
		error = WRITE_ERROR;
		fclose(fp);
		return error;
	}*/
	
	Card *obj = (Card *)malloc(sizeof(Card));
	obj = (*object);
	
	fp = fopen(fileName, "w+");
	fprintf(fp, "BEGIN:VCARD\r\n");
	fprintf(fp, "VERSION:4.0\r\n");
	
	strcpy(toPrint, propertyToString(obj->fn));
	fprintf(fp, "%s\r\n", toPrint);

	if(obj->birthday){
		strcpy(toPrint, "BDAY:");
		strcat(toPrint, dateToString(obj->birthday));
		fprintf(fp, "%s\r\n", toPrint);
	}
	if(obj->anniversary){
		strcpy(toPrint, "ANNIVERSARY:");
		strcat(toPrint, dateToString(obj->anniversary));
		fprintf(fp, "%s\r\n", toPrint);
	}

	if(obj->optionalProperties){
		node = obj->optionalProperties->head;
		while(node){
			strcpy(toPrint, propertyToString(node->data));
			fprintf(fp, "%s\r\n", toPrint);
			node = node->next;
		}
	}
	
	fprintf(fp, "END:VCARD\r\n");
	fclose(fp);
	
	free(toPrint);

	return error;
}

VCardErrorCode validatePropertyName(char * propName){
	int i = 0;
	
	if(propName == NULL){
		return INV_CARD;
	}
	if(strcmp(propName, "") == 0){
		return INV_CARD;
	}
	for(i = 0; i < 38; i++){
		if(strcmp(propName,PROPERTY_NAMES[i]) == 0){
			return OK;
		}
	}
	
	return INV_CARD;
}

VCardErrorCode validateParameter(Parameter *parameter){
	if((parameter->name == NULL) || (parameter->value == NULL)){
		return INV_PROP;
	}
	return OK;
}

VCardErrorCode validateCard(const Card* obj){
	VCardErrorCode error = OK;
	Node *propertyNode = (Node *)malloc(sizeof(Node));
	Node *parameterNode = (Node *)malloc(sizeof(Node));
	
	Property *tempProperty = (Property *)malloc(sizeof(Property));
	Parameter *tempParameter = (Parameter *)malloc(sizeof(Parameter));
	
	bool FNchecker = false;
	bool kindChecker = false;
	//check card
	
	if(obj == NULL){
		error = INV_CARD;
		return error;
	}
	
	//check manadatory function
	if(obj->fn == NULL){ //check if property is null
		error = INV_CARD;
		return error;
	}
	if(obj->fn->name == NULL){
		error = INV_PROP;
		return error;
	}
	fprintf(stderr, "\"%s \"", obj->fn->name);
	error = validatePropertyName(obj->fn->name); //checks for valid property name
	if(error != OK){
		return error;
	}

	if(strcmp(obj->fn->name, "FN") == 0){
		FNchecker = true;
	}
	if(strcmp(obj->fn->name, "KIND") == 0){
		kindChecker = true;
	}
		
	parameterNode = obj->fn->parameters->head; //checks for valid parameters, has name and value
	while(parameterNode){
		tempParameter = (Parameter *)parameterNode->data;
		error = validateParameter(tempParameter);
		if(error != OK){
			return error;
		}
		parameterNode = parameterNode->next;
	}
		
	if(obj->fn->values == NULL){ //checks if value is null
		error = INV_PROP;
		return error;
	}
	if(obj->fn->values->length < 1){
		error = INV_PROP;
		return error;
	}
		/*else{ //checks if value is valid
			valueNode = obj->fn->values->head;
			//check listLength
			while(valueNode){
				//check for valid values here
				valueNode = valueNode->next;
			}
		}*/
	if(obj->optionalProperties == NULL){
		error = INV_CARD;
		return error;
	}
	if(obj->optionalProperties){
		propertyNode = obj->optionalProperties->head;
		
		while(propertyNode){
			if(propertyNode->data == NULL){
				error = INV_PROP;
				return error;
			}
			tempProperty = (Property *)propertyNode->data;
			if(tempProperty->name == NULL){
				error = INV_PROP;
				return error;
			}	
			error = validatePropertyName(tempProperty->name); //checks for valid property name
			if(error != OK){
				return error;
			}
			if(strcmp(tempProperty->name, "FN") == 0){ //checks for FN cardinality
				if(FNchecker){
					error = INV_CARD;
					return error;
				}
				else{
					FNchecker = true;
				}
			}
			if(strcmp(tempProperty->name, "KIND") == 0){
				if(kindChecker){
					error = INV_PROP;
					return error;
				}
				else{
					kindChecker = true;
				}
			}
			
			if(strcmp(tempProperty->name, "VERSION") == 0){ //checks for FN cardinality
				if(FNchecker){
					error = INV_CARD;
					return error;
				}
			}
			if(tempProperty->values->length < 1){
				error = INV_PROP;
				return error;
			}
			
			parameterNode = tempProperty->parameters->head; //checks for valid parameters, has name and value
			while(parameterNode){
				tempParameter = (Parameter *)parameterNode->data;
				error = validateParameter(tempParameter);
				if(error != OK){
					return error;
				}
				parameterNode = parameterNode->next;
			}
		
			if(tempProperty->values == NULL){ //checks if value is null
				error = INV_PROP;
				return error;
			}
			propertyNode = propertyNode->next;
		}
	}

	if(!FNchecker){
		error =	INV_CARD;
		return error;
	}
	
	if(obj->birthday){
		if(obj->birthday->isText){
			if(strcmp(obj->birthday->text, "stuff") == 0){
				error = INV_DT;
				return error;
			}
			if(strcmp(obj->birthday->text, "") == 0){
				error = INV_DT;
				return error;
			}
			if(strcmp(obj->birthday->date, "") != 0){
				error = INV_DT;
				return error;
			}
			if(strcmp(obj->birthday->time, "") != 0){
				error = INV_DT;
				return error;
			}
		}
		else{
			if(strcmp(obj->birthday->text, "") != 0){
				error = INV_DT;
				return error;
			}
		}
	}
	
	if(obj->anniversary){
		if(obj->anniversary->isText){
			if(strcmp(obj->anniversary->text, "") == 0){
				error = INV_DT;
				return error;
			}
			if(strcmp(obj->anniversary->date, "") != 0){
				error = INV_DT;
				return error;
			}
			if(strcmp(obj->anniversary->time, "") != 0){
				error = INV_DT;
				return error;
			}
		}
		else{
			if(strcmp(obj->anniversary->text, "") != 0){
				error = INV_DT;
				return error;
			}
		}
	}

	return error;
}

char* strListToJSON(const List* strList){
	char *temp = (char *)malloc(1000);
	char *output = (char *)malloc(100000);
	
	Node *node = (Node *)malloc(sizeof(Node));
	
	strcpy(output, "[");
	if(strList == NULL){
		return NULL;
	}
	if(strList->length == 0){
		strcat(output, "]");
		return output;
	}
	
	node = strList->head;
	while(node){
		strcat(output, "\"");
		temp = (char *)(node->data);
		strcat(output, temp);
		strcat(output, "\",");
		node = node->next;
	}
	output[strlen(output) - 1] = ']';
	
	return output;
}

char *removeQuotations(char *toRemove){
	char *output = (char *)malloc(1000);
	int i = 0;
	
	if(strcmp(toRemove, "\"\"") == 0){
		strcpy(output, "");
		return output;
	}
	
	for(i = 0; i < strlen(toRemove) - 2; i++){
		output[i] = toRemove[i + 1];
	}
	output[i] = '\0';
	return output;
}

List* JSONtoStrList(const char* str){
	List *output = initializeList(&valueToString, &deleteValue, &compareValues);
	char *temp = (char *)malloc(10000);
	char **grab = (char **)malloc(sizeof(char *));
	bool turnToSemiColons = true;
	int i = 0;
	int numSemicolons = 0;
	
	if(str == NULL){
		return NULL;
	}
	if(str[0] != '[' || str[1] != '\"'){
		return NULL;
	}
	if(str[strlen(str)-1] != ']' || str[strlen(str) -2] != '\"'){
		return NULL;
	}
	
	strcpy(temp, str);
	for(i = 0; i < strlen(temp); i++){
		if(temp[i] == '\"'){
			turnToSemiColons = !turnToSemiColons;
			continue;
		}
		if(turnToSemiColons == true){
			temp[i] = ';';
			numSemicolons = numSemicolons + 1;
		}
	}
	grab[0] = (char *)malloc(1000);
	grab[0] = strtok(temp, ";");
	grab = (char **)realloc(grab, sizeof(char *) * 2);
	
	for(i = 1; i < numSemicolons - 1; i++){
		grab[i] = (char *)malloc(1000);
		grab[i] = strtok(NULL, ";");
		grab = (char **)realloc(grab, sizeof(char *) * (i + 2));
	}

	for(i = 0; i < numSemicolons - 1; i++){
		strcpy(grab[i], removeQuotations(grab[i]));
		insertBack(output, grab[i]);
	}
	return output;
}

char* propToJSON(const Property* prop){
	char* output = (char *)malloc(10000);
	if(prop == NULL){
		strcpy(output,"");
		return output;
	}
	if(strcmp(prop->name, "") == 0){
		strcpy(output,"");
		return output;
	}
	if(prop->values->length < 1){
		strcpy(output,"");
		return output;
	}

	strcpy(output,"{");
	strcat(output, "\"group\":\"");
	if(strcmp(prop->group, "") != 0){
		strcat(output, prop->group);
	}
	strcat(output, "\",\"name\":\"");
	strcat(output, prop->name);
	strcat(output, "\",\"values\":");
	strcat(output, strListToJSON(prop->values));
	strcat(output,"}");
	
	return output;
}

Property* JSONtoProp(const char* str){
	Property *outputProperty = (Property *)malloc(sizeof(Property));
	outputProperty->name = (char *)malloc(sizeof(char *));
	outputProperty->group = (char *)malloc(sizeof(char *));
	outputProperty->values = initializeList(&valueToString, &deleteValue, &compareValues);
	outputProperty->parameters = initializeList(&parameterToString, &deleteParameter, &compareParameters);
	
	char **grab = (char **)malloc(sizeof(char *));
	char *temp = (char *)malloc(1000);
	bool turnToSemiColons = true;
	bool keepSkipping = false;
	int numSemicolons = 0;
	int i = 0;
	
	if(str == NULL){
		return NULL;
	}
	if(str[0] != '{' || str[strlen(str)-1] != '}'){
		return NULL;
	}
	if(str[1] != '\"' || str[strlen(str) -2] != ']'){
		return NULL;
	}

	strcpy(temp, str);
	for(i = 0; i < strlen(temp); i++){
		if(temp[i] == '['){
			keepSkipping = true;
		}
		else if(temp[i] == ']'){
			keepSkipping = false;
			continue;
		}
		
		if(keepSkipping){
			continue;
		}
		
		if(temp[i] == '\"'){
			turnToSemiColons = !turnToSemiColons;
			continue;
		}
		if(turnToSemiColons == true){
			temp[i] = ';';
			numSemicolons = numSemicolons + 1;
		}
	}

	if(numSemicolons != 7){
		return NULL;
	}

	grab[0] = (char *)malloc(1000);
	grab[0] = strtok(temp, ";");
	grab = (char **)realloc(grab, sizeof(char *) * 2);
	
	for(i = 1; i < numSemicolons - 1; i++){
		grab[i] = (char *)malloc(1000);
		grab[i] = strtok(NULL, ";");
		grab = (char **)realloc(grab, sizeof(char *) * (i + 2));
	}

	if(strcmp(removeQuotations(grab[0]), "group") != 0){
		return NULL;
	}
	if(strcmp(removeQuotations(grab[2]), "name") != 0){
		return NULL;
	}
	if(strcmp(removeQuotations(grab[4]), "values") != 0){
		return NULL;
	}

	strcpy(outputProperty->group, removeQuotations(grab[1])); 
	strcpy(outputProperty->name, removeQuotations(grab[3]));
	outputProperty->values = JSONtoStrList(grab[5]);

	return outputProperty;
}

char* dtToJSON(const DateTime* prop){
	char *output = (char *)malloc(1000);
	
	if(prop == NULL){
		strcpy(output, "");
		return output;
	}
	
	strcpy(output, "{\"isText\":");
	if(prop->isText == true){
		strcat(output, "true");
	}
	else{
		strcat(output, "false");
	}
	strcat(output,",\"date\":\"");
	strcat(output, prop->date);
	strcat(output, "\",\"time\":\"");
	strcat(output, prop->time);
	strcat(output, "\",\"text\":\"");
	strcat(output, prop->text);
	strcat(output, "\",\"isUTC\":");
	if(prop->UTC == true){
		strcat(output, "true");
	}
	else{
		strcat(output, "false");
	}
	strcat(output,"}");
	
	return output;
}

DateTime* JSONtoDT(const char* str){
	DateTime *outputDt = (DateTime *)malloc(sizeof(DateTime));
	char **grab = (char **)malloc(sizeof(char *));
	char *temp = (char *)malloc(1000);
	char *operator = (char *)malloc(1000);
	char *value = (char *)malloc(1000);
	int i = 0;
	int commaCounter = 0;
	
	if(str == NULL){
		return NULL;
	}
	if(str[0] != '{' || str[strlen(str) -1] != '}'){
		return NULL;
	}
	for(i = 0; i < strlen(str); i++){
		if(str[i] == '['){
			return NULL;
		}
		if(str[i] == ']'){
			return NULL;
		}
	}
	
	strcpy(temp, str);
	for(i = 0; i < strlen(temp) - 1; i++){
		temp[i] = temp[i + 1];
	}
	temp[i - 1] = '\0';
	
	for(i = 0; i < strlen(temp); i++){
		if(temp[i] == ','){
			commaCounter = commaCounter + 1;
		}
	}
	
	if(commaCounter != 4){
		return NULL;
	}
	
	grab[0] = (char *)malloc(1000);
	grab[0] = strtok(temp, ",");
	grab = (char **)realloc(grab, sizeof(char *) * 2);
	for(i = 1 ; i < commaCounter + 1; i++){
		grab[i] = (char *)malloc(1000);
		grab[i] = strtok(NULL, ",");
		grab = (char **)realloc(grab, sizeof(char *) * (i + 2));
	}
	
	for(i = 0; i < commaCounter + 1; i++){
		operator = strtok(grab[i], ":");
		value = strtok(NULL, "");
		
		if(strcmp(removeQuotations(operator), "isText") == 0){
			if(strcmp(value, "false") == 0){
				outputDt->isText = false; 
			}
			else if(strcmp(value, "true") == 0){
				outputDt->isText = false;
			}
			else{
				return NULL;
			}
		}
		else if(strcmp(removeQuotations(operator), "isUTC") == 0){
			if(strcmp(value, "false") == 0){
				outputDt->UTC = false; 
			}
			else if(strcmp(value, "true") == 0){
				outputDt->UTC = true;
			}
			else{
				return NULL;
			}
		}
		else if(strcmp(removeQuotations(operator), "date") == 0){
			value = removeQuotations(value);
			if((outputDt->isText) && (strcmp(value, "") != 0)){
				return NULL;
			}
			else{
				outputDt->date = (char *)malloc(1000);
				strcpy(outputDt->date, value);
			}
		}
		else if(strcmp(removeQuotations(operator), "time") == 0){
			value = removeQuotations(value);
			if((outputDt->isText) && (strcmp(value, "") != 0)){
				return NULL;
			}
			else{
				outputDt->time = (char *)malloc(1000);
				strcpy(outputDt->time, value);
			}
		}
		else if(strcmp(removeQuotations(operator), "text") == 0){
			value = removeQuotations(value);
			if((outputDt->isText) && (strcmp(value, "") == 0)){
				return NULL;
			}
			else{
				outputDt->text = (char *)malloc(1000);
				strcpy(outputDt->text, value);
			}
		}
	}
	
	return outputDt;
}

char* CardtoJSON(Card** obj){
	char *output = (char *)malloc(10000);
	Node *node = (Node *)malloc(sizeof(Node));
	Card *object = (*obj);
	strcpy(output, "{\"FN\":");
	strcat(output, propToJSON(object->fn));
	
	strcat(output, ", \"optionalProperties\":[");
	if(object->optionalProperties->length > 0){
		node = object->optionalProperties->head;
		while(node->next){
			strcat(output, propToJSON(node->data));
			strcat(output, ",");
			node = node->next;
		}
		output[strlen(output)-1] = ']';
	}
	if(output[strlen(output)-1] != ']'){
		strcat(output, "]");
	}
	
	if(object->birthday){
		strcat(output, ",\"birthday\":");
		strcat(output, dtToJSON(object->birthday));
	}
	if(object->anniversary){
		strcat(output, ",\"anniversary\":");
		strcat(output, dtToJSON(object->anniversary));
	}
	strcat(output, "}");
	
	return output;
}

Card** JSONtoCard(const char* str){
	Card *cardOutput = (Card *)malloc(sizeof(Card));
	int i = 0;
	//int counter = 0;
	//bool foundColon = false;
	char *temp = (char *)malloc(1000);
	char *prop = (char *)malloc(1000);
	char *value = (char *)malloc(1000);
	/*if(str == NULL){
		printf("string did not pass through\n");
		return NULL;
	}
	if(str[0] != '{' || str[strlen(str) - 1] != '}'){
		printf("no brackets\n");
		return NULL;
	}
	for(i = 0; i < strlen(str); i++){
		if(str[i] == ':'){
			foundColon = true;
		}
		else if(str[i] == '\"'){
			counter = counter + 1;
		}
		else if((str[i] == ':') && (foundColon)){
			printf("too many colons\n");
			return NULL;
		}
	}
	if(!foundColon){
		printf("no colons\n");
		return NULL;
	}
	if(counter != 4){
		printf("incorrect number of double quotes\n");
		return NULL;
	}*/
	
	strcpy(temp, str);
	for(i = 0; i < strlen(temp) - 1; i++){
		temp[i] = temp[i + 1];
	}
	temp[i - 1] = '\0';
	
	prop = strtok(temp, ":");
	prop = removeQuotations(prop);
	value = strtok(NULL, "");
	value = removeQuotations(value);
	
	cardOutput->fn = (Property *)malloc(sizeof(Property));
	cardOutput->optionalProperties = initializeList(&propertyToString, &deleteProperty, &compareProperties);
	cardOutput->birthday = NULL;
	cardOutput->anniversary = NULL;
	
	cardOutput->fn = createProperty(&prop, 1, &value, 1);
	Card **outputCard = (Card **)malloc(sizeof(Card *));
	return outputCard;
}

void addProperty(Card* card, const Property* toBeAdded){
	//check for null on either
	void * addMe = (void *)malloc(sizeof(Property));
	if(card == NULL || toBeAdded == NULL){
		//do nothing
	}
	else{
		addMe = (void *)toBeAdded;
		insertBack(card->optionalProperties, addMe);
	}
}

int getNumParameters(Property* countMe){
	return(countMe->parameters->length);
}

int getNumProperties(Card* countMe){
	return(countMe->optionalProperties->length);
}

Card **getEmptyCard(){
	Card **emptyCard = (Card **)malloc(sizeof(Card *));
	
	return emptyCard;
}

Card *getCard(Card **card){
	return (*card);
}

int getNumOfOptionalProperties(Card **obj){
	Card *object = (*obj);
	int total = object->optionalProperties->length;
	if(object->birthday){
		total = total + 1;
	}
	if(object->anniversary){
		total = total + 1;
	}
	
	return total;
}

char *getFN(Card **obj){
	Card *object = (*obj);
	if(object->fn){
		return (char *)(object->fn->values->head->data);
	}
	else{
		return "";
	}
}