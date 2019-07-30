#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#define LONG_MAX 0xFFFFFFFFUL
#define P_Q 0xC37
#define newline printf("\n");

void printSmolBinary(uint32_t * input);
void printHex(uint32_t * input);
void print64Hex(uint64_t input);

uint32_t * rshift1024(uint32_t * input) {
	register int i;
	register uint32_t carryBit = 0, nextCarryBit = 0;
	for(i = 1; i >= 0; i--){
		nextCarryBit = input[i] & 1;
		input[i] >>= 1;
		input[i] += carryBit;
		nextCarryBit <<= 31;
		carryBit = nextCarryBit;
	}
	return input;
}

uint32_t * add1024(uint32_t * a, uint32_t * b) {
	register int i = 0;
	register unsigned int carryFlag = 0;
	uint32_t tmpa;
	for(i = 0; i < 2; i++){
		tmpa = a[i];
		a[i] += b[i];
		a[i] += carryFlag;
		carryFlag = 0;
		if(b[i] > (LONG_MAX - tmpa)) {
			carryFlag = 1;
		}
	}
	return a;
}

int alargerequalb1024(uint32_t * a, uint32_t * b){
	register int i;
	for(i = 63; i >= 0; i--){
		//temp = a[1-i]-b[1-i];
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
	unsigned int cfi;
	unsigned int carryCheckBit;

	register int i = 0;
	for(; i < 64; i++){
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
	uint32_t * T = calloc(2, sizeof(uint32_t));
	unsigned char xi;
	unsigned char eta;
	//printSmolBinary(x);
	//printSmolBinary(y);
	//printSmolBinary(PQptr);
	//printf("\n\n");

	for(i = 0; i < 56; i++){
		
		xi = (x[i/32] & (0x1 << i%32)) >> i%32;

		//printf("i=%d first chunk: %ld, second: %ld, ", i, (*T) & 0x1, (xi & (y[0] & 1)));

		eta = (T[0] & 0x1) ^ (xi & (y[0] & 0x1));
		//printf("%d %d ", xi, eta);
		//printSmolBinary(T);
		if(xi == 0) {
			if(eta == 0) {
				uint64_t big = T[1]; big <<= 32; big += T[0];
				T = rshift1024(T);
				uint64_t big2 = T[1]; big2 <<= 32; big2 += T[0];
				if(big & 1) big--;
				big >>= 1;
				if(big != big2) { printf("BAD SHIFT1"); newline print64Hex(big); newline print64Hex(big2); newline }
			}
			else {				
				uint64_t big = T[1]; big <<= 32; big += T[0];
				uint64_t big2 = PQptr[1]; big2 <<= 32; big2 += PQptr[0];
				T = add1024(T, PQptr);
				uint64_t big3 = T[1]; big3 <<= 32; big3 += T[0]; 
				if((big+big2) != big3) { printf("BAD ADD2"); newline print64Hex(big); newline print64Hex(big2); newline print64Hex(big3); newline }

				big2 = T[1]; big2 <<= 32; big2 += T[0];
				if(big2 & 1) big2--;
				big2 >>= 1;
				T = rshift1024(T);
				uint64_t big4 = T[1]; big4 <<= 32; big4 += T[0];
				if(big2 != big4) { printf("BAD SHIFT3"); newline }
			}
		}
		else {
			if(eta == 0){

				uint64_t big = T[1]; big <<= 32; big += T[0];
				uint64_t big2 = y[1]; big2 <<= 32; big2 += y[0];
				T = add1024(T, y);
				uint64_t big3 = T[1]; big3 <<= 32; big3 += T[0]; 
				if((big+big2) != big3) { printf("BAD ADD4"); newline print64Hex(big); newline print64Hex(big2); newline print64Hex(big3); newline }

				big2 = T[1]; big2 <<= 32; big2 += T[0];
				if(big2 & 1) big2--;
				big2 >>= 1;
				T = rshift1024(T);
				uint64_t big4 = T[1]; big4 <<= 32; big4 += T[0];
				if(big2 != big4) { printf("BAD SHIFT5"); newline }
			}
			else {

				uint32_t * tmp = calloc(2, sizeof(uint32_t));
				uint64_t big = tmp[1]; big <<= 32; big += tmp[0];
				uint64_t big2 = y[1]; big2 <<= 32; big2 += y[0];
				tmp = add1024(tmp, y);
				uint64_t big3 = tmp[1]; big3 <<= 32; big3 += tmp[0]; 
				if((big+big2) != big3) { printf("BAD ADD6"); newline print64Hex(big); newline print64Hex(big2); newline print64Hex(big3); newline }


				
				uint64_t big9 = tmp[1]; big9 <<= 32; big9 += tmp[0];
				uint64_t big8 = PQptr[1]; big8 <<= 32; big8 += PQptr[0];
				tmp = add1024(tmp, PQptr);
				uint64_t big7 = tmp[1]; big7 <<= 32; big7 += tmp[0]; 
				if((big9+big8) != big7) { printf("BAD ADD7"); newline print64Hex(big9); newline print64Hex(big8); newline print64Hex(big7); newline }


				uint64_t big11 = T[1]; big11 <<= 32; big11 += T[0];
				uint64_t big12 = tmp[1]; big12 <<= 32; big12 += tmp[0];
				T = add1024(T, tmp);
				uint64_t big13 = T[1]; big13 <<= 32; big13 += T[0]; 
				if((big11+big12) != big13) { printf("BAD ADD8"); newline print64Hex(big11); newline print64Hex(big12); newline print64Hex(big13); newline }

				//free(tmp);

				uint64_t big112 = T[1]; big112 <<= 32; big112 += T[0];
				if(big112 & 1) big112--;
				big112 >>= 1;
				T = rshift1024(T);
				uint64_t big114 = T[1]; big114 <<= 32; big114 += T[0];
				if(big112 != big114) { printf("BAD SHIFT5"); newline }

			}
		}
		//printSmolBinary(T);
	}
	uint64_t tes1 = T[1];
	tes1 <<= 32;
	tes1 += T[0];

	uint64_t tes2 = PQptr[1];
	tes2 <<= 32;
	tes2 += PQptr[0];
	int shouldbelarger = (tes1 >= tes2);

	if(alargerequalb1024(T, PQptr)) {
		uint64_t test1 = T[1];
		test1 <<= 32;
		test1 += T[0];
		//print64Hex(	)
		uint64_t test2 = PQptr[1];
		test2 <<= 32;
		test2 += PQptr[0];
		T = subtract1024(T, PQptr);
		uint64_t test3 = T[1];
		test3 <<= 32;
		test3 += T[0];
		if(test3 != test1-test2) { printf("BAD SUBTRACT"); newline print64Hex(test1); newline print64Hex(test2); newline print64Hex(test3); newline }
		//else printf("GOOD SUBTRACT"); newline
	}
	else if(shouldbelarger) {
		printf("INCORRECT LARGENESS"); newline print64Hex(tes1); newline print64Hex(tes2); newline
	}
	//printf("\n");
	return T;
}

void printSmolBinary(uint32_t input[]) {
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
	//printf("\n");
}

uint32_t * copy1024(uint32_t * copyTo, uint32_t * copyFrom){
	int i;
	for(i = 0; i < 2; i++){
		copyTo[i] = copyFrom[i];
	}
	return copyTo;
}

void printHex(uint32_t * input) {
	int i = 1;
	for(; i >= 0; i--){
		printf("%08" PRIx32, input[i]);
	}
	fflush(stdout);
}

void print64Hex(uint64_t input) {
	int i = 0;
	for(; i >= 0; i--){
		printf("%016" PRIx64, input);
	}
	fflush(stdout);
}

int main(int argc, char *argv[]) {

	uint32_t ptn = 0x0058afff;
	//for(ptn = 0; ptn < 133; ptn++) {
	uint32_t * D = calloc(2, sizeof(uint32_t));
	D[0] = 0x3245a881;
	D[1] = 0x0053e5ff;

	uint32_t * PQ = calloc(2, sizeof(uint32_t));
	PQ[0] = 0xb241a8f5;
	PQ[1] = 0x00990617;

	uint32_t E = 0x10001;

	uint32_t * Rsq = calloc(2, sizeof(uint32_t));
	Rsq[0] = 0x1ca22284;
	Rsq[1] = 0x004d8c6b;
	
	uint32_t * R = calloc(2, sizeof(uint32_t));
	R[0] = 0x00000000;
	R[1] = 0x00800000; 

	uint32_t * testT = calloc(2, sizeof(uint32_t));
	testT[0] = ptn;//0x0A; //10
	//printf("TestT: ");
	//printSmolBinary(testT);
	//printf("\n");
	
	uint32_t * tToPowerOf2 = calloc(2, sizeof(uint32_t));

	int i = 0;
	//printHex(testT);
	//printf("\n\n\n");

	/*								 */
	/*            Encrypt            */
	/*								 */

	uint32_t * rollingTVal = R;
	//printf("mmm with 0x0A\n");
	//printf("Plaintext: ");
	//printHex(testT);
	//printf("\n");
	testT = bitwiseMMM(testT, Rsq, PQ); //testT is now   TR mod PQ

	if(E & 1) {
		printf("needed a power, 1\n");
		rollingTVal = bitwiseMMM(rollingTVal, testT, PQ); //get T^1 if we need it
	}
	//printHex(rollingTVal);
	//printf("\naaaaaaaaaaaaaaa\n");
	tToPowerOf2 = bitwiseMMM(testT, testT, PQ); //T^2R
	//free(testT);
	
	for(i = 1; i < 17; i++){ //17 is bit length of E
		if(E & (1 << i)) {
			printf("needed a power, %d\n", i+1);
			rollingTVal = bitwiseMMM(rollingTVal, tToPowerOf2, PQ);
		}
		tToPowerOf2 = bitwiseMMM(tToPowerOf2, tToPowerOf2, PQ); //get the next power of 2
	}

	//descale (T^n)*R
	uint32_t * one = calloc(2, sizeof(uint32_t));
	one[0] = 1;
	rollingTVal = bitwiseMMM(rollingTVal, one, PQ);

	//printf("Encrypted value: ");
	//printHex(rollingTVal);
	//printf("\n");


	printHex(rollingTVal); newline
	//testT = copy1024(testT, rollingTVal); //set the input to decrypt to be the encrypted text
	testT = rollingTVal;



	/*								 */
	/*            Decrypt            */
	/*								 */
	
	rollingTVal = R;
	testT = bitwiseMMM(testT, Rsq, PQ); //testT is now   TR mod PQ

	if(D[0] & 1) {
		rollingTVal = bitwiseMMM(rollingTVal, testT, PQ); //get T^1 if we need it
	}

	tToPowerOf2 = bitwiseMMM(testT, testT, PQ); //T^2R
	//decrypt

	for(i = 1; i < 56; i++){ // 56 is bit length of D
		if(D[i/32] & (1 << i%32)) {
			printf("needed a power, %d\n", i+1);
			rollingTVal = bitwiseMMM(rollingTVal, tToPowerOf2, PQ);
		}
		tToPowerOf2 = bitwiseMMM(tToPowerOf2, tToPowerOf2, PQ); //get the next power of 2
	}

	//descale (T^n)*R
	rollingTVal = bitwiseMMM(rollingTVal, one, PQ);
	//printf("This is the decrypted value: ");
	//printHex(rollingTVal); //should be plaintext now
	//printf("\n");
	if(rollingTVal[0] != ptn) {
		printf("BAD BOI: %u\n", ptn);
	}
//}*/
	return 0;
}	

