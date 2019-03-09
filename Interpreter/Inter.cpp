#include <iostream>
#include <string.h>
#include <stdio.h>
#include <string>

using namespace std;


enum type_of_lex
{
	LEX_NULL, // 0
	LEX_PROGRAM, // 1
	LEX_INT, //2
	LEX_STRING, //3
	LEX_REAL, //4
	LEX_IF, //5
	LEX_CASE,//6
	LEX_OF,//7
	LEX_END,//8
	LEX_FOR,//9
	LEX_STEP,//10
	LEX_UNTIL,//11
	LEX_DO,//12
	LEX_READ,//13
	LEX_WRITE,//14
	LEX_BREAK,//15
	LEX_AND,//16
	LEX_NOT,//17
	LEX_OR,//18
	LEX_FIN,//19
	LEX_SEMICOLON,//20
	LEX_COMMA,//21
	LEX_COLON,//22
	LEX_ASSIGN,//23
	LEX_LPAREN,//24
	LEX_RPAREN,//25
	LEX_EQ,//26
	LEX_LSS,//27
	LEX_GTR,//28
	LEX_PLUS,//29
	LEX_MINUS,//30
	LEX_TIMES,//31
	LEX_SLASH,//32
	LEX_LEQ,//33
	LEX_NEQ,//34
	LEX_GEQ,//35
	LEX_PERCENT,//36
	LEX_LSO, //37
	LEX_RSO, //38
	LEX_NUM,//39
	LEX_STR,//40
	LEX_RNUM,//41
	LEX_ID,//42
	POLIZ_LABEL, // 43
	POLIZ_ADDRESS, // 44
	POLIZ_GO, // 45
	POLIZ_FGO, // 46
	POLIZ_UNFGO, // 47
	POLIZ_SPECIAL_EQ, // 48 если равно, возвращаем 1, иначе первый операнд и 0 (для case)
	LEX_UPLUS,
	LEX_UMINUS
};



class Lex
{
	type_of_lex t_lex = LEX_NULL;
	int v_lex = 0;
	double vd_lex = 0;
	string s_lex = "";
public:
	Lex(type_of_lex t = LEX_NULL)
	{
		t_lex = t; v_lex = 0; 
	}
	Lex(type_of_lex t, int v) {
		t_lex = t; v_lex = v;
	}
	Lex(type_of_lex t, double vd) {
		t_lex = t; vd_lex = vd;
	}
	Lex(type_of_lex t, string s) {
		t_lex = t; s_lex = s;
	}
	type_of_lex get_type() { return t_lex; }
	void put_value(int a) {
		v_lex=a;
	}
	void put_value(double a) {
		vd_lex=a;
	}
	void put_value(string a) {
		s_lex=a;
	}
	int get_value() { return v_lex; }
	double get_dvalue() {return vd_lex;}
	string get_svalue() {return s_lex;}
	friend ostream& operator << (ostream &s, Lex l)
	{
		if (l.t_lex!=LEX_RNUM && l.t_lex!=LEX_STR) s << '(' << l.t_lex << ',' << l.v_lex << ");";
			else if (l.t_lex==LEX_RNUM) s << '(' << l.t_lex << ',' << l.vd_lex << ");";
				else s << '(' << l.t_lex << ',' << '"' << l.s_lex << '"' << ");";
		return s;
	}
};


class ident
{
	char * name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value;
	double dvalue;
	string svalue;
public:
	ident()
	{
		declare = false;
		assign = false;
	}
	char *get_name()
	{
		return name;
	}
	void put_name(const char *n)
	{
		name = new char[strlen(n) + 1];
		strcpy(name, n);
	}
	bool get_declare()
	{
		return declare;
	}
	void put_declare()
	{
		declare = true;
	}
	type_of_lex get_type()
	{
		return type;
	}
	void put_type(type_of_lex t)
	{
		type = t;
	}
	bool get_assign()
	{
		return assign;
	}
	void put_assign()
	{
		assign = true;
	}
	int get_value()
	{
		return value;
	}
	double get_dvalue()
	{
		return dvalue;
	}
	string get_svalue()
	{
		return svalue;
	}
	void put_value(int v)
	{
		value = v;
	}
	void put_value(double v)
	{
		dvalue = v;
	}
	void put_value(string v)
	{
		svalue = v;
	}

};

class tabl_ident
{
	ident * p;
	int size;
	int top;
public:
	tabl_ident(int max_size)
	{
		p = new ident[size = max_size];
		top = 1;
	}
	~tabl_ident()
	{
		delete[]p;
	}
	ident& operator[] (int k)
	{
		return p[k];
	}
	int put(const char *buf);
};
int tabl_ident::put(const char *buf)
{
	for (int j = 1; j<top; ++j)
		if (!strcmp(buf, p[j].get_name()))
			return j;
	p[top].put_name(buf);
	++top;
	return top - 1;
}

class Scanner
{
	enum state { H, IDENT, NUMB, ALE, DELIM, NEQ, STR, REAL };
	static char * TW[];
	static type_of_lex words[];
	static char * TD[];
	static type_of_lex dlms[];
	state CS;
	FILE * fp;
	char c;
	char buf[80];
	int buf_top;
	void clear()
	{
		buf_top = 0;
		for (int j = 0; j < 80; ++j)
			buf[j] = '\0';
	}
	void add()
	{
		buf[buf_top++] = c;
	}
	int look(const char *buf, char **list)
	{
		int i = 0;
		while (list[i])
		{
			if (!strcmp(buf, list[i]))
				return i;
			++i;
		}
		return 0;
	}
	void gc()
	{
		c = fgetc(fp);
	}
public:
	Lex get_lex();
	Scanner(const char * program)
	{
		fp = fopen(program, "r");
		CS = H;
		clear();
		gc();
	}
};

