#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void printHex(long* in, int blocks);
void roundF(unsigned long* in, unsigned long w, unsigned long k);
unsigned long rotr(unsigned long in, int rot);
void compress(unsigned long* m, unsigned long* hash);
void reverser(unsigned long* in);

int main(int argc, char** argv){
	FILE* in = fopen(argv[1],"rb");
	fseek(in,0,SEEK_END);
	long size=ftell(in);
	unsigned long message[16];
	rewind(in);
	unsigned long hash[8]={0x6A09E667F3BCC908,0xBB67AE8584CAA73B,0x3C6EF372FE94F82B,0xA54FF53A5F1D36F1,0x510E527FADE682D1,0x9B05688C2B3E6C1F,0x1F83D9ABFB41BD6B,0x5BE0CD19137E2179};
	for(int i=0;i<16;i++){
			message[i]=0x0000000000000000;
	}
	int length=fread(message,1,128,in);
	while(length==128){
		reverser(message);
		compress(message,hash);
		for(int i=0;i<16;i++){
			message[i]=0x0000000000000000;
		}
		length=fread(message,1,128,in);
	}

	//begin padding
	reverser(message);
	int pos=length/8;
	int bitpos=(length%8)*8+1;
	long temp=0x0000000000000001;
	message[pos]^=(temp<<(64-bitpos));
	// printf("%d\n",pos);
	// printf("%d\n",bitpos);
	for(int i=pos+1;i<16;i++){
		message[i]=0x0000000000000000;
	}
	//add and hash another block if necessary. Realistically, all of the first 64 bits of L should be zero, provided your file is less than multiple exabytes long.
	if(length>=112){
		compress(message,hash);
		for(int i=0;i<16;i++){
			message[i]=0x0000000000000000;
		}
	}
	message[15]=size*8;
	// printHex(message,16);
	compress(message,hash);
	printHex(hash,8);
}

void reverser(unsigned long* in){
	char* boofer=(char*) in;
	for(int j=0;j<16;j++){
		char* boofer=(char*) in+8*j;
		for(int i=0;i<4;i++){
			char temp=boofer[i];
			boofer[i]=boofer[7-i];
			boofer[7-i]=temp;
		}
	}
}

void compress(unsigned long* m, unsigned long* hash){
	unsigned long keys[80]={0x428A2F98D728AE22,0x7137449123EF65CD,0xB5C0FBCFEC4D3B2F,0xE9B5DBA58189DBBC,0x3956C25BF348B538,0x59F111F1B605D019,0x923F82A4AF194F9B,0xAB1C5ED5DA6D8118,0xD807AA98A3030242,0x12835B0145706FBE,0x243185BE4EE4B28C,0x550C7DC3D5FFB4E2,0x72BE5D74F27B896F,0x80DEB1FE3B1696B1,0x9BDC06A725C71235,0xC19BF174CF692694,0xE49B69C19EF14AD2,0xEFBE4786384F25E3,0x0FC19DC68B8CD5B5,0x240CA1CC77AC9C65,0x2DE92C6F592B0275,0x4A7484AA6EA6E483,0x5CB0A9DCBD41FBD4,0x76F988DA831153B5,0x983E5152EE66DFAB,0xA831C66D2DB43210,0xB00327C898FB213F,0xBF597FC7BEEF0EE4,0xC6E00BF33DA88FC2,0xD5A79147930AA725,0x06CA6351E003826F,0x142929670A0E6E70,0x27B70A8546D22FFC,0x2E1B21385C26C926,0x4D2C6DFC5AC42AED,0x53380D139D95B3DF,0x650A73548BAF63DE,0x766A0ABB3C77B2A8,0x81C2C92E47EDAEE6,0x92722C851482353B,0xA2BFE8A14CF10364,0xA81A664BBC423001,0xC24B8B70D0F89791,0xC76C51A30654BE30,0xD192E819D6EF5218,0xD69906245565A910,0xF40E35855771202A,0x106AA07032BBD1B8,0x19A4C116B8D2D0C8,0x1E376C085141AB53,0x2748774CDF8EEB99,0x34B0BCB5E19B48A8,0x391C0CB3C5C95A63,0x4ED8AA4AE3418ACB,0x5B9CCA4F7763E373,0x682E6FF3D6B2B8A3,0x748F82EE5DEFB2FC,0x78A5636F43172F60,0x84C87814A1F0AB72,0x8CC702081A6439EC,0x90BEFFFA23631E28,0xA4506CEBDE82BDE9,0xBEF9A3F7B2C67915,0xC67178F2E372532B,0xCA273ECEEA26619C,0xD186B8C721C0C207,0xEADA7DD6CDE0EB1E,0xF57D4F7FEE6ED178,0x06F067AA72176FBA,0x0A637DC5A2C898A6,0x113F9804BEF90DAE,0x1B710B35131C471B,0x28DB77F523047D84,0x32CAAB7B40C72493,0x3C9EBE0A15C9BEBC,0x431D67C49C100D4C,0x4CC5D4BECB3E42B6,0x597F299CFC657E2A,0x5FCB6FAB3AD6FAEC,0x6C44198C4A475817};
	unsigned long hashCopy[8];
	for(int i=0;i<8;i++){
		hashCopy[i]=hash[i];
	}
	for(int i=0;i<16;i++){
		roundF(hash,m[i],keys[i]);
	}
	//printHex(hash,8);
	unsigned long test=rotr(hash[0],1);
	//printHex(&test,1);
	for(int i=16;i<80;i++){
		unsigned long sig0=rotr(m[(i+1)%16],1)^rotr(m[(i+1)%16],8)^(m[(i+1)%16]>>7);
		unsigned long sig1=rotr(m[(i-2)%16],19)^rotr(m[(i-2)%16],61)^(m[(i-2)%16]>>6);
		m[i%16]=m[i%16]+sig0+sig1+m[(i-7)%16];
		roundF(hash,m[i%16],keys[i]);
	}
	//printHex(hash,8);
	for(int i=0;i<8;i++){
		hash[i]+=hashCopy[i];
	}
}

void roundF(unsigned long* in, unsigned long w, unsigned long k){
	unsigned long maj=(in[0]&in[1])^(in[0]&in[2])^(in[1]&in[2]);
	unsigned long ch=(in[4]&in[5])^((~in[4])&in[6]);
	unsigned long suma=rotr(in[0],28)^rotr(in[0],34)^rotr(in[0],39);
	unsigned long sume=rotr(in[4],14)^rotr(in[4],18)^rotr(in[4],41);
	unsigned long temp=in[7]+ch;
	temp+=sume;
	temp+=w;
	temp+=k;
	in[7]=in[6];
	in[6]=in[5];
	in[5]=in[4];
	in[4]=in[3]+temp;
	in[3]=in[2];
	in[2]=in[1];
	in[1]=in[0];
	in[0]=suma+maj+temp;
}

unsigned long rotr(unsigned long in, int rot){
	return (in>>rot)|(in<<(64-rot));
}
void printHex(long* in,int blocks){
	for(int i=0;i<blocks;i++){
		printf("%016lx",(in[i]));
	}
	printf("\n");
}