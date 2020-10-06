#include <stdio.h>
#include "hamming.h"
#include "hamming.c"

int main(){
	HammingWord testWord = 10;
	for(int j=1; j < 9; j++){
		printf("Is %i a parity position %i", j, is_parity_position(j));
	}		

	return 0;

}