char * Scanner::TW[] =
{
	(char*)"", // 0 
	(char*)"program", // 1
	(char*)"int", // 2
	(char*)"string", // 3
	(char*)"real", // 4
	(char*)"if", // 5
	(char*)"case", // 6
	(char*)"of", // 7
	(char*)"end", // 8
	(char*)"for", // 9
	(char*)"step", // 10
	(char*)"until", // 11
	(char*)"do", // 12
	(char*)"read", // 13
	(char*)"write", // 14
	(char*)"break", // 15
	(char*)"and", // 16
	(char*)"not", // 17
	(char*)"or", // 18
	NULL
};
char * Scanner::TD[] =
{
	(char*)"", // 0 
	(char*)"@", // 1
	(char*)";", // 2
	(char*)",", // 3
	(char*)":", // 4
	(char*)"=", // 5
	(char*)"(", // 6
	(char*)")", // 7
	(char*)"==", // 8
	(char*)"<", // 9
	(char*)">", // 10
	(char*)"+", // 11
	(char*)"-", // 12
	(char*)"*", // 13
	(char*)"/", // 14
	(char*)"<=", // 15
	(char*)"!=", // 16
	(char*)">=", // 17
	(char*)"%", // 18
	(char*)"{", // 19
	(char*)"}", // 20
	NULL
};
tabl_ident TID(100);
type_of_lex Scanner::words[] = {
	LEX_NULL, // 0
	LEX_PROGRAM, // 1
	LEX_INT, //2
	LEX_STRING, //3
	LEX_REAL, //4
	LEX_IF, //5
	LEX_CASE,//6
	LEX_OF,//7
	LEX_END,//8
	LEX_FOR,//9
	LEX_STEP,//10
	LEX_UNTIL,//11
	LEX_DO,//12
	LEX_READ,//13
	LEX_WRITE,//14
	LEX_BREAK,//15
	LEX_AND,//16
	LEX_NOT,//17
	LEX_OR//18
};
type_of_lex Scanner::dlms[] = {
	LEX_NULL,
	LEX_FIN,
	LEX_SEMICOLON,
	LEX_COMMA,
	LEX_COLON,
	LEX_ASSIGN,
	LEX_LPAREN,
	LEX_RPAREN,
	LEX_EQ,
	LEX_LSS,
	LEX_GTR,
	LEX_PLUS,
	LEX_MINUS,
	LEX_TIMES,
	LEX_SLASH,
	LEX_LEQ,
	LEX_NEQ,
	LEX_GEQ,
	LEX_PERCENT
};

template <class T, int max_size > class Stack { 
	T s[max_size]; 
	int top; 
public: 
	Stack(){top = 0;} 
	void reset() { top = 0; } 
	void push(T i); 
	T pop(); 
	T get_top_el() { return s[top]; }
	bool is_empty(){ return top == 0; } 
	bool is_full() { return top == max_size; } 
};

template <class T, int max_size >

void Stack <T, max_size >::push(T i) { 
	if ( !is_full() ) 
		{ s[top] = i; ++top; } 
	else 
		throw "Stack_is_full"; }

template <class T, int max_size > 

T Stack <T, max_size >::pop() { 
	if ( !is_empty() ) 
		{ --top; return s[top]; } 
	else
	 throw "Stack_is_empty"; 
}


Lex Scanner::get_lex()
{
	int d, j, i1, i2;
	double q1, q2;
	char * l;
	CS = H;
	do
	{
		switch (CS)
		{
		case H:
			if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
				gc();
			else if (isalpha(c))
			{
				clear();
				add();
				gc();
				CS = IDENT;
			}
			else if (isdigit(c))
			{
				d = c - '0';
				gc();
				CS = NUMB;
			}
			else if (c == '{')
			{
				gc();
				return Lex(LEX_LSO);
			}
			else if (c == '}')
			{
				gc();
				return Lex(LEX_RSO);
			}
			else if (c == '=' || c == '<' || c == '>')
			{
				clear();
				add();
				gc();
				CS = ALE;
			}
			else if (c == '"')
				CS = STR;
			else if (c == '@') {
				return Lex(LEX_FIN);
			}
			else if (c == '%'){
                gc();
				return Lex(LEX_PERCENT);
			}
			else if (c == '!')
			{
				clear();
				add();
				gc();
				CS = NEQ;
			}
			else
				CS = DELIM;
			break;
		case IDENT:
			if (isalpha(c) || isdigit(c))
			{
				add();
				gc();
			}
			else
				if ((j = look(buf, TW))!=false) {
					return Lex(words[j], j);
				}
				else
				{
					j = TID.put(buf);
					return Lex(LEX_ID, j);
				}
			break;
		case NUMB:
			if (isdigit(c))
			{
				d = d * 10 + (c - '0');
				gc();
			}
			else
				if(c!='.')
					return Lex(LEX_NUM, d);
				else {
					gc();
					CS=REAL;
				}
			break;
		case REAL:
				q1=d;
				i2=1;
				while(isdigit(c)) {
					c=c-'0';
					q2=c;
					for(i1=0; i1<i2; i1++)
						q2=q2/10;
					i2++;
					q1=q1+q2;
					gc();
				}
				return Lex(LEX_RNUM, q1);
			break;
		case STR:
			clear();
			gc();
			while (c!='"') {
				add();
				gc();
			}
			l = new char[strlen(buf)+1];
			strcpy(l, buf);
			clear();
			gc();
			{
				string s;
				s = l;
				delete[] l;
				return Lex(LEX_STR, s);
				break;
			}
		case ALE:
			if (c == '=')
			{
				add();
				gc();
				j = look(buf, TD);
				return Lex(dlms[j], j);
			}
			else
			{
				j = look(buf, TD);
				return Lex(dlms[j], j);
			}
			break;
		case NEQ:
			if (c == '=')
			{
				add();
				gc();
				j = look(buf, TD);
				return Lex(LEX_NEQ, j);
			}
			else
				throw '!';
			break;
		case DELIM:
			clear();
			add();
			if ((j = look(buf, TD))!=false)
			{
				gc();
				return Lex(dlms[j], j);
			}
			else
				throw c;
			break;
		} // end switch
	} while (true);
}

class Poliz { 
	Lex *p; 
	int size; 
	int free; 
public: 
	Poliz ( int max_size ) { 
		p = new Lex [size = max_size]; 
		free = 0; 
	}; 
	~Poliz() { 
		delete []p; 
	}; 
	void put_lex(Lex l) { 
		p[free]=l; 
		++free; 
	}; 
	void put_lex(Lex l, int place) { 
		p[place]=l; 
	}; 
	type_of_lex get_lex_type(int place) {
		return p[place].get_type();
	}
	int get_lex_value(int place) {
		return p[place].get_value();
	}
	void blank() { 
		++free; 
	}; 
	int get_free() { 
		return free; 
	}; 
	void change_last(Lex l){
		p[free-1]=l;
	}
	int get_size() {
		return size;
	}
	Lex& operator[] ( int index ) { 
		if (index > size) 
			throw "POLIZ:out of array"; 
		else if ( index > free ) 
			throw "POLIZ:indefinite element of array"; 
		else 
			return p[index]; 
	};
	void print() { 
		for ( int i = 0; i < free; ++i ) 
			cout << i << p[i] << ' '; 
	}; 
};

class Parser {
	Lex curr_lex; // текущая лексема
	type_of_lex c_type;
	int c_val;
	double c_dval;
	string c_sval;
	bool bo; //
	Scanner scan;
	Stack < int, 500 > st_int;
	Stack < type_of_lex, 500 > st_lex;
	Stack < type_of_lex, 50 > st_case;
	Stack < int , 50 > st_case_int;
	Stack < char * , 50 > st_case_string;
	Stack <int, 50 > st_break;
	void P(); // процедуры РС-метода
	void D1();
	void D();
	void D2();
	void O();
	void B();
	void C();
	void SV();
	void V();
	void FOR();
	void READ();
	void WRITE();
	void BREAK();
	void AS();
	void S();
	void E();
	void E1();
	void T();
 	void F();
 	void EO();
	void E1O();
	void TO();
 	void FO();
	void dec ( type_of_lex); // семантичиеские действия
 	void check_id ();
  	void check_op ();
  	void check_uop();
   	void check_not ();
   	void eq_type ();
   	void eq_bool ();
   	void check_id_in_read ();
    void gl () {                   // получить очередную лексему
    	curr_lex = scan.get_lex();
    	c_type = curr_lex.get_type();
    	c_val = curr_lex.get_value();
    	c_dval = curr_lex.get_dvalue();
    	c_sval = curr_lex.get_svalue();
     //  	cout << curr_lex;
    }
public:
	Poliz prog; // внутреннее представление программы
	Parser (const char *program) : scan (program), prog(1000) {}//, prog (1000) {}
	void analyze(); // анализатор с действиями
	void poliz_print() {
		prog.print();
		cout << endl;
	}
};

void Parser::analyze () {
	gl ();
	P ();
	cout << "Successful analysis" << endl;
}

void Parser::check_id() { 
	if ( TID[c_val].get_declare() ) 
		st_lex.push(TID[c_val].get_type()); 
	else 
		throw "not declared"; 
}

void Parser::dec ( type_of_lex type ) {
 	int i; 
 	while ( !st_int.is_empty()) { 
 		i = st_int.pop(); 
 		if ( TID[i].get_declare() ) 
 			throw "twice"; 
 		else { 
 			TID[i].put_declare(); 
 			TID[i].put_type(type); 
 		} 
 	} 
}

void Parser::P() {  //синтаксический анализ
	if (c_type == LEX_PROGRAM)
		gl();
	else
		throw curr_lex;
	while (c_type != LEX_FIN) {
		if (c_type == LEX_INT)
			D();
		else if (c_type == LEX_STRING)
			D1();
		else if (c_type == LEX_REAL)
			D2();
		else
			O();
	}
}

void Parser::D() {   //описаниe int
	st_int.reset();
	do {
		int a;
		bool flag;
		gl();
		if (c_type != LEX_ID)
			throw curr_lex;
		st_int.push(c_val);
		a=c_val;
		gl();
		if (c_type == LEX_ASSIGN) {
			gl();
			if (c_type == LEX_PLUS || c_type == LEX_MINUS) {
				if (c_type == LEX_MINUS) 
					flag = true;
				gl();
			}
			if (c_type != LEX_NUM)
				throw curr_lex;
			if (flag)
				TID[a].put_value(-c_val);
			else
				TID[a].put_value(c_val);
			TID[a].put_assign();
			flag = false;
            gl();
		}
	}
	while (c_type == LEX_COMMA);
	dec(LEX_NUM);
	if (c_type != LEX_SEMICOLON)
		throw curr_lex;
	gl();
}

