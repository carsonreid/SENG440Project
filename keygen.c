#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void) {
	printf("Boo\n");
	unsigned int E = 1887; // = 3*17*37
	unsigned int P = 53; // randomly chose // PQ = 000000000000000003127
	unsigned int Q = 59; // randomly chose
	unsigned int D = 943; //computed by this function
	unsigned int X = 0; // tmp stuff for calculating D
	float tmp = 0.1; // tmp stuff for calculating D
	float phi = (P-1)*(Q-1);
	while(1) {
		tmp = (float)(X*phi+1)/(float)E;
		printf("%f\n", tmp);
		if(floor(tmp) == tmp) {
			break;
		}
		else {
			X++;	
		}
	}
	D = (int)tmp;
	printf("%f\n", tmp); //print the calculated value of D
	return 0;
}


// 
