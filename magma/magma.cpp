#include <cstring>
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

uint32_t P[8][16] = {
    {12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1},
    {6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15},
    {11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0},
    {12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11},
    {7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12},
    {5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0},
    {8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7},
    {1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2},
};

void printman(){
    cout << "magma [-h|--help]" << endl;
    cout << "magma [--ecb|--ctr|--ofb|--cbc|--cfb] {-e|-d} -k <key file> [options]" << endl;
    cout << "magma --mac -k <key file> [options]" << endl;
    cout << "• -h | --help – вывести текущее описание в stdout." << endl;
    cout << "• режимы работы, по умолчанию используется режим ECB:" << endl;
    cout << "  --ecb – ГОСТ Р 34.13-2015, пункт 5.1" << endl;
    cout << "  --ctr – ГОСТ Р 34.13-2015, пункт 5.2" << endl;
    cout << "  --ofb – ГОСТ Р 34.13-2015, пункт 5.3" << endl;
    cout << "  --cbc – ГОСТ Р 34.13-2015, пункт 5.4" << endl;
    cout << "  --cfb – ГОСТ Р 34.13-2015, пункт 5.5" << endl;
    cout << "  --mac – ГОСТ Р 34.13-2015, пункт 5.6" << endl;
    cout << "• -e – произвести зашифрование." << endl;
    cout << "• -d – произвести расшифрование." << endl;
    cout << "• -k <key file> – файл с бинарным ключом." << endl;
    cout << "• [options]:" << endl;
    cout << "  -v <iv file> – файл с бинарным значением IV. Используется с режимами CTR (32 бита), OFB, CBC, CFB (64z бита, z ∈ N)." << endl; 
    cout << "     По умолчанию IV=0 минимально допустимой длины;" << endl;
    cout << "  -i <input file> – входной файл. По умолчанию читать с stdin до EOF;" << endl;
    cout << "  -o <output file> – выходной файл. По умолчанию выводить в stdout." << endl;
}

uint32_t t (uint32_t a) {
    uint32_t a7=a>>28, a6 = (a >> 24)&15, a5 = (a>>20)&15, a4 = (a>>16)&15, a3=(a>>12)&15, a2=(a>>8)&15, a1=(a>>4)&15, a0=a&15, b=0;
    uint32_t ao[8] = {a0, a1, a2, a3, a4, a5, a6, a7};
    int i;
    for (i=7; i>=0; i--) {
        b = b << 4;
        b = b + P[i][ao[i]];
    }
    return b;
}

uint32_t g(uint32_t k, uint32_t a) {
    uint32_t b = t(a+k);
    return ((b << 11) | (b >> (32-11)));
}

struct v32 {
    uint32_t a1;
    uint32_t a0;
    v32(uint32_t x, uint32_t y) {
        a1 = x;
        a0 = y;
    }
    v32() {
        a1=a0=0;
    }
};

v32 G(uint32_t k, v32 a) {
    return v32(a.a0, g(k, a.a0)^a.a1);
}

uint64_t G1(uint32_t k, v32 a) {
    uint64_t x = 0;
    x = g(k, a.a0)^a.a1;
    x = x << 32;
    x = x | a.a0;
    return x;
}


void deploy (uint32_t KEY[32]) {
    int i = 0;
    for (i = 0; i<8; i++)
        KEY[i+8]=KEY[i+16]=KEY[i];
    for (int i = 0; i<8; i++)
        KEY[i+24]=KEY[7-i];
}

v32 GE(uint32_t KEY[32], int i, v32 a) {
    if (i==0)
        return G(KEY[i], a);
    else
        return G(KEY[i], GE(KEY, i-1, a));
}

uint64_t E(uint32_t KEY[32], v32 a){
    return G1(KEY[31], GE(KEY, 30, a));
}


v32 GD(uint32_t KEY[32], int i, v32 a) {
    if (i==31)
        return G(KEY[i], a);
    else
        return G(KEY[i], GD(KEY, i+1, a));
}

uint64_t D(uint32_t KEY[32], v32 a){
    return G1(KEY[0], GD(KEY, 1, a));
}

uint64_t charint(char * c, int size = 8) {
    uint64_t x=0;
    for (int k=0; k<size; k++) {
        x = (x << 8)&0xFFFFFFFFFFFFFF00;
        x = x | ((int)c[k] & 0xff);
    } 
    return x;
}