void Parser::D1() {   //описания string
	st_int.reset();
	do {
		int a;
		gl();
		if (c_type != LEX_ID)
			throw curr_lex;
		st_int.push(c_val);
		a=c_val;
		gl();
		if (c_type == LEX_ASSIGN) {
			gl();
			if (c_type != LEX_STR)
				throw curr_lex;
		}
		TID[a].put_value(c_sval);
		TID[a].put_assign();
		gl();
	}
	while (c_type == LEX_COMMA);
	dec(LEX_STR);
	if (c_type != LEX_SEMICOLON)
		throw curr_lex;
	gl();
}

void Parser::D2() {   //описаниe real
	st_int.reset();
	do {
		int a;
		bool flag, f2;
		gl();
		if (c_type != LEX_ID)
			throw curr_lex;
		st_int.push(c_val);
		a=c_val;
		gl();
		if (c_type == LEX_ASSIGN) {
			gl();
			if (c_type == LEX_PLUS || c_type == LEX_MINUS){
				if (c_type == LEX_MINUS) 
					flag = true;
				gl();
			}
			if (c_type != LEX_RNUM && c_type != LEX_NUM)
				throw curr_lex;
			if (c_type == LEX_NUM)
				f2=true;
			else
				f2=false;
			if (flag && f2)
				TID[a].put_value((double)-c_val);
			else if (!flag && f2)
				TID[a].put_value((double)c_val);
			else if (flag && !f2)
				TID[a].put_value(-c_dval);
			else
				TID[a].put_value(c_dval);
			TID[a].put_assign();
			flag = false;
			f2=false;
            gl();
		}
	}
	while (c_type == LEX_COMMA);
	dec(LEX_RNUM);
	if (c_type != LEX_SEMICOLON)
		throw curr_lex;
	gl();
}


void Parser::O(){ //перебор операторов
	bo=false;
	if (c_type == LEX_LSO) {
        gl();
        do
            O();
        while (c_type != LEX_RSO);
        gl();
	}
	else if (c_type == LEX_IF)
		B();
	else if (c_type == LEX_CASE)
		C();
	else if (c_type == LEX_FOR)
		FOR();
	else if (c_type == LEX_READ)
		READ();
	else if (c_type == LEX_WRITE)
		WRITE();
	else if (c_type == LEX_BREAK)
		BREAK();
	else {
		EO();
		if (c_type != LEX_SEMICOLON) 
			throw curr_lex;
		gl();
	}
}

void Parser::B() {  //оператор if
	int pl2;
	gl();
	if (c_type != LEX_LPAREN)
		throw curr_lex;
	E();
	pl2 = prog.get_free();
	prog.blank();
	prog.put_lex(Lex(POLIZ_FGO));
	if (c_type != LEX_RPAREN)
		throw curr_lex;
	gl();
	O();
	prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);
}

void Parser::C(){ //оператор case
	type_of_lex t;
	gl();
	if (c_type != LEX_LPAREN)
		throw curr_lex;
	E();
	t=st_lex.pop();
	st_lex.push(t);
	st_case.push(t);
	if (c_type != LEX_RPAREN)
		throw curr_lex;
	gl();
	if (c_type != LEX_OF)
		throw curr_lex;
	gl();
	SV();
	if (c_type != LEX_END)
		throw curr_lex;
	gl();
	if (c_type != LEX_SEMICOLON)
		throw curr_lex;
	gl();
}

void Parser::SV(){ //список вариантов
	int pl[20], i, ps[20], j = 0, pz[20], z = 0, start; 	//pl исполнять если верно ps на следующий оператор после предыдущего 
	start = prog.get_free();											 	//pz после всех констант в варианте на следующий
	do { //Вариант
		i=0;
		if (j == 20)
			throw "to much variants in case, change pl.size (2 strings highter)\n";
		if (z > 0)
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pz[z-1]);
		type_of_lex t=st_case.pop();
		st_case.push(t);
		if (t == LEX_INT) /////////////// семантика
			t = LEX_NUM;
		else if (t == LEX_STRING)
			t = LEX_STR;
		else 
			t = LEX_RNUM;
		if (c_type != LEX_NUM && c_type != LEX_STR)
			throw curr_lex;
		if (c_type != t)
			if (t != LEX_RNUM || c_type != LEX_NUM) 
				throw "wrong types in case"; ///////////////////// семантика
		prog.put_lex(curr_lex);
		prog.put_lex(Lex(POLIZ_SPECIAL_EQ));
		pl[i] = prog.get_free();
		prog.blank();
		prog.put_lex(Lex(POLIZ_UNFGO));
		gl();
		i++;
		while (c_type == LEX_COMMA) {
			if (i == 20)
				throw "to much variants in case, change pl.size (2 strings highter)\n";
			gl();
			if (c_type != LEX_NUM && c_type != LEX_STR)
				throw curr_lex;
			if (c_type != t)
				if (t != LEX_RNUM || c_type != LEX_NUM) 
					throw "wrong types in case";
			prog.put_lex(curr_lex);
			prog.put_lex(Lex(POLIZ_SPECIAL_EQ));
			pl[i] = prog.get_free();
			prog.blank();
			prog.put_lex(Lex(POLIZ_UNFGO));
			gl();
			i++;
		} 
		pz[z] = prog.get_free();
		prog.blank();
		z++;
		prog.put_lex(Lex(POLIZ_GO));
		for (int m = 0; m < i; m++)
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl[m]);
		if (j>0)
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), ps[j-1]);
		if (c_type != LEX_COLON)
			throw curr_lex;
  	    gl();
 	    O();
 	    ps[j] = prog.get_free();
		prog.blank();
		prog.put_lex(Lex(POLIZ_GO));
		j++;
	}
	while (c_type != LEX_END);
	for (int q = start; q<prog.get_free(); q++) { // все что осталось нулем - break 
		if (prog.get_lex_type(q) == LEX_NULL) { ///////prog.get_lex_value(q) == prog.get_free()+28///
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), q);
		}
	}
}

