#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]) {
	
	unsigned int T = atoi(argv[1]);
	unsigned int E = 1887;
	unsigned int PQ = 3127;


	unsigned int C = pow(T, E);

	C = C % PQ;
	printf("%d\n", C);
	return 0;
}