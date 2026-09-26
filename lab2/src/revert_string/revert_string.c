#include "revert_string.h"

void RevertString(char *str)
{
	int n =0;
	char *strt = str;
	while( *strt != '\0' ){
		n++;
 strt++ ;
	}
	strt--;
	for(int i=0; i<n/2;i++){

    char temp = *(strt-(n-2*i-1)*sizeof(char));
    *(strt-(n-2*i-1)*sizeof(char)) = *strt;
    *strt = temp;
	strt--;

	}

}