void Parser::FOR(){ // I` E() = until jmp I` I step() + = until() after jz O() Istep() jmp
	gl();
	int start = prog.get_free();
	if (c_type != LEX_ID)
		throw curr_lex;
	check_id();
	int pcounter = c_val;
	prog.put_lex(Lex(POLIZ_ADDRESS, pcounter)); //I`
	gl();
	if (c_type != LEX_ASSIGN)
		throw curr_lex;
	E(); 
	prog.put_lex(Lex(LEX_ASSIGN)); // =
	int pl1 = prog.get_free(); //метка на until
	prog.blank(); 
	prog.put_lex(Lex(POLIZ_GO)); //jmp
	int remember = prog.get_free();
	prog.put_lex(Lex(POLIZ_ADDRESS, pcounter));
	prog.put_lex(Lex(LEX_ID, pcounter));

	if (c_type != LEX_STEP)
		throw curr_lex;
	E(); //step
	prog.put_lex(Lex(LEX_PLUS));
	prog.put_lex(Lex(LEX_ASSIGN));
	if (c_type != LEX_UNTIL)
		throw curr_lex;
	prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl1);
	E();
	int pl2 = prog.get_free();
	prog.blank();
	prog.put_lex(Lex(POLIZ_FGO));
	if (c_type != LEX_DO)
		throw curr_lex;
	gl();
	O();
	prog.put_lex(Lex(POLIZ_LABEL, remember)); //возвращаемся в цикл для проверки увеличение на step и проверки условия
	prog.put_lex(Lex(POLIZ_GO));
	prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), pl2);
	for (int q = start; q<prog.get_free(); q++) { // все что осталось нулем - break 
		if (prog.get_lex_type(q) == LEX_NULL) { ///////prog.get_lex_value(q) == prog.get_free()+28///
			prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()), q);
		}
	}
}

void Parser::READ() {
	gl();
	if (c_type != LEX_LPAREN)
		throw curr_lex;
	gl();
	if (c_type != LEX_ID)
		throw curr_lex;
	check_id_in_read();
	prog.put_lex(Lex(POLIZ_ADDRESS, c_val));
	prog.put_lex(Lex(LEX_READ));
	gl();
	if (c_type != LEX_RPAREN)
		throw curr_lex;
	gl();
	if (c_type != LEX_SEMICOLON)
		throw curr_lex;
	gl();
}

void Parser::WRITE(){
	gl();
	if (c_type != LEX_LPAREN)
		throw curr_lex;
	E();
	while (c_type == LEX_COMMA)
		E();
	if (c_type != LEX_RPAREN)
		throw curr_lex;
	gl();
	if (c_type != LEX_SEMICOLON)
		throw curr_lex;
	gl();
	prog.put_lex(Lex(LEX_WRITE));
}

void Parser::BREAK(){
	gl();
	if (c_type != LEX_SEMICOLON)
		throw curr_lex;
	prog.blank();
	prog.put_lex(Lex(POLIZ_GO));
	gl();
}

void Parser::E(){ // выражение
	if (c_type == LEX_ID) {
		check_id();
		prog.put_lex(curr_lex);
		int helper = c_val;
		gl();
		if (c_type == LEX_ASSIGN) {
			prog.change_last(Lex(POLIZ_ADDRESS, helper));
			E();
			eq_type();
			prog.put_lex(Lex(LEX_ASSIGN));
			return;
			bo = false;
		}
		else {
			bo = true;
		}
	}
	E1();
	while (c_type == LEX_EQ || c_type == LEX_GTR || c_type == LEX_GEQ || c_type == LEX_LSS || c_type == LEX_LEQ || c_type == LEX_NEQ) {// аккуратно - прочел ли я очередную лексему в E1 или нужно перед этим добавить gl(); ?
		st_lex.push(c_type);
		E1();
		check_op();
	}
}

void Parser::E1(){
	if (!bo) T();
	while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR) {
		st_lex.push(c_type);
		T();
		check_op();
	}
}

void Parser::T(){
	if (!bo) F();
		else bo=false;
	while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_PERCENT || c_type == LEX_AND) {
		st_lex.push(c_type);
		F();
		check_op();
	}
}