void intchar(char * c,uint64_t x){
    for (int k=0; k<8; k++) {
        c[7-k] = x & 0xff;
        x = x >> 8;
    } 
}

uint32_t charint32(char * c, int size = 4) {
    uint32_t x=0;
    for (int k=0; k<size; k++) {
        x = x << 8;
        x = x + ((int)c[k] & 0xff);
    } 
    return x;
}

void intchar32(char * c,uint32_t x){
    for (int k=0; k<4; k++) {
        c[3-k] = x & 0xff;
        x = x >> 8;
    } 
}

int filesize64(char * name) {
    int size=0;
    fstream file;
    file.open(name);
    if (!file.is_open()) {
        cerr << "Can`t open file " << name << endl;
        printman();
        exit(1);
    }
    file.seekg(0, std::ios::end);  
    size = file.tellg();
    file.close();
    return size/8;
}

int addition(char * name) {
    char c[8];
    int size, size1;
    fstream file;
    file.open(name);
    file.seekg(0, std::ios::end);  
    size = file.tellg();
    file.close();
    size1 = size%8;
    ofstream f(name, std::ios::binary|std::ios::app);
    c[0]=0x80; //1000 0000
    for (int j=1; j<8-size1; j++) 
        c[j]=0;
    f.write(c, 8-size1);
    f.close();
    return (size+8-size1)/8;
}

void readkey(uint32_t KEY[32], char * name) { 
    char c[4];
    uint32_t a;
    ifstream keyd;
    keyd.open(name, ios::binary);
    if (!keyd.is_open()) {
        cerr << "Can`t open " << name << endl;
        printman();
        exit(3);
    };
    for (int i = 0; i < 8; i ++) {
        keyd.read(c, 4);
        a=charint32(c);
        KEY[i]=a;
    }
};

int32_t readiv(char * name) {
    char c[4];
    uint32_t iv;
    ifstream ivd;
    if (name == NULL)
        return 0;
    ivd.open(name, ios::binary);
    if (!ivd.is_open()) {
        cerr << "Can`t open " << name << endl;
        printman();
        exit(3);
    };
    ivd.read(c, 4);
    iv=charint32(c);
    return iv;
}

int checkprint64(char * c) {
    int j=7;
    while (c[j]==0) {
        j--;
    }
    if (c[j]==(char)0x80) {
        return j; //to print
    }
    return 
        -1; 
}

void ecb(bool enc, uint32_t KEY[32], int size){ 
    uint64_t x; 
    char c[8];
    v32 a;
    int i=0;
    deploy(KEY);
    while (i < size) {
        x = fread(c, 1, 8, stdin);
        x = charint(c);
        a = v32((uint32_t)((x&0xFFFFFFFF00000000)>>32), (uint32_t)x&0xFFFFFFFF);
        if (enc) {
            x = E(KEY, a);
            intchar(c, x);
            fwrite(c, 1, 8, stdout);
        }
        else {
            x = D(KEY, a);
            intchar(c, x);
            if (i==size-1) {
                int size1=checkprint64(c);
                if (size1 == -1) {
                    cerr << "Bad text to decrypt (expected 1|0|...|0 in the end)" << endl;
                    printman();
                    exit(6);
                }
                else {
                    fwrite(c, 1, size1, stdout);
                }
            }
            else
                fwrite(c, 1, 8, stdout);
        }
        i++;
    }
}

void ctr(uint32_t KEY[32], char * ivname, int size) {
    int i=0, size1;
    char c[8];
    v32 a;
    uint32_t iv = readiv(ivname);
    uint64_t ctri = 0, x;
    ctri = ctri + ((((uint64_t)iv))&0xffffffff);
    ctri = ctri << 32;
    deploy(KEY);
    while (i < size) {
        x=fread(c, 1, 8, stdin);
        x = charint(c);
        a = v32((uint32_t)((ctri&0xFFFFFFFF00000000)>>32), (uint32_t)ctri&0xFFFFFFFF);
        x = x ^ E(KEY, a);
        intchar(c, x);
        fwrite(c, 1, 8, stdout);
        i++;
        ctri++;
    }
    size1=fread(c, 1, 8, stdin);
    if (size1==0)
        return;
    x = charint(c, size1);
    a = v32((uint32_t)((ctri&0xFFFFFFFF00000000)>>32), (uint32_t)ctri&0xFFFFFFFF);
    x = x ^ E(KEY, a);
    intchar(c, x);
    fwrite(c, 1, size1, stdout);
}


