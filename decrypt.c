#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define LONG_MAX 0xFFFFFFFFUL

void printSmolBinary(unsigned long input[]);


unsigned long * rshift1024(unsigned long input[]) {
	register int i = 0;
	register long carryBits = 0;
	for(i = 0; i < 32; i++){
		input[i] += carryBits;
		carryBits = input[i] & 3;
		input[i] >>= 2;
		carryBits <<= 32;
	}
	return input;
}

unsigned long * add1024(unsigned long a[], unsigned long b[]) {
	register int i = 0;
	register unsigned int carryFlag = 0;
	for(i = 31; i >= 0; i--){
		a[i] += carryFlag;
		carryFlag = 0;
		if(b[i] > LONG_MAX - a[i]) {
			carryFlag = 1;
		}
		a[i] += b[i];
	}
	return a;
}

unsigned long * add64(unsigned long a[], unsigned long b[]) {
	register int i = 0;
	register unsigned int carryFlag = 0;
	for(i = 1; i >= 0; i--){
		a[i] += carryFlag;
		carryFlag = 0;
		if(b[i] > (LONG_MAX - a[i])) {
			carryFlag = 1;
		}
		a[i] += b[i];
	}
	return a;
}

unsigned long * prescale1024(unsigned long input[], int numInputLongs){
	long * prescaled = calloc(1024+numInputLongs, sizeof(unsigned long));
	long * tmp = prescaled;
	register int i;
	for(i = 0; i < numInputLongs; i++){
		*tmp = input[i];
		*tmp++;
	}
	return prescaled;
}


void printBinary(long input[]) {
	int i = 0;
	int j = 0;
	while (i < 32) {
		for(j = 31; j >= 0; j--){
			if ((input[i] >> (j)) & 1)
				printf("1");
			else
				printf("0");
		}
		i++;
		printf("\n");
	}
	printf("\n");
}

void printSmolBinary(unsigned long input[]) {
	int i = 0;
	int j = 0;
	while (i < 4) {
		for(j = 31; j >= 0; j--){
			if ((input[i] >> (j)) & 1)
				printf("1");
			else
				printf("0");
		}
		i++;
		if(i % 2 == 0)
			printf("\n");
		else {
			printf(" | ");
		}
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	
	//unsigned int T = atoi(argv[1]);
	unsigned int D = 943;
	unsigned int PQ = 3127;


	unsigned long arr[32] = { 15, 16, 10, 99, 19, 14, 20, 255, 20, 20, 77, 30, 30, 30, 40, 40, 40, 50, 50, 50, 50, 60, 60, 60, 10, 40, 0, 50, 50, 50, 50, 56 };
	//unsigned long * newarr = rshift1024(arr);
	unsigned long arry[2] = { 2032, 255 }; // 7 1s shifted left 4, then 8 1s
	//unsigned long * newarry = rshift64(arry);
	/*
	unsigned long addarr[2] = { 2991672907, 1305373492 };
	unsigned long addarr2[2] = { 1303294388, 2989593803 };
	printSmolBinary(addarr);
	printSmolBinary(addarr2);
	unsigned long * addarrresult = add64(addarr, addarr2);
	printSmolBinary(addarrresult);
	
	printf("\n");
	
	unsigned long addarr3[2] = { 1946157070, 2147483648 };
	unsigned long addarr4[2] = { 134217731, 2147483648 };
	printSmolBinary(addarr3);
	printSmolBinary(addarr4);
	unsigned long * addarrresult2 = add64(addarr3, addarr4);
	printSmolBinary(addarrresult2);
	*/
	printSmolBinary(arry);
	long * prescaled = prescale1024(arry, 2);
	printSmolBinary(prescaled);
	return 0;
}	