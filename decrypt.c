#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define LONG_MAX 0xFFFFFFFFUL
#define P_Q 0xC37

void printSmolBinary(unsigned long input[]);


unsigned long * rshift1024(unsigned long * input) {
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

unsigned long * add1024(unsigned long * a, unsigned long * b) {
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

unsigned long * add64(unsigned long * a, unsigned long * b) {
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

unsigned long * prescale1024(unsigned long * input, int numInputLongs){
	long * prescaled = calloc(32+numInputLongs, sizeof(unsigned long));
	long * tmp = prescaled;
	register int i;
	for(i = 0; i < numInputLongs; i++){
		*tmp = input[i];
		*tmp++;
	}
	return prescaled;
}

unsigned long * padto1024(unsigned long * input, int numInputLongs){
	long * padded = calloc(32, sizeof(unsigned long));
	long * tmp = padded+(32-numInputLongs);
	register int i;
	for(i = 0; i < numInputLongs; i++){
		*tmp = input[i];
		*tmp++;
	}
	return padded;
}

unsigned long * padto64(unsigned long * input, int numInputLongs){
	long * padded = calloc(2, sizeof(unsigned long));
	long * tmp = padded+(2-numInputLongs);
	register int i;
	for(i = 0; i < numInputLongs; i++){
		*tmp = input[i];
		*tmp++;
	}
	return padded;
}

int alargerequalb1024(unsigned long * a, unsigned long * b){
	register int i;
	long temp;
	for(i = 0; i < 32; i++){
		temp = a[31-i]-b[31-i];
		if(temp > 0) {
			return 1;
		}
		else if(temp < 0) {
			return 0;
		}
	}
	return 1;
}

int alargerb64(long * a, long * b){
	register int i;
	long temp;
	for(i = 0; i < 2; i++){
		temp = a[1-i]-b[1-i];
		if(temp > 0) {
			return 1;
		}
		else if(temp < 0) {
			return 0;
		}
	}
	return 1;
}

/*
* This function does bitwise unsigned subtraction. It works cause T will always be > M,
* and the nasty bit does the borrow part of subtraction
*/
long * subtract1024(unsigned long * T, unsigned long * M){ 
	unsigned long tmpT;
	unsigned long tmpM;
	unsigned int cfi;
	unsigned int carryCheckBit;

	register int i = 0;
	for(; i < 1024; i++){
		tmpT = T[i/32] & (0x1 << i%32); //maybe replace i%32 with a temp
		tmpM = M[i/32] & (0x1 << i%32);
		if(tmpT & tmpM) { //1-1
			T[i/32] = T[i/32] ^ (0x1 << i%32); 
			//set T(i) to 0
		}
		else if(!tmpM) {} //1-0 or 0-0
		else { //0-1
			//borrow
			cfi = i;
			carryCheckBit = (T[i/32] & (0x1 << i%32)) >> i%32;
			while(!carryCheckBit){
				T[cfi/32] = T[cfi/32] | (0x1 << cfi%32);
				cfi++;
				carryCheckBit = (T[cfi/32] & (0x1 << cfi%32)) >> cfi%32;
			}
			T[cfi/32] = T[cfi/32] ^ (0x1 << cfi%32);
		}
	}
}

long * subtract64(unsigned long * T, unsigned long * M){ 
	unsigned long tmpT;
	unsigned long tmpM;
	unsigned int cfi;
	unsigned int carryCheckBit;
    register int imod;
	register int i;
	for(i = 0; i < 64; i++){
        imod = i%32;
		tmpT = T[i/32] & (0x1 << imod); //maybe replace i%32 with a temp
		tmpM = M[i/32] & (0x1 << imod);
		if(tmpT & tmpM) { //1-1
			T[i/32] = T[i/32] ^ (0x1 << imod);
		}
		else if(!tmpM) {} //1-0 or 0-0
		else { //0-1
			//borrow
			cfi = i;
			carryCheckBit = (T[i/32] & (0x1 << imod)) >> imod;
			while(!carryCheckBit){
				T[cfi/32] = T[cfi/32] | (0x1 << cfi%32);
				cfi++;
				carryCheckBit = (T[cfi/32] & (0x1 << cfi%32)) >> cfi%32;
			}
			T[cfi/32] = T[cfi/32] ^ (0x1 << cfi%32);
		}
	}
	//printf("\n");
	return T;
}

unsigned long * bitwiseMMM(unsigned long * x, unsigned long * y, unsigned long * PQptr) {
	int i;
	unsigned long * T = calloc(32, sizeof(unsigned long));
	unsigned char xi;
	unsigned char eta;

	for(i = 0; i < 1024; i++){
		xi = (x[i/32] & (0x1 << i%32)) >> i%32;
		eta = ((*T) & 0x1) ^ (xi & (y[0] & 0x1));
		if(xi == 0) {
			if(eta == 0) {
				T = rshift1024(T);
			}
			else {
				T = rshift1024(add1024(T, PQptr));
			}
		}
		else {
			if(eta == 0){
				T = rshift1024(add1024(T, y));
			}
			else {
				unsigned long * tmp = calloc(32, sizeof(unsigned long));
				add1024(tmp, T);
				add1024(tmp, y);
				add1024(T, tmp);
				T = rshift1024(T);
			}
		}
	}

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
	int i = 1;
	int j = 0;
	while (i >= 0) {
		for(j = 31; j >= 0; j--){
			if ((input[i] >> (j)) & 1)
				printf("1");
			else
				printf("0");
		}
		
		if(i == 0)
			;
		else {
			printf(" | ");
		}
		i--;
	}
	printf("\n");
}

int main(int argc, char *argv[]) {
	
	//unsigned int T = atoi(argv[1]);
	unsigned int D = 943;
	unsigned int PQ = 3127;

/*
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
	
	printSmolBinary(arry);
	long * prescaled = prescale1024(arry, 2);
	printSmolBinary(prescaled);
	

	unsigned long shorty[1] = { 2041 };
	printSmolBinary(shorty);

	long * padded64 = padto64(shorty,1);
	printSmolBinary(padded64);
	*/
	unsigned long * atest = calloc(2, sizeof(unsigned long));
	unsigned long * btest = calloc(2, sizeof(unsigned long));
	atest[0] = 1 + (1 << 5);
	atest[1] = 0;
	btest[0] = 0;
	btest[1] = 4;

	printSmolBinary(btest);
	printSmolBinary(atest);
	btest = subtract64(btest, atest);
	printSmolBinary(btest);

	printf("green\n");




/*
	unsigned long * xtest = calloc(32, sizeof(unsigned long));
	unsigned long * ytest = calloc(32, sizeof(unsigned long));
	xtest[0] += 17;
	ytest[0] += 22;
	unsigned long * PQarr = calloc(32, sizeof(unsigned long));
	PQarr[0] = 23;

	printSmolBinary(bitwiseMMM(xtest, ytest, PQarr));
*/
	return 0;
}	
