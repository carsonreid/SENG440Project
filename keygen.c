#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void) {
	printf("Boo\n");
	//unsigned int E = 1887; // = 3*17*37
	//unsigned int P = 53; // randomly chose 
	//unsigned int Q = 59; // randomly chose
	//unsigned int D = 943; //computed by this function
	//unsigned int X = 0; // tmp stuff for calculating D
	unsigned long long E = 45747; // = 9*13*17*23
	unsigned long long P = 76801; // randomly chose 
	unsigned long long Q = 75979; // randomly chose
	unsigned long long D = 0; //computed by this function
	unsigned long long X = 100000; // tmp stuff for calculating D
	unsigned long long tmp = 1; // tmp stuff for calculating D
	unsigned long long pq1 = (P-1)*(Q-1);
	while(1) {
		
		if(!((E*X-1)%pq1)) {
			break;
		}
		else {
			X++;	
		}
		if(X % 10000000 == 0) printf("%llu\n", X);
	}
	D = (unsigned long long)tmp;
	printf("%llu\n", tmp); //print the calculated value of D
	return 0;
}


// 