void Parser::F(){
	gl();
	if (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_NOT) { // унарные +-not
		st_lex.push(c_type);
		E();
		check_uop();
	}
	else if (c_type != LEX_ID && c_type != LEX_NUM && c_type != LEX_RNUM && c_type != LEX_STR) {
			throw curr_lex;
		}
	else if (c_type == LEX_ID) {
		check_id();
		prog.put_lex(curr_lex);
		int helper = c_val;
		gl();
		if (c_type == LEX_ASSIGN) {
			prog.change_last(Lex(POLIZ_ADDRESS, helper));
			E();
			eq_type();
			prog.put_lex(Lex(LEX_ASSIGN));
		}
	}
	else if (c_type == LEX_NUM){
		st_lex.push(LEX_NUM);
		prog.put_lex(curr_lex);
		gl();
	}
	else if (c_type == LEX_RNUM){
		st_lex.push(LEX_RNUM);
		prog.put_lex(curr_lex);
		gl();
	}
	else {
		st_lex.push(LEX_STR);
		prog.put_lex(curr_lex);
		gl();
	}
}


void Parser::EO(){ // выражение
	if (c_type == LEX_ID) {
		check_id();
		prog.put_lex(curr_lex);
		int helper = c_val;
		gl();
		if (c_type == LEX_ASSIGN) {
			prog.change_last(Lex(POLIZ_ADDRESS, helper));
			E();
			eq_type();
			prog.put_lex(Lex(LEX_ASSIGN));
			return;
			bo = false;
		}
		else {
			bo = true;
		}
	}
	E1O();
	while (c_type == LEX_EQ || c_type == LEX_GTR || c_type == LEX_GEQ || c_type == LEX_LSS || c_type == LEX_LEQ || c_type == LEX_NEQ) {// аккуратно - прочел ли я очередную лексему в E1 или нужно перед этим добавить gl(); ?
		st_lex.push(c_type);
		E1();
		check_op();
	}
}

void Parser::E1O(){
	TO();
	while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_OR) {
		st_lex.push(c_type);
		T();
		check_op();
	}
}

void Parser::TO(){
	if (!bo) FO();
		else bo=false;
	while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_PERCENT || c_type == LEX_AND) {
		st_lex.push(c_type);
		F();
		check_op();
	}
}

void Parser::FO(){
	if (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_NOT) { // унарные +-not
		st_lex.push(c_type);
		E();
		check_uop();
	}
	else if (c_type != LEX_ID && c_type != LEX_NUM && c_type != LEX_RNUM && c_type != LEX_STR) 
			throw curr_lex;
	else if (c_type == LEX_ID) {
		check_id();
		prog.put_lex(curr_lex);
		int helper = c_val;
		gl();
		if (c_type == LEX_ASSIGN) {
			prog.change_last(Lex(POLIZ_ADDRESS, helper));
			E();
			eq_type();
			prog.put_lex(Lex(LEX_ASSIGN));
		}
	}
	else if (c_type == LEX_NUM){
		st_lex.push(LEX_NUM);
		prog.put_lex(curr_lex);
		gl();
	}
	else if (c_type == LEX_RNUM){
		st_lex.push(LEX_RNUM);
		prog.put_lex(curr_lex);
		gl();
	}
	else {
		st_lex.push(LEX_STR);
		prog.put_lex(curr_lex);
		gl();
	}
}

void Parser::check_uop() {
	type_of_lex t, op;
	t = st_lex.pop();
	op = st_lex.pop();
	if (t == LEX_RNUM)
		st_lex.push(LEX_RNUM);
	else if (t == LEX_NUM)
		st_lex.push(LEX_NUM);
	else 
		throw "not int/real in unary";
	if (op == LEX_PLUS)
		op = LEX_UPLUS;
	else if (op == LEX_MINUS)
		op = LEX_UMINUS;
	else 
		op = LEX_NOT;
	prog.put_lex(Lex(op));
}

void Parser::check_op(){
	type_of_lex t1, t2, op, r;
	t2 = st_lex.pop();
	op = st_lex.pop();
	t1 = st_lex.pop();

	if ((t1 == LEX_STRING && t2 == LEX_STRING) || (t1 == LEX_STR && t2 == LEX_STR))
		r = LEX_STR;
	else if ((t1 == LEX_INT && t2 == LEX_INT) || (t1 == LEX_NUM && t2 == LEX_NUM))
		r = LEX_NUM;
	else if ((t1 == LEX_REAL && (t2 == LEX_INT || t2 == LEX_REAL)) || (t1 == LEX_RNUM && (t2 == LEX_NUM || t2 == LEX_RNUM)))
		r = LEX_RNUM;
	else if ((t2 == LEX_REAL && (t1 == LEX_INT || t1 == LEX_REAL)) || (t2 == LEX_RNUM && (t1 == LEX_NUM || t1 == LEX_RNUM)))
		r = LEX_RNUM;
	else 
		throw "1wrong types in operation";
	if (op == LEX_SLASH || op == LEX_PERCENT)
		if (r != LEX_NUM)
			throw "2wrong types in operation";
	if (op == LEX_EQ || op == LEX_NEQ || op == LEX_GEQ || op == LEX_GTR || op == LEX_LSS || op == LEX_LEQ)
		r = LEX_NUM;
	if (r == LEX_STR)
		if (op != LEX_PLUS && op != LEX_EQ && op != LEX_NEQ)
			throw "3wrong types in operation";
	st_lex.push(r);
	prog.put_lex(Lex(op));
}

void Parser::eq_type() {
	type_of_lex t1, t2;
	t2 = st_lex.pop();
	t1 = st_lex.pop();
	if (t1 != t2)
		throw "wrong types in =";
	else
		st_lex.push(t1);
}

void Parser::check_id_in_read () { 
	if ( !TID [c_val].get_declare() ) 
		throw "not declared"; 
}



