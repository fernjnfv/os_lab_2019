#include "revert_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RevertString(char* str)
{
	int len = strlen(str);

	char* copy = (char*) malloc(len+1);
	for (int i = 0; i < len; i++)
		copy[i] = str[len-i-1];
	
	copy[len] = '\0';
	
	strcpy(str, copy);
}

