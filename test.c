#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void) {
	printf("Boo\n");
	unsigned int E =  1443; // = 3*13*37
	unsigned int P = 53; // randomly chose
	unsigned int Q = 59; // randomly chose
	unsigned int D = 11832; //computed by this function
	unsigned int X = 1; // tmp stuff for calculating D
	float tmp = 0.1; // tmp stuff for calculating D

	while(floor(tmp) != tmp) {
		tmp = (float)(X*(P-1)*(Q-1)+1)/(float)E;
		X++;
	}
	D = (int)tmp;
	printf("%d\n", D);
	return 0;
}