void readiv(char * name, queue<uint64_t> &qv) {
    char c[8];
    ifstream ivd;
    if (name == NULL) {
        qv.push(0);
        return;
    }
    ivd.open(name, ios::binary);
    if (!ivd.is_open()) {
        cerr << "Can`t open " << name << endl;
        printman();
        exit(3);
    };
    while(ivd.read(c, 8)) {
        qv.push(charint(c));
    }
}


void ofb(uint32_t KEY[32], char * ivname, int size) {
    int i=0;
    char c[8];
    v32 a;
    queue<uint64_t> qv;
    readiv(ivname, qv);
    uint64_t x, Yi, iv;
    deploy(KEY);
    while (i<size) {
        iv=qv.front();
        qv.pop();
        a = v32((uint32_t)((iv&0xFFFFFFFF00000000)>>32), (uint32_t)iv&0xFFFFFFFF);
        Yi=E(KEY, a);
        x=fread(c, 1, 8, stdin);
        x = charint(c);
        x = x ^ Yi;
        intchar(c, x);
        fwrite(c, 1, 8, stdout);
        i++;
        qv.push(Yi);
    }
     iv=qv.front();
        qv.pop();
    a = v32((uint32_t)((iv&0xFFFFFFFF00000000)>>32), (uint32_t)iv&0xFFFFFFFF);
    Yi=E(KEY, a);
    int size1=fread(c, 1, 8, stdin);
    if (size1==0)
        return;
    x = charint(c, size1);
    x = x ^ Yi;
    x = x >> (8*(8-size1));
    x = x << (8*(8-size1));
    intchar(c, x);
    fwrite(c, 1, size1, stdout);
}


void cbc(bool enc, uint32_t KEY[32], char * ivname, int size) {
    int i=0;
    char c[8];
    v32 a;
    queue<uint64_t> qv;
    readiv(ivname, qv);
    uint64_t x, z, iv;
    deploy(KEY);
    iv=qv.front();
    qv.pop();
    while (i<size) {
        x=fread(c, 1, 8, stdin);
        x = charint(c);
        if (enc) {
            a = v32((uint32_t)(((iv^x)&0xFFFFFFFF00000000)>>32), (uint32_t)(iv^x)&0xFFFFFFFF);
            x=E(KEY, a);
            qv.push(x);
            iv=qv.front();
            qv.pop();
        }
        else {
            a = v32((uint32_t)(((x)&0xFFFFFFFF00000000)>>32), (uint32_t)(x)&0xFFFFFFFF);
            z = x;
            x = D(KEY, a) ^ iv;
            iv=qv.front();
            qv.pop();
            qv.push(z);
        }
        intchar(c, x);
        if (!enc && i == size-1) {
            int size1=checkprint64(c);
            if (size1 == -1) {
                cerr << "Bad text to decrypt (expected 1|0|...|0 in the end)" << endl;
                printman();
                exit(6);
            }
            else {
                fwrite(c, 1, size1, stdout);
            }
        }
        else
            fwrite(c, 1, 8, stdout);
        i++;
    }
}

void cfb (bool enc, uint32_t KEY[32], char * ivname, int size) {
    int i=0;
    char c[8];
    v32 a;
     queue<uint64_t> qv;
    readiv(ivname, qv);
    uint64_t x, iv;
    deploy(KEY);
    iv=qv.front();
    qv.pop();
     while (i<size) {
        a = v32((uint32_t)((iv&0xFFFFFFFF00000000)>>32), (uint32_t)iv&0xFFFFFFFF);
        x=fread(c, 1, 8, stdin);
        x = charint(c);
        if (enc) {
            x = x ^ E(KEY, a);
            iv=qv.front();
            qv.pop();
            qv.push(x);
        }
        else {
            iv=qv.front();
            qv.pop();
            qv.push(x);
            x = x ^ E(KEY, a);
        }
        intchar(c, x);
        if (!enc && (i == size-1)) {
            int size1=checkprint64(c);
            if (size1 == -1) {
                cerr << "Bad text to decrypt (expected 1|0|...|0 in the end)" << endl;
                printman();
                exit(6);
            }
            else {
                fwrite(c, 1, size1, stdout);
            }
        }
        else
            fwrite(c, 1, 8, stdout);
        i++;
    }
}


