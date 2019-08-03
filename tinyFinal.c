#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <time.h>
#define LONG_MAX 0xFFFFFFFFUL
#define newline printf("\n");

void printSmolBinary(uint32_t input[]);
void printHex(uint32_t * input);


uint32_t * rshift1024(uint32_t * input) {
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

uint32_t * add1024(uint32_t * a, uint32_t * b) {
	register int i = 0;
	register uint32_t carryFlag = 0;
	for(i = 0; i >= 0; i--){
		a[i] += carryFlag;
		carryFlag = 0;
		if(b[i] > 0xFFFFFFFF - a[i]) {
			carryFlag = 1;
		}
		a[i] += b[i];
	}
	return a;
}

int alargerequalb1024(uint32_t * a, uint32_t * b){
	register int i;
	for(i = 63; i >= 0; i--){
		uint32_t abit = a[i/32] & (1 << i%32);
		uint32_t bbit = b[i/32] & (1 << i%32);
		if(abit && !bbit) return 1;
		else if(!abit && bbit) return 0;
	}
	return 1;
}

/*
* This function does bitwise unsigned subtraction. It works cause T will always be > M,
* and the nasty bit does the borrow part of subtraction
*/
uint32_t * subtract1024(uint32_t * T, uint32_t * M){ 
	uint32_t tmpT;
	uint32_t tmpM;
	uint32_t cfi;
	uint32_t carryCheckBit;

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

uint32_t * bitwiseMMM(uint32_t * y, uint32_t * x, uint32_t * PQptr) {
	int i;
	uint32_t * T = calloc(1, sizeof(uint32_t));
	unsigned char xi;
	unsigned char eta;

	for(i = 0; i < 8; i++){
		
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
				uint32_t * tmp = calloc(1, sizeof(uint32_t));
				add1024(tmp, PQptr);
				add1024(tmp, y);
				add1024(T, tmp);
				T = rshift1024(T);
			}
		}
	}
	int alargereq = alargerequalb1024(T, PQptr);
	if(alargereq) {
		T = subtract1024(T, PQptr);
	}
	return T;
}

void printSmolBinary(uint32_t input[]) {
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


uint32_t * copy1024(uint32_t * copyTo, uint32_t * copyFrom){
	int i;
	for(i = 0; i < 1; i++){
		copyTo[i] = copyFrom[i];
	}
	return copyTo;
}

void printHex(uint32_t * input) {
	int i = 0;
	for(; i >= 0; i--){
		//printf("%"PRIx32, input[i]);
		printf("%08X", input[i]);
	}
	fflush(stdout);
}

uint32_t * split7Bits(uint32_t * inputArray) {
	uint32_t * splitArray = calloc(128, sizeof(uint32_t));
	int i;
	for(i = 0; i < 896; i++) {
		splitArray[i/7] |= (((inputArray[i/32] & (1 << 31-i%32)) >> 31-i%32) << 6-i%7);
	}
	return splitArray;
}

int main(int argc, char *argv[]) {
	uint32_t ptn = 0;
	uint32_t * inputText = calloc(28, sizeof(uint32_t));
	int j = 0;

	//srand(time(NULL)); //TODO: uncomment this
	for (j = 0; j < 28; j++)
	{
		int randy = rand();
		inputText[j] = (randy % 0xFFFFFFFE)+1;
		printHex(inputText+j); printf(" ");
	} //have to process this in 7-bit chunks
	newline
	uint32_t * outputText = calloc(28, sizeof(uint32_t));
	j = 0;

	uint32_t * D = calloc(1, sizeof(uint32_t));
	D[0] = 65;

	uint32_t * PQ = calloc(1, sizeof(uint32_t));
	PQ[0] = 133;

	uint32_t E = 5;

	uint32_t * Rsq = calloc(1, sizeof(uint32_t));
	Rsq[0] = 100;
	
	uint32_t * R = calloc(1, sizeof(uint32_t));
	R[0] = 123;

	uint32_t * testT = calloc(1, sizeof(uint32_t));
	
	uint32_t * tToPowerOf2 = calloc(1, sizeof(uint32_t));

	uint32_t * splitInputChunks = split7Bits(inputText);


	for(j = 0; j < 128; j++){
		tToPowerOf2[0] = splitInputChunks[j];
		printHex(splitInputChunks+j); printf(" ");
		int i = 0;
		//printHex(testT);
		//printf("\n\n\n");

		/*								 */
		/*            Encrypt            */
		/*								 */

		uint32_t * rollingTVal = R;
		uint32_t * tmp1; //used for freeing
		//printf("mmm with 0x0A\n");
		//printf("Plaintext: ");
		//printHex(testT);
		//printf("\n");
		testT = bitwiseMMM(testT, Rsq, PQ); //testT is now 	\  TR mod PQ
		if(E & 1) {
			//printf("needed a power1, 0\n");
			rollingTVal = bitwiseMMM(rollingTVal, testT, PQ); //get T^1 if we need it
			//free(tmp1); want this eventually since tmp1 is R but we need it for decrypting in the same c file
			tmp1 = rollingTVal;
		}
		//printHex(rollingTVal);
		//printf("\naaaaaaaaaaaaaaa\n");
		tToPowerOf2 = bitwiseMMM(testT, testT, PQ); //T^2R
		//free(testT);
		

		uint32_t * tmp2 = tToPowerOf2; //used to free the old tToPowerOf2 chunk
		for(i = 1; i < 3; i++){ //3 is bit length of E
			if(E & (1 << i)) {
				//printf("needed a power1, %d\n", i);
				rollingTVal = bitwiseMMM(rollingTVal, tToPowerOf2, PQ);
				//free(tmp1);
				tmp1 = rollingTVal;
			}
			tToPowerOf2 = bitwiseMMM(tToPowerOf2, tToPowerOf2, PQ); //get the next power of 2
			//free(tmp2);
			tmp2 = tToPowerOf2;
		}

		//descale (T^n)*R
		uint32_t * one = calloc(1, sizeof(uint32_t));
		one[0] = 1;
		rollingTVal = bitwiseMMM(rollingTVal, one, PQ);

		//printf("Encrypted value: ");
		//printHex(rollingTVal);
		//printf("\n");

		testT = rollingTVal;

		/*								 */
		/*            Decrypt            */
		/*								 */
		
		rollingTVal = R;
		testT = bitwiseMMM(testT, Rsq, PQ); //testT is now   TR mod PQ

		if(D[0] & 1) {
			//printf("needed a power2, 0\n");
			rollingTVal = bitwiseMMM(rollingTVal, testT, PQ); //get T^1 if we need it
			//free(tmp1);
			tmp1 = rollingTVal;
		}

		tToPowerOf2 = bitwiseMMM(testT, testT, PQ); //T^2R
		//free(testT);
		//decrypt

		tmp2 = tToPowerOf2; //used to free the old tToPowerOf2 chunk
		for(i = 1; i < 7; i++){ //7 is length of D
			if(D[i/32] & (1 << i%32)) {
				//printf("needed a power2, %d\n", i);
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
			printf("bad boi\n");
			//printf("BAD BOI: %lu\n", ptn);
		}
		else {
			//printf("Decrypted again:");
			//printHex(rollingTVal);
			//newline
		}
	}
	newline
	return 0;
}	

