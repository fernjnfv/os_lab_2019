#include "swap.h"

void Swap(char *left, char *right)
{
	char time = *left;
	*left = *right;
	*right = time;
}