void mac(uint32_t KEY[32], char * name) {
    uint64_t R, K1, K2, B=0x1B, C=0, x;
    v32 a; //=0;
    char c[8];
    int i=0, r;
    fstream file;
    file.open(name);
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.close();
    if (size % 8)
        size = size/8;
    else
        size=size/8-1;
    deploy(KEY);
    R=E(KEY, a);
    if (R>>63) 
        K1=(R<<1)^B;
    else
        K1=R<<1;
    if (K1>>63)
        K2=(K1<<1)^B;
    else
        K2=K1<<1;
    while (i<size) {
        x=fread(c, 1, 8, stdin);
        x = charint(c);
        x=x^C;
        a = v32((uint32_t)((x&0xFFFFFFFF00000000)>>32), (uint32_t)x&0xFFFFFFFF);
        C=E(KEY, a);
        i++;
    }
    r = fread(c, 1, 8, stdin);
    if (r==8){
        x = charint(c);
        x=x^C^K1;
        a = v32((uint32_t)((x&0xFFFFFFFF00000000)>>32), (uint32_t)x&0xFFFFFFFF);
        x=E(KEY, a);
    }
    else {
        c[r]=0x80; //1000 0000
        for (int j=r+1; j<8; j++) 
            c[j]=0;
        x = charint(c);
        x=x^C^K2;
        a = v32((uint32_t)((x&0xFFFFFFFF00000000)>>32), (uint32_t)x&0xFFFFFFFF);
        x=E(KEY, a);
    }
    intchar(c, x); 
    fwrite(c, 1, 4, stdout); //Ts where s=32
}


int search(int argc, char * argv[], char * s) {
    for (int i=1; i<argc; i++) 
        if (!strcmp(argv[i], s)) 
            return i;
    return 0;
}

void checkkey(char * name) {
    int size;
    fstream file;
    file.open(name);
    file.seekg(0, std::ios::end);  
    size = file.tellg();
    file.close();
    if (size != 32) {
        cerr << "Unexpected key length" << endl;
        exit(10);
    }
}

void checkiv32(char* name) {
    if (name == NULL)
        return;
    int size;
    fstream file;
    file.open(name);
    file.seekg(0, std::ios::end);  
    size = file.tellg();
    file.close();
    if (size != 4) {
        cerr << "Unexpected iv length" << endl;
        exit(10);
    }
}

void checkiv(char* name) {
    if (name == NULL)
        return;
    int size;
    fstream file;
    file.open(name);
    file.seekg(0, std::ios::end);  
    size = file.tellg();
    file.close();
    if ((size%8)) {
        cerr << "Unexpected iv length" << endl;
        exit(10);
    }
}

bool fecb = false, fctr = false, fofb = false, fcbc = false, fcfb = false, fmac = false,
        e = false, d = false, v = false, k = false, in = false, o = false, cr = false;
char * iname=NULL, * oname = NULL, *kname=NULL, *vname=NULL;

void setflags(int argc, char * argv[]) {
int i=1;
    while (i<argc) {
        if (!strcmp(argv[i], "--ecb")) {
            fecb = true;
            i++;
        }
        else if(!strcmp(argv[i], "--ctr")) {
            fctr = true;
            i++;
        }
        else if(!strcmp(argv[i], "--ofb")) {
            fofb = true;
            i++;
        }
        else if(!strcmp(argv[i], "--cbc")) {
            fcbc = true;
            i++;
        }
        else if(!strcmp(argv[i], "--cfb")) {
            fcfb = true;
            i++;
        }
        else if(!strcmp(argv[i], "--mac")) {
            fmac = true;
            i++;
        }
        else if(!strcmp(argv[i], "-e")) {
            e = true;
            i++;
        }
        else if(!strcmp(argv[i], "-d")) {
            d = true;
            i++;
        }
        else if(!strcmp(argv[i], "-k")) {
            k = true;
            i++;
            if (i==argc)
                throw "No key-file after -k";
            kname = argv[i];
            i++;
        }
        else if(!strcmp(argv[i], "-i")) {
            in = true;
            i++;
            if (i==argc)
                throw "No file after -i";
            iname = argv[i];
            i++;
        }
        else if(!strcmp(argv[i], "-o")) {
            o = true;
            i++;
            if (i==argc)
                throw "No file after -o";
            oname = argv[i];
            i++;
        }
        else if(!strcmp(argv[i], "-v")) {
            v = true;
            i++;
            if (i==argc)
                throw "No IV-file after -v";
            vname = argv[i];
            i++;
        }
        else {
            cerr << "Unexpected parameter " << argv[i] << endl;
            printman();
            exit(6);
        }
    }
}