class Executer { 
	Lex pc_el; 
public: 
	void execute ( Poliz& prog ); 
}; 
void Executer::execute ( Poliz& prog ) { 
	Stack < Lex, 500 > args; 
	int i, j, index = 0, size = prog.get_free(); 
	Lex l1, l2;
	while ( index < size ) { 
		pc_el = prog [ index ];
		switch ( pc_el.get_type () ) { 
			case LEX_STR:
			case LEX_NUM:
			case LEX_RNUM: 
			case POLIZ_ADDRESS: 
			case POLIZ_LABEL: 
				args.push ( pc_el ); 
				break; 
			case LEX_ID: 
				i = pc_el.get_value (); 
				if ( TID[i].get_assign () ) { 
					if (TID[i].get_type() == LEX_RNUM) {
						args.push ( Lex(TID[i].get_type (), TID[i].get_dvalue()) );
					}
					else if (TID[i].get_type() == LEX_STR)
						args.push ( Lex(TID[i].get_type (), TID[i].get_svalue()) ); 
					else 
						args.push ( Lex(TID[i].get_type (), TID[i].get_value()) );
					break; 
				}
				else 
					throw "POLIZ: indefinite identifier"; 
			case LEX_NOT: 
				l1=args.pop();
				args.push(Lex(l1.get_type(), !l1.get_value()));
				break; 
			case LEX_OR: 
				l1=args.pop();
				l2=args.pop();
				args.push(Lex(LEX_NUM, l1.get_value() || l2.get_value()));
				break; 
			case LEX_AND: 
				l1=args.pop();
				l2=args.pop();
				args.push(Lex(LEX_NUM, l1.get_value() && l2.get_value()));
				break; 
			case POLIZ_GO: 
				l1=args.pop();
				index = l1.get_value() - 1; 
				break; 
			case POLIZ_FGO: 
				l1 = args.pop();
				l2 = args.pop();
				if ( !l2.get_value() )
					index = l1.get_value() - 1; 
				break; 
			case POLIZ_UNFGO:
				l1 = args.pop();
				l2 = args.pop();
				if ( l2.get_value() )
					index = l1.get_value() - 1; 
				break;
			case LEX_WRITE: 
				l1 = args.pop();
				if (l1.get_type() == LEX_NUM) 
					cout << l1.get_value() << endl;
				else if (l1.get_type() == LEX_RNUM)
					cout << l1.get_dvalue() << endl;
				else if (l1.get_type() == LEX_STR)
					cout << l1.get_svalue() << endl;  
				break; 
			case LEX_READ: { 
					int k; 
					double d;
					string s;
					l1 = args.pop();
					i = l1.get_value(); //адрес ID
					if ( TID[i].get_type () == LEX_NUM ) {
						cout << "Input value for int "; cout << TID[i].get_name () << endl; 
						cin >> k; 
						TID[i].put_value(k);
					} 
					else if (TID[i].get_type () == LEX_RNUM){ ;
						cout << "Input value for real "; cout << TID[i].get_name () << endl; 
						cin >> d; 
						TID[i].put_value(d);
					}
					else if (TID[i].get_type () == LEX_STR){
						cout << "Input value for string "; cout << TID[i].get_name () << endl;
						getline(cin, s);
						TID[i].put_value(s);
					}
				} 
				TID[i].put_assign (); 
				break; 
			case LEX_PLUS: 
				l1 = args.pop();
				l2 = args.pop();
				if (l1.get_type() == LEX_STR) {
					args.push (Lex(l1.get_type(), l2.get_svalue()+l1.get_svalue())); 
				}
				else if (l1.get_type() == LEX_RNUM || l2.get_type() == LEX_RNUM)
					args.push (Lex(LEX_RNUM, l2.get_dvalue()+l2.get_value()+l1.get_dvalue()+l1.get_value())); 
				else
					args.push (Lex(l1.get_type(), l2.get_value()+l1.get_value())); 
				break; 
			case LEX_TIMES: 
				l1 = args.pop();
				l2 = args.pop();
				if (l1.get_type() == LEX_RNUM || l2.get_type() == LEX_RNUM)
					args.push (Lex(LEX_RNUM, (l2.get_dvalue()+l2.get_value())*(l1.get_dvalue()+l1.get_value()))); 
				else
					args.push (Lex(l1.get_type(), l2.get_value()*l1.get_value()));
				break; 
			case LEX_MINUS: 
				l1 = args.pop();
				l2 = args.pop();
				if (l1.get_type() == LEX_RNUM || l2.get_type() == LEX_RNUM)
					args.push (Lex(LEX_RNUM, l2.get_dvalue()+l2.get_value()-l1.get_dvalue()-l1.get_value())); 
				else
					args.push (Lex(l1.get_type(), l2.get_value()-l1.get_value())); 
				break; 
			case LEX_SLASH: 
				l1 = args.pop();
				l2 = args.pop();
				if (l1.get_value()){
					args.push (Lex(LEX_INT, l2.get_value()/l1.get_value())); 
					break; 
				}
				else 
					throw "POLIZ:divide by zero"; 
			case LEX_PERCENT: 
				l1 = args.pop();
				l2 = args.pop();
				if (l1.get_value()){
					args.push (Lex(LEX_INT, l2.get_value()%l1.get_value())); 
					break; 
				}
				else 
					throw "POLIZ:divide by zero"; 
			case LEX_UMINUS:
				l1 = args.pop();
				if (l1.get_type() == LEX_RNUM)
					args.push (Lex(l1.get_type(), -l1.get_dvalue()));
				else
					args.push (Lex(l1.get_type(), -l1.get_value()));
			case LEX_UPLUS: 
				break;
			case LEX_EQ:
				l1 = args.pop();
				l2 = args.pop();
				if (l1.get_type() == LEX_RNUM)
					args.push (Lex(LEX_NUM, l2.get_dvalue()==l1.get_dvalue()));
				else if (l1.get_type() == LEX_STR)
					args.push (Lex(LEX_NUM, l2.get_svalue()==l1.get_svalue()));
				else
					args.push (Lex(LEX_NUM, l2.get_value()==l1.get_value()));
				break;
			case POLIZ_SPECIAL_EQ: {
				l2 = args.pop();
				l1 = args.pop();
				if (l1.get_type() == LEX_RNUM) 
					if (l2.get_dvalue()==l1.get_dvalue())
						args.push (Lex(LEX_NUM, l2.get_dvalue()==l1.get_dvalue()));
					else {
						args.push (Lex(l1.get_type(), l1.get_dvalue()));
						args.push (Lex(LEX_NUM, l2.get_dvalue()==l1.get_dvalue()));
					}
				else if (l1.get_type() == LEX_STR) 
					if (l2.get_svalue()==l1.get_svalue())
						args.push (Lex(LEX_NUM, l2.get_svalue()==l1.get_svalue()));
					else {
						args.push (Lex(l1.get_type(), l1.get_svalue()));
						args.push (Lex(LEX_NUM, l2.get_svalue()==l1.get_svalue()));
					}
				else 
					if (l2.get_value()==l1.get_value())
						args.push (Lex(LEX_NUM, l2.get_value()==l1.get_value()));
					else {
						args.push (Lex(l1.get_type(), l1.get_value()));
						args.push (Lex(LEX_NUM, l2.get_value()==l1.get_value()));
					}
				break;
			}
			case LEX_LSS: 
				l2 = args.pop();
				l1 = args.pop();
				if (l1.get_type() == LEX_RNUM) 
					args.push (Lex(LEX_NUM, l1.get_dvalue()<l2.get_dvalue()));
				else
					args.push (Lex(LEX_NUM, l1.get_value()<l2.get_value()));
				break; 
			case LEX_GTR: 
				l2 = args.pop();
				l1 = args.pop();
				if (l1.get_type() == LEX_RNUM) 
					args.push (Lex(LEX_NUM, l1.get_dvalue()>l2.get_dvalue()));
				else
					args.push (Lex(LEX_NUM, l1.get_value()>l2.get_value()));
				break; 
			case LEX_LEQ: 
				l2 = args.pop();
				l1 = args.pop();
				if (l1.get_type() == LEX_RNUM) 
					args.push (Lex(LEX_NUM, l1.get_dvalue()<=l2.get_dvalue()));
				else
					args.push (Lex(LEX_NUM, l1.get_value()<=l2.get_value()));
				break; 
			case LEX_GEQ: 
				l2 = args.pop();
				l1 = args.pop();
				if (l1.get_type() == LEX_RNUM) 
					args.push (Lex(LEX_NUM, l1.get_dvalue()>=l2.get_dvalue()));
				else
					args.push (Lex(LEX_NUM, l1.get_value()>=l2.get_value()));
				break; 
			case LEX_NEQ: 
				l2 = args.pop();
				l1 = args.pop();
				if (l1.get_type() == LEX_RNUM)
					args.push (Lex(LEX_NUM, l2.get_dvalue()!=l1.get_dvalue()));
				else if (l1.get_type() == LEX_STR)
					args.push (Lex(LEX_NUM, l2.get_svalue()!=l1.get_svalue()));
				else
					args.push (Lex(LEX_NUM, l2.get_value()!=l1.get_value()));
				break; 
			case LEX_ASSIGN: 
				l1 = args.pop(); 
				l2 = args.pop();
				{
					if (l1.get_type() == LEX_RNUM) {
						double k = l1.get_dvalue();
						j = l2.get_value();
						TID[j].put_value(k); 
						TID[j].put_assign(); 
					}
					else if (l1.get_type() == LEX_STR) {
						string s = l1.get_svalue();
						j = l2.get_value();
						TID[j].put_value(s); 
						TID[j].put_assign(); 
					}
					else {
						i = l1.get_value();
						j = l2.get_value();
						TID[j].put_value(i); 
						TID[j].put_assign(); 
					}
				
				}	
				break; 
			default: 
				throw "POLIZ: unexpected elem"; 
		} // end of switch 
		++index; 
	}; //end of while 
	cout << "Finish of executing!!!" << endl; }
class Interpretator { 
	Parser pars; 
	Executer E; 
public: 
	Interpretator ( char* program ): pars (program) {}; 
	void interpretation (); 
};
void Interpretator::interpretation () { 
	pars.analyze (); 
	E.execute ( pars.prog ); 
}

int main (int argc, char * argv[]) { 
	try { 
		Interpretator I ( argv[1]);
		I.interpretation (); 
		return 0; 
	}
	catch ( char c ) { 
		cout << "unexpected symbol " << c << endl; 
		return 1; 
	} 
	catch ( Lex l ) { 
		cout << "unexpected lexeme"; 
		cout << l; 
		return 1; 
	} 
	catch ( const char *source ) { 
		cout << source << endl; 
		return 1; 
	} 
}