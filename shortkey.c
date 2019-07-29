#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define LONG_MAX 0xFFFFFFFFUL
#define P_Q 0xC37

void printSmolBinary(unsigned long input[]);


unsigned long * rshift1024(unsigned long * input) {
	register int i = 0;
	register long carryBits = 0;
	for(i = 0; i < 1; i++){
		input[i] += carryBits;
		carryBits = input[i] & 1;
		input[i] >>= 1;
		carryBits <<= 32;
	}
	return input;
}

unsigned long * add1024(unsigned long * a, unsigned long * b) {
	register int i = 0;
	register unsigned int carryFlag = 0;
	for(i = 0; i >= 0; i--){
		a[i] += carryFlag;
		carryFlag = 0;
		if(b[i] > LONG_MAX - a[i]) {
			carryFlag = 1;
		}
		a[i] += b[i];
	}
	return a;
}

int alargerequalb1024(unsigned long * a, unsigned long * b){
	register int i;
	long temp;
	for(i = 0; i < 1; i++){
		temp = a[0]-b[0];
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
unsigned long * subtract1024(unsigned long * T, unsigned long * M){ 
	unsigned long tmpT;
	unsigned long tmpM;
	unsigned int cfi;
	unsigned int carryCheckBit;

	register int i = 0;
	for(; i < 32; i++){
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
	return T;
}

unsigned long * bitwiseMMM(unsigned long * y, unsigned long * x, unsigned long * PQptr) {
	int i;
	unsigned long * T = calloc(1, sizeof(unsigned long));
	unsigned char xi;
	unsigned char eta;
	//printSmolBinary(x);
	//printSmolBinary(y);
	//printSmolBinary(PQptr);
	//printf("\n\n");

	for(i = 0; i < 8; i++){
		
		xi = (x[i/32] & (0x1 << i%32)) >> i%32;

		//printf("i=%d first chunk: %ld, second: %ld, ", i, (*T) & 0x1, (xi & (y[0] & 1)));

		eta = ((*T) & 0x1) ^ (xi & (y[0] & 0x1));
		//printf("%d %d ", xi, eta);
		//printSmolBinary(T);
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
				unsigned long * tmp = calloc(1, sizeof(unsigned long));
				add1024(tmp, PQptr);
				add1024(tmp, y);
				add1024(T, tmp);
				//free(tmp);
				T = rshift1024(T);
			}
		}
		//printSmolBinary(T);
	}
	if(alargerequalb1024(T, PQptr)) {
		
		//printf("T 	: %lu\n", T[0]);
		//printf("PQ 	: %lu\n", PQptr[0]);
		T = subtract1024(T, PQptr);
		//printf("TPQ	: %lu\n\n", T[0]);
	}
	//printf("\n");
	return T;
}

void printSmolBinary(unsigned long input[]) {
	int i = 0;
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


unsigned long * copy1024(unsigned long * copyTo, unsigned long * copyFrom){
	int i;
	for(i = 0; i < 1; i++){
		copyTo[i] = copyFrom[i];
	}
	return copyTo;
}

void printHex(unsigned long * input) {
	int i = 0;
	for(; i >= 0; i--){
		printf("%08lX", input[i]);
	}
	fflush(stdout);
}


int main(int argc, char *argv[]) {

	unsigned long ptn = 0;
	for(ptn = 0; ptn < 133; ptn++) {
	unsigned long * D = calloc(1, sizeof(unsigned long));
	D[0] = 65;

	unsigned long * PQ = calloc(1, sizeof(unsigned long));
	PQ[0] = 133;

	unsigned long E = 5;

	unsigned long * Rsq = calloc(1, sizeof(unsigned long));
	Rsq[0] = 100;
	
	unsigned long * R = calloc(1, sizeof(unsigned long));
	R[0] = 123;

	unsigned long * testT = calloc(1, sizeof(unsigned long));
	testT[0] = ptn;//0x0A; //10
	//printf("TestT: ");
	//printSmolBinary(testT);
	//printf("\n");
	
	unsigned long * tToPowerOf2 = calloc(1, sizeof(unsigned long));
	tToPowerOf2[0] = testT[0];

	int i = 0;
	//printHex(testT);
	//printf("\n\n\n");

	/*								 */
	/*            Encrypt            */
	/*								 */

	unsigned long * rollingTVal = R;
	unsigned long * tmp1; //used for freeing
	//printf("mmm with 0x0A\n");
	//printf("Plaintext: ");
	//printHex(testT);
	//printf("\n");
	testT = bitwiseMMM(testT, Rsq, PQ); //testT is now   TR mod PQ

	if(E & 1) {
		//printf("needed a power, 1\n");
		rollingTVal = bitwiseMMM(rollingTVal, testT, PQ); //get T^1 if we need it
		//free(tmp1); want this eventually since tmp1 is R but we need it for decrypting in the same c file
		tmp1 = rollingTVal;
	}
	//printHex(rollingTVal);
	//printf("\naaaaaaaaaaaaaaa\n");
	tToPowerOf2 = bitwiseMMM(testT, testT, PQ); //T^2R
	//free(testT);
	

	unsigned long * tmp2 = tToPowerOf2; //used to free the old tToPowerOf2 chunk
	for(i = 1; i < 3; i++){ //17 is bit length of E
		if(E & (1 << i)) {
			//printf("needed a power, %d\n", i);
			rollingTVal = bitwiseMMM(rollingTVal, tToPowerOf2, PQ);
			//free(tmp1);
			tmp1 = rollingTVal;
		}
		tToPowerOf2 = bitwiseMMM(tToPowerOf2, tToPowerOf2, PQ); //get the next power of 2
		//free(tmp2);
		tmp2 = tToPowerOf2;
	}

	//descale (T^n)*R
	unsigned long * one = calloc(1, sizeof(unsigned long));
	one[0] = 1;
	rollingTVal = bitwiseMMM(rollingTVal, one, PQ);

	//printf("Encrypted value: ");
	//printHex(rollingTVal);
	//printf("\n");



	//testT = copy1024(testT, rollingTVal); //set the input to decrypt to be the encrypted text
	testT = rollingTVal;











	/*								 */
	/*            Decrypt            */
	/*								 */
	
	rollingTVal = R;
	testT = bitwiseMMM(testT, Rsq, PQ); //testT is now   TR mod PQ

	if(D[0] & 1) {
		rollingTVal = bitwiseMMM(rollingTVal, testT, PQ); //get T^1 if we need it
		//free(tmp1);
		tmp1 = rollingTVal;
	}

	tToPowerOf2 = bitwiseMMM(testT, testT, PQ); //T^2R
	//free(testT);
	//decrypt

	tmp2 = tToPowerOf2; //used to free the old tToPowerOf2 chunk
	for(i = 1; i < 7; i++){ //1024 is length of D
		if(D[i/32] & (1 << i%32)) {
			rollingTVal = bitwiseMMM(rollingTVal, tToPowerOf2, PQ);
			//free(tmp1);
			tmp1 = rollingTVal;
		}
		tToPowerOf2 = bitwiseMMM(tToPowerOf2, tToPowerOf2, PQ); //get the next power of 2
		//free(tmp2);
		tmp2 = tToPowerOf2;
	}

	//descale (T^n)*R
	rollingTVal = bitwiseMMM(rollingTVal, one, PQ);
	//printf("This is the decrypted value: ");
	//printHex(rollingTVal); //should be plaintext now
	//printf("\n");
	if(rollingTVal[0] != ptn) {
		printf("BAD BOI: %lu\n", ptn);
	}
}
	return 0;
}	