void checkcorrect(){
    int i=0;
    if (!fctr && !fofb && !fcbc && !fcfb && !fmac)
            fecb=true;
    if (fecb)
        i++;
    if (fctr)
        i++;
    if (fofb)
        i++;
    if (fcbc)
        i++;
    if (fcfb)
        i++;
    if (fmac)
        i++;
    if (i!=1) {
        cerr << "Expected only one mode-flag or no flags" << endl;
        printman();
        exit(11);
    }
        if (fmac) {
            if (e) 
                throw "Unexpected -e with --mac";
            else if(d)
                throw "Unexpected -d with --mac";
            else if(v)
                throw "Unexpected -v with --mac";
        }
        if (fecb && v)
            throw "Unexpected -v with --ecb";
        if (e && d)
            throw "Unexpected -e and -d";
        if (!fmac && !e && !d)
            throw "Expected -e or -d";
        if (!k)
            throw "Expected -k <keyfile>";
}

void callmode(uint32_t KEY[32]) {
    int size;
    if (fecb) {
            if (e)
                size = addition(iname); 
            else
                size = filesize64(iname);
            ecb(e, KEY, size);
        }
        else if(fctr) {
            checkiv32(vname);
            size = filesize64(iname);
            ctr(KEY, vname, size);
        }
        else if(fofb) {
            checkiv(vname);
            size = filesize64(iname);
            ofb(KEY, vname, size);
        }
        else if(fcbc) {
            checkiv(vname);
            if (e)
                size = addition(iname); 
            else
                size = filesize64(iname);
            cbc(e, KEY, vname, size);
        }
        else if(fcfb) {
            checkiv(vname);
            if (e)
                size = addition(iname); 
            else
                size = filesize64(iname);
            cfb(e, KEY, vname, size);
        }
        else if(fmac) {
            mac(KEY, iname);
        }
        else {
            cerr << "Unexpected" << endl;
            exit(4);
        }
        if (cr)
            if (remove("readmagma1239.bin")==-1) {
                cerr << "error" << endl;
                exit(6);
            }
}

void inout(){
    FILE * f, * g, *f1;
        if (in) {
            if (!(f=freopen(iname, "rb", stdin))) {
                cerr << "Freopen error" << endl;
                exit(1);
            };
        }
        else {
            if (!(f=freopen(NULL, "rb", stdin))) {
                cerr << "Freopen error" << endl;
                exit(1);
            };
            if (!(f1=fopen("readmagma1239.bin", "wb"))){
                cerr << "error" << endl;
                exit(8);
            }
            cr = true;
            char c[8];
            int t;
            while ((t=fread(c, 1, 8, f))!=0) 
                fwrite(c, 1, t, f1);
            fclose(f);
            fclose(f1);
            iname=(char *)"readmagma1239.bin";
            if (!(f=freopen(iname, "rb", stdin))) {
                cerr << "Freopen error" << endl;
                exit(1);
            };
        };
        if (o) {
            if (!(g=freopen(oname, "wb", stdout))) {
                cerr << "Freopen error" << endl;
                exit(1);
                fclose(stdout);
            };
        }
}

int main(int argc, char * argv[]) {
    uint32_t KEY[32];
    try {
        if (argc == 2)
            if (search(argc, argv, (char *)"-h") || search(argc, argv, (char *)"--help")) {
                printman();
                return 0;
            };
        setflags(argc, argv);
        checkcorrect();
        inout();
        readkey(KEY, kname);
        checkkey(kname);
        deploy(KEY);
        callmode(KEY);
        return 0;
    } //try end
    catch (const char * s) {
        cerr << s << endl;
        printman();
        return 2;
    }
}