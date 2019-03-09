#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

const int N = 256;
const int bufsize = 512; 

bool b = false, t = false, c = false, ignore1 = false, quiet = false, 
	status  = false, strict = false, warn = false, help = false;
int w1=0, w2=0, w3=0; bool wascorrect=false;

uint32_t Pi[256] = {
	252, 238, 221, 17, 207, 110, 49, 22, 251,196, 250, 218, 35, 197, 4, 77, 233, 119, 240, 219, 147, 46,
	153, 186, 23, 54, 241, 187, 20, 205, 95, 193, 249, 24, 101, 90, 226, 92, 239, 33, 129, 28, 60, 66, 139, 1, 142,
	79, 5, 132, 2, 174, 227, 106, 143, 160, 6, 11, 237, 152, 127, 212, 211, 31, 235, 52, 44, 81, 234, 200, 72, 171,
	242, 42, 104, 162, 253, 58, 206, 204, 181, 112, 14, 86, 8, 12, 118, 18, 191, 114, 19, 71, 156, 183, 93, 135, 21,
	161, 150, 41, 16, 123, 154, 199, 243, 145, 120, 111, 157, 158, 178, 177, 50, 117, 25, 61, 255, 53, 138, 126,
	109, 84, 198, 128, 195, 189, 13, 87, 223, 245, 36, 169, 62, 168, 67, 201, 215, 121,214, 246, 124, 34, 185, 3,
	224, 15, 236, 222, 122, 148, 176, 188, 220, 232, 40, 80, 78, 51, 10, 74, 167, 151, 96, 115, 30, 0, 98, 68, 26,
	184, 56, 130, 100, 159, 38, 65, 173, 69, 70, 146, 39, 94, 85, 47, 140, 163, 165, 125, 105, 213, 149, 59, 7, 88,
	179, 64, 134, 172, 29, 247, 48, 55, 107, 228, 136, 217, 231, 137, 225, 27, 131, 73, 76, 63, 248, 254, 141, 83,
	170, 144, 202, 216, 133, 97, 32, 113, 103, 164, 45, 43, 9, 91, 203, 155, 37, 208, 190, 229, 108, 82, 89, 166,
	116, 210, 230, 244, 180, 192, 209, 102, 175, 194, 57, 75, 99, 182
};

uint32_t R[64] = {
	0, 8, 16, 24, 32, 40, 48, 56, 1, 9, 17, 25, 33, 41, 49, 57, 2, 10, 18, 26, 34, 42, 50, 58, 3, 11, 19, 27, 
	35, 43, 51, 59, 4, 12, 20, 28, 36, 44, 52, 60, 5, 13, 21, 29, 37, 45, 53, 61, 6, 14, 22, 30, 38, 46, 54, 62, 7, 15, 
	23, 31, 39, 47, 55, 63
};

uint64_t M[64] = {
	0x8e20faa72ba0b470, 0x47107ddd9b505a38, 0xad08b0e0c3282d1c, 0xd8045870ef14980e,
	0x6c022c38f90a4c07, 0x3601161cf205268d, 0x1b8e0b0e798c13c8, 0x83478b07b2468764,
	0xa011d380818e8f40, 0x5086e740ce47c920, 0x2843fd2067adea10, 0x14aff010bdd87508,
	0x0ad97808d06cb404, 0x05e23c0468365a02, 0x8c711e02341b2d01, 0x46b60f011a83988e,
	0x90dab52a387ae76f, 0x486dd4151c3dfdb9, 0x24b86a840e90f0d2, 0x125c354207487869,
	0x092e94218d243cba, 0x8a174a9ec8121e5d, 0x4585254f64090fa0, 0xaccc9ca9328a8950,
	0x9d4df05d5f661451, 0xc0a878a0a1330aa6, 0x60543c50de970553, 0x302a1e286fc58ca7,
	0x18150f14b9ec46dd, 0x0c84890ad27623e0, 0x0642ca05693b9f70, 0x0321658cba93c138,
	0x86275df09ce8aaa8, 0x439da0784e745554, 0xafc0503c273aa42a, 0xd960281e9d1d5215,
	0xe230140fc0802984, 0x71180a8960409a42, 0xb60c05ca30204d21, 0x5b068c651810a89e,
	0x456c34887a3805b9, 0xac361a443d1c8cd2, 0x561b0d22900e4669, 0x2b838811480723ba,
	0x9bcf4486248d9f5d, 0xc3e9224312c8c1a0, 0xeffa11af0964ee50, 0xf97d86d98a327728,
	0xe4fa2054a80b329c, 0x727d102a548b194e, 0x39b008152acb8227, 0x9258048415eb419d,
	0x492c024284fbaec0, 0xaa16012142f35760, 0x550b8e9e21f7a530, 0xa48b474f9ef5dc18,
	0x70a6a56e2440598e, 0x3853dc371220a247, 0x1ca76e95091051ad, 0x0edd37c48a08a6d8,
	0x07e095624504536c, 0x8d70c431ac02a736, 0xc83862965601dd1b, 0x641c314b2b8ee083
};

class uint512_t {         
	uint64_t elem[8]; ////////// в 0 - старшие, в 7 - младшие (прямой порядок)
public:
	uint512_t(){   //init 
		for (int i=0; i<8; i++)
			elem[i]=0;
	}
	uint512_t(uint64_t * arr){  ////////// в 0 - старшие, в 7 - младшие (прямой порядок)
		for (int i=0; i<8; i++)
			elem[i]=arr[i];
	}
	uint512_t(uint8_t arr[]){  // в 0 - младший, в 63 - старший
		uint64_t x;
		for(int i=0; i<8; i++) {
			x=0;
			for(int j=0; j<8; j++) {
				x = x << 8;
				x = x + arr[63-8*i-j];
			}
			elem[i]=x;
		}
	}
	uint512_t(uint64_t a) {
		for (int i=0; i<7; i++)
			elem[i]=0;
		elem[7]=a;
	}
	void setzero(){
		for(int i=4; i<8; i++)
			elem[i]=0;
	}
	int operator == (const uint512_t z) {
		for(int i=0; i<8; i++)
			if (elem[i]!=z.elem[i]) 
				return 0;
		return 1;
	}
 	uint512_t operator ^ (const uint512_t z) {
		uint64_t arr[8];
		for (int i=0; i<8; i++)
			arr[i]=elem[i]^z.elem[i];
		return uint512_t(arr);	
	}
	uint512_t operator + (const uint512_t z) {
		uint64_t arr[8];
		bool f=false;  //carry flag
		for (int i=7; i>=0; i--) {
			if (f) {
				if (elem[i]<=0xFFFFFFFFFFFFFFFF-z.elem[i]-1)  // overflow
					f = false; 
				arr[i]=elem[i]+z.elem[i]+1;
			}
			else {
				if (elem[i]>0xFFFFFFFFFFFFFFFF-z.elem[i]) 
					f = true;
				arr[i]=elem[i]+z.elem[i];
			}
		}
		return uint512_t(arr);	
	}
	uint8_t get8bits(int i, bool b = false) { // 0 - младший байт elem[7][0], 63 - старший elem[0][7]
		uint64_t x = 0xFF;
		uint64_t j=7-i/8;
		i=i%8;
		uint64_t k=i;
		while (i>0){
			x = x<< 8; 
			i--;
		}
		x = elem[j]&x;
		while (k>0) {
			x = x >> 8;
			k--;
		}
		return (uint8_t)x;
	};
	void makebytes(uint8_t arr[]) {  
		for (int i = 0; i<64; i++){
			arr[i]=this->get8bits(i);
		}
	}
	void getanswer(uint8_t arr[], bool mode256){
		if ( ! mode256) {
			for (int i = 0; i<64; i++)
			arr[i]=this->get8bits(63-i);
		}
		else {
			for (int i = 0; i<32; i++)
			arr[i]=this->get8bits(63-i);
		}
	} 
	void makearr(uint64_t arr[]) {
		for (int i=0; i<8; i++)
			arr[i]=elem[i];
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint64_t C1[8] = {0xb1085bda1ecadae9, 0xebcb2f81c0657c1f,
0x2f6a76432e45d016, 0x714eb88d7585c4fc,
0x4b7ce09192676901, 0xa2422a08a460d315,
0x05767436cc744d23, 0xdd806559f2a64507};

uint64_t C2[8] = {0x6fa3b58aa99d2f1a, 0x4fe39d460f70b5d7,
0xf3feea720a232b98, 0x61d55e0f16b50131,
0x9ab5176b12d69958, 0x5cb561c2db0aa7ca,
0x55dda21bd7cbcd56, 0xe679047021b19bb7};

uint64_t C3[8] = {0xf574dcac2bce2fc7, 0x0a39fc286a3d8435,
0x06f15e5f529c1f8b, 0xf2ea7514b1297b7b,
0xd3e20fe490359eb1, 0xc1c93a376062db09,
0xc2b6f443867adb31, 0x991e96f50aba0ab2};

uint64_t C4[8] = {0xef1fdfb3e81566d2, 0xf948e1a05d71e4dd,
0x488e857e335c3c7d, 0x9d721cad685e353f,
0xa9d72c82ed03d675, 0xd8b71333935203be,
0x3453eaa193e837f1, 0x220cbebc84e3d12e};

uint64_t C5[8] = {0x4bea6bacad474799, 0x9a3f410c6ca92363,
0x7f151c1f1686104a, 0x359e35d7800fffbd,
0xbfcd1747253af5a3, 0xdfff00b723271a16,
0x7a56a27ea9ea63f5, 0x601758fd7c6cfe57};

uint64_t C6[8] = {0xae4faeae1d3ad3d9, 0x6fa4c33b7a3039c0,
0x2d66c4f95142a46c, 0x187f9ab49af08ec6,
0xcffaa6b71c9ab7b4, 0x0af21f66c2bec6b6,
0xbf71c57236904f35, 0xfa68407a46647d6e};

uint64_t C7[8] = {0xf4c70e16eeaac5ec, 0x51ac86febf240954,
0x399ec6c7e6bf87c9, 0xd3473e33197a93c9,
0x0992abc52d822c37, 0x06476983284a0504,
0x3517454ca23c4af3, 0x8886564d3a14d493};
					
uint64_t C8[8] = {0x9b1f5b424d93c9a7, 0x03e7aa020c6e4141,
0x4eb7f8719c36de1e, 0x89b4443b4ddbc49a,
0xf4892bcb929b0690, 0x69d18d2bd1a5c42f,
0x36acc2355951a8d9, 0xa47f0dd4bf02e71e};

uint64_t C9[8] = {0x378f5a541631229b, 0x944c9ad8ec165fde,
0x3a7d3a1b25894224, 0x3cd955b7e00d0984,
0x800a440bdbb2ceb1, 0x7b2b8a9aa6079c54,
0x0e38dc92cb1f2a60, 0x7261445183235adb};

uint64_t C10[8] = {0xabbedea680056f52, 0x382ae548b2e4f3f3,
0x8941e71cff8a78db, 0x1fffe18a1b336103,
0x9fe76702af69334b, 0x7a1e6c303b7652f4,
0x3698fad1153bb6c3, 0x74b4c7fb98459ced};
						
uint64_t C11[8] = {0x7bcd9ed0efc889fb, 0x3002c6cd635afe94,
0xd8fa6bbbebab0761, 0x2001802114846679,
0x8a1d71efea48b9ca, 0xefbacd1d7d476e98,
0xdea2594ac06fd85d, 0x6bcaa4cd81f32d1b};

uint64_t C12[8] = {0x378ee767f11631ba, 0xd21380b00449b17a,
0xcda43c32bcdf1d77, 0xf82012d430219f9b,
0x5d80ef9d1891cc86, 0xe71da4aa88e12852,
0xfaf417d5d9b21b99, 0x48bc924af11bd720};


uint512_t C[13] = {C1, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12}; // Для индексирования с 1

void paddingstr(uint8_t * s, uint8_t * s1, int size){  //3 Этап, с заполнением 0|0|...|0|1|s
	int len=64-size, i=0, j=0;
	for (i=0; i<len-1; i++){
		s1[i]=0;
	}
	s1[i]=(uint8_t)0x01;
	i++;
	for (i=i; i<65; i++){
		s1[i]=s[j];
		j++;
	}
	for (int i = 0; i<64; i++)
		s[i]=s1[i];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint512_t X(uint512_t K, uint512_t a) {
	return K^a;
}

uint512_t S(uint512_t a) {
	uint512_t stor; //=0;
	uint64_t x[8];
	for(int j=0; j<8; j++) {
		x[j]=0;
		for (int i=63-8*j; i>63-8*j-8; i--) {
			x[j] = x[j] << 8;
			x[j] = x[j] + Pi[a.get8bits(i)];
		}
	}
	stor=uint512_t(x);
	return (stor);
}

uint64_t l(uint64_t a){
	uint64_t stor=0;
	for (int i=0; i<64; i++){
		if (a&0x1)
			stor = stor ^ M[63-i];
		a = a >> 1;
	}
	return stor;
}

uint512_t P(uint512_t a) {  
	uint8_t arr[64], arr2[64];
	a.makebytes(arr2);   
	for(int i=0; i<64; i++)
		arr[i]=arr2[R[i]];
	return uint512_t(arr);
}

uint512_t L(uint512_t a){
	uint64_t arr[8];
	a.makearr(arr);
	for (int i=0; i<8; i++) 
		arr[i]=l(arr[i]);
	return uint512_t(arr);
}

uint512_t E(uint512_t K, uint512_t m, int i=1){
	if (i==13)
		return X(K, m);
	else
		return E(L(P(S(K^C[i]))), L(P(S(X(K, m)))), i+1);
}

uint512_t g(uint512_t N, uint512_t h, uint512_t m) {
	return (E(L(P(S(h^N))), m) ^ h ^ m);
}

int readf(char *data, FILE * f) {  
	return fread(data, 1, bufsize, f);
}

class context {
public:
	uint512_t h, m, Sigma, N;
	uint8_t s[2*bufsize];
	size_t ssize;
	bool mode256;
};

void init(context * ctx, bool mode256) {
	uint64_t IV[8] = {0x0101010101010101, 0x0101010101010101, 0x0101010101010101, 0x0101010101010101,
					  0x0101010101010101, 0x0101010101010101, 0x0101010101010101, 0x0101010101010101};
	if (mode256)
		ctx->h = uint512_t(IV);
	else
		ctx->h = uint512_t();
	ctx->N = (uint64_t)0;
	ctx->Sigma = (uint64_t)0;
	ctx->ssize = (uint64_t)0;
	ctx->mode256 = mode256;
}

void mystrcat(uint8_t s[2*bufsize], size_t &ssize, const uint8_t data[bufsize], size_t dsize) {
	for (size_t i=0; i<dsize; i++)
		s[ssize+i]=data[i];
	ssize = ssize + dsize;
}

void mystrcpy(uint8_t *s1, uint8_t *s2) {
	for (int i=0; i<64; i++)
		s1[i]=s2[i];
}

void update(context * ctx, uint8_t * data, size_t size) { 
	while (size > 0) {
		if (size >= bufsize) {	
			mystrcat(ctx->s, ctx->ssize, data, bufsize);
			size = size - bufsize;
			data = data + bufsize;
		}
		else {
			mystrcat(ctx->s, ctx->ssize, data, size);
			size = 0;
		}
		int steps = ctx->ssize/64, i;
		for (i = 0; i < steps; i++) {   // обрабатываем то, что можем (что > 512 бит)
			ctx->m = uint512_t((uint8_t *)(ctx->s+64*i));
			ctx->h = g(ctx->N, ctx->h, ctx->m);
			ctx->N=ctx->N+512;
			ctx->Sigma = ctx->Sigma + ctx->m;
		}									
		uint8_t k[64];                  // 		сохраняем в контексте 								
		mystrcpy(k, ctx->s+64*i);		//		обрывок строки,	
		ctx->ssize = ctx->ssize % 64;	//		который недостаточно
		mystrcpy(ctx->s, k);			//		длинный для обработки       
	}   
}

void finish(context * ctx, uint8_t * hash) {
	uint8_t s1[65] ,s2[64], swap[bufsize];						
	for (size_t i=0; i<ctx->ssize; i++) 
    	swap[i] = ctx->s[ctx->ssize-i-1];
    for (size_t i=0; i<ctx->ssize; i++) {
    	ctx->s[i] = swap[i];
    }
	paddingstr(ctx->s, s1, ctx->ssize); 		
	for (int i=0; i<64; i++) {
		s2[i]=ctx->s[63-i];
	}
	for (int i=0; i<64; i++) {
		ctx->s[i]=s2[i];
	}
	ctx->m=uint512_t(ctx->s);
	ctx->h = g(ctx->N, ctx->h, ctx->m);;
	ctx->N=ctx->N+uint64_t(ctx->ssize*8);
	ctx->Sigma = ctx->Sigma + ctx->m;
	ctx->h = g((uint64_t)0, ctx->h, ctx->N);
	ctx->h = g((uint64_t)0, ctx->h, ctx->Sigma); 
	if (ctx->mode256)
		ctx->h.setzero();
	ctx->h.getanswer(hash, ctx->mode256);
}

int main(int argc, char * argv[]) {  //argv[1] == filename
	FILE * f;
	int size = 0;
	uint8_t buff[bufsize];
	#if defined(g256)
		uint8_t ans[256/8];
	#else
		uint8_t ans[512/8];
	#endif
	if (!(f=fopen(argv[1], "rb"))) {
        	cerr << "Open error" << endl;
        	exit(1);
    	};
    context ctx;


    #if defined(g256) //проверка правильности значения, не войдет в итоговую программу
    	init(&ctx, true);
    #else
    	init(&ctx, false);
    #endif


    do {
    	size = readf((char *)buff, f);
    	update(&ctx, buff, size);
    } while(size == bufsize);
    finish(&ctx, ans);


  #if defined(test)      //проверка правильности значения, не войдет в итоговую программу
    #if defined(g256)
    	for (int i=0; i<32; i++)
    #else
    	for (int i=0; i<64; i++)
    #endif
    		cout << std::hex << std::setw(2) << std::setfill('0') << (int)ans[i];
    cout << endl;
  #endif

    return 0;
}