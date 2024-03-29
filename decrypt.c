
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#define LONG_MAX 0xFFFFFFFFUL
#define P_Q 0xC37

void printSmolBinary(uint32_t input[]);

//double check this works
uint32_t * rshift1024(uint32_t * input) {
	register int i;
	register uint32_t carryBit = 0, nextCarryBit = 0;
	for(i = 31; i >= 0; i--){
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
	for(i = 0; i < 32; i++){
		//if(carryFlag) printf("had a carry on index: %d\n", i);
		tmpa = a[i];
		a[i] += b[i];
		a[i] += carryFlag;
		carryFlag = 0;
		if(b[i] > (LONG_MAX - tmpa + 1)) {
			carryFlag = 1;
		}
	}
	return a;
}

uint32_t * add64(uint32_t * a, uint32_t * b) {
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

int alargerequalb1024(uint32_t * a, uint32_t * b){
	register int i;
	uint32_t abit;
	uint32_t bbit;
	for(i = 1023; i >= 0; i--){
		//temp = a[1-i]-b[1-i];
		abit = a[i/32] & (1 << i%32);
		bbit = b[i/32] & (1 << i%32);
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
	return T;
}

uint32_t * bitwiseMMM(uint32_t * x, uint32_t * y, uint32_t * PQptr) {
	int i;
	uint32_t * T = calloc(32, sizeof(uint32_t));
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
				T = add1024(T, PQptr);
				T = rshift1024(T);
			}
		}
		else {
			if(eta == 0){
				T = rshift1024(add1024(T, y));
			}
			else {
				uint32_t * tmp = calloc(32, sizeof(uint32_t));
				add1024(tmp, PQptr);
				add1024(tmp, y);
				add1024(T, tmp);
				//free(tmp);
				T = rshift1024(T);
			}
		}
	}
	if(alargerequalb1024(T, PQptr)) {
		T = subtract1024(T, PQptr);
	}
	return T;
}

void printBinary(uint32_t * input) {
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
		//printf("\n");
	}
	printf("\n");
}

void printSmolBinary(uint32_t * input) {
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

uint32_t * copy1024(uint32_t * copyTo, uint32_t * copyFrom){
	int i;
	for(i = 0; i < 32; i++){
		copyTo[i] = copyFrom[i];
	}
	return copyTo;
}

void printHex(uint32_t * input) {
	int i = 31;
	for(; i >= 0; i--){
		//printf(" %d - ", i);
		printf("%08" PRIx32, input[i]);
		//if(i % 4 == 0) printf("\n");
	}
	fflush(stdout);
}


int main(int argc, char *argv[]) {

	uint32_t * D = calloc(32, sizeof(uint32_t));
	D[31] = 0x8ded8575; D[30] = 0x9dbdf09b; D[29] = 0x64057c80; D[28] = 0x735b1b92; D[27] = 0x7d752009; D[26] = 0x868ad5eb; D[25] = 0x12de20fb; D[24] = 0x4114c074; D[23] = 0x06b445f5; D[22] = 0x8e798e9d; D[21] = 0x43201d7c; D[20] = 0x7f831a24; D[19] = 0x11a5a4ad; D[18] = 0x5c78f90a; D[17] = 0x465761c4; D[16] = 0xb25f6bed; D[15] = 0x2bbb262e; D[14] = 0x4f4ff63b; D[13] = 0x63525b84; D[12] = 0x193b2d2c; D[11] = 0x7f04e62c; D[10] = 0x51ed7974; D[9] = 0x5931e8a8; D[8] = 0x5bd1506a; D[7] = 0xad6d13ed; D[6] = 0xc283c9bc; D[5] = 0xed6c4855; D[4] = 0x4cc16a5c; D[3] = 0x89931419; D[2] = 0x26be70ae; D[1] = 0x2c77fcb0; D[0] = 0xfcf18641;
	
	uint32_t * PQ = calloc(32, sizeof(uint32_t));
	PQ[31] = 0xa5e12297; PQ[30] = 0xb6ad7eea; PQ[29] = 0x39d6a3d0; PQ[28] = 0xb3ac7774; PQ[27] = 0xd09e5a5e; PQ[26] = 0xd36f2b63; PQ[25] = 0x9c8431e5; PQ[24] = 0x9f15f770; PQ[23] = 0x59bd210d; PQ[22] = 0x7cf816ac; PQ[21] = 0xed476ed7; PQ[20] = 0x6b0e8431; PQ[19] = 0x1abbd00f; PQ[18] = 0x0d83dd5a; PQ[17] = 0xb62ab6df; PQ[16] = 0x319701a4; PQ[15] = 0xb6342916; PQ[14] = 0x0db4883c; PQ[13] = 0xcd05d67e; PQ[12] = 0x1f574332; PQ[11] = 0xc16b7f1d; PQ[10] = 0x850adc5a; PQ[9] = 0x628568a1; PQ[8] = 0x9bbce25d; PQ[7] = 0xd8da02d6; PQ[6] = 0x0898b653; PQ[5] = 0xf9517d4c; PQ[4] = 0x846faac4; PQ[3] = 0xa5c72d6d; PQ[2] = 0x46a645ac; PQ[1] = 0x026fe669; PQ[0] = 0x41fd6115;
	
	uint32_t E = 0x10001;

	uint32_t * Rsq = calloc(32, sizeof(uint32_t));
	Rsq[31] = 0x7e5b2782; Rsq[30] = 0xd72c520c; Rsq[29] = 0xffe99ea9; Rsq[28] = 0x3f2f778d; Rsq[27] = 0x1f3be0f6; Rsq[26] = 0x98eeb118; Rsq[25] = 0x15165f4c; Rsq[24] = 0x58a1be63; Rsq[23] = 0x55046a91; Rsq[22] = 0x0e2076b5; Rsq[21] = 0x4d79837a; Rsq[20] = 0x2e8fd0ef; Rsq[19] = 0xcc970252; Rsq[18] = 0xa7e12206; Rsq[17] = 0x8cbd6f33; Rsq[16] = 0xa8003707; Rsq[15] = 0x2b6478b9; Rsq[14] = 0xb47e9269; Rsq[13] = 0xba4c8883; Rsq[12] = 0x60312252; Rsq[11] = 0xd6bbd545; Rsq[10] = 0x4806ab3b; Rsq[9] = 0x4984530c; Rsq[8] = 0x849b539f; Rsq[7] = 0xbadc8dff; Rsq[6] = 0xe63b0528; Rsq[5] = 0x20a7b19d; Rsq[4] = 0x151204a3; Rsq[3] = 0x404f70ef; Rsq[2] = 0xcfe802ac; Rsq[1] = 0xd7419d1a; Rsq[0] = 0xc98deb18;
	
	uint32_t * R = calloc(32, sizeof(uint32_t));
	R[31] = 0x5a1edd68; R[30] = 0x49528115; R[29] = 0xc6295c2f; R[28] = 0x4c53888b; R[27] = 0x2f61a5a1; R[26] = 0x2c90d49c; R[25] = 0x637bce1a; R[24] = 0x60ea088f; R[23] = 0xa642def2; R[22] = 0x8307e953; R[21] = 0x12b89128; R[20] = 0x94f17bce; R[19] = 0xe5442ff0; R[18] = 0xf27c22a5; R[17] = 0x49d54920; R[16] = 0xce68fe5b; R[15] = 0x49cbd6e9; R[14] = 0xf24b77c3; R[13] = 0x32fa2981; R[12] = 0xe0a8bccd; R[11] = 0x3e9480e2; R[10] = 0x7af523a5; R[9] = 0x9d7a975e; R[8] = 0x64431da2; R[7] = 0x2725fd29; R[6] = 0xf76749ac; R[5] = 0x06ae82b3; R[4] = 0x7b90553b; R[3] = 0x5a38d292; R[2] = 0xb959ba53; R[1] = 0xfd901996; R[0] = 0xbe029eeb;

	uint32_t * testT = calloc(32, sizeof(uint32_t));
	testT[2] = 0x41414141; //ascii AAAA
	//testT[1] = 0x765432;
	//testT[2] = 0x123456;
	//testT[3] = 0xabcdef;

	uint32_t * tToPowerOf2 = calloc(32, sizeof(uint32_t));
	tToPowerOf2[2] = testT[2];
	//tToPowerOf2[1] = testT[1];
	//tToPowerOf2[2] = testT[2];
	//tToPowerOf2[3] = testT[3];
	int i = 0;
	printHex(testT);
	printf("\n\n\n");

	/*								 */
	/*            Encrypt            */
	/*								 */

	uint32_t * rollingTVal = R;
	uint32_t * tmp1; //used for freeing
	testT = bitwiseMMM(testT, Rsq, PQ); //testT is now   TR mod PQ

	if(E & 1) {
		//printf("Needed a power: 1\n");
		rollingTVal = bitwiseMMM(rollingTVal, testT, PQ); //get T^1 if we need it
		//free(tmp1); want this eventually since tmp1 is R but we need it for decrypting in the same c file
		tmp1 = rollingTVal;
	}

	tToPowerOf2 = bitwiseMMM(testT, testT, PQ); //T^2R
	//free(testT);
	

	uint32_t * tmp2 = tToPowerOf2; //used to free the old tToPowerOf2 chunk
	for(i = 1; i < 17; i++){ //17 is bit length of E
		if(E & (1 << i%32)) {
			//printf("Needed a power: %d\n", i);
			rollingTVal = bitwiseMMM(rollingTVal, tToPowerOf2, PQ);
			//free(tmp1);
			tmp1 = rollingTVal;
		}
		tToPowerOf2 = bitwiseMMM(tToPowerOf2, tToPowerOf2, PQ); //get the next power of 2
		//free(tmp2);
		tmp2 = tToPowerOf2;
	}

	//descale (T^n)*R
	uint32_t * one = calloc(32, sizeof(uint32_t));
	one[0] = 1;
	rollingTVal = bitwiseMMM(rollingTVal, one, PQ);

	printf("\n\n\n");
	printHex(rollingTVal);
	printf("\n\n\n");


	//testT = copy1024(testT, rollingTVal); //set the input to decrypt to be the encrypted text
	testT = rollingTVal;

	/*								 */
	/*            Decrypt            */
	/*								 */
	
	rollingTVal = R;
	testT = bitwiseMMM(testT, Rsq, PQ); //testT is now   TR mod PQ

	if(D[0] & 1) {
		//printf("Needed a power: 1\n");
		rollingTVal = bitwiseMMM(rollingTVal, testT, PQ); //get T^1 if we need it
		//free(tmp1);
		tmp1 = rollingTVal;
	}

	tToPowerOf2 = bitwiseMMM(testT, testT, PQ); //T^2R
	//free(testT);
	//decrypt

	tmp2 = tToPowerOf2; //used to free the old tToPowerOf2 chunk
	for(i = 1; i < 1024; i++){ //1024 is length of D
		if(D[i/32] & (1 << i%32)) {
			//printf("Needed a power: %d\n", i);
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
	printf("\n\n\n");
	printHex(rollingTVal); //should be plaintext now
	printf("\n\n\n");
	return 0;

//why the heck is it printing 3 numbers of different lengths???

}	

//P: 00f36475b06954f3ef0dece9a6af0fa3379bc819df00c97595afe2cc2c5749b33f
//Q: 00e011e2301af8ceec4f3cb193d63cafa80e61015e9dd0d41c365ce7d68b0fa2ad

