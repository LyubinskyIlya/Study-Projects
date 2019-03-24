#include <iostream>
#include <sstream>	

using namespace std;

enum Token_value : char { 
	NUMBER, END, PLUS='+', MINUS='-', MUL='*', DIV='/'
};

enum Number_value : char {
	NUM0='0', NUM1='1', NUM2='2',
	NUM3='3', NUM4='4', NUM5='5',
	NUM6='6', NUM7='7', NUM8='8',
	NUM9='9',
};

Token_value curr_tok = NUMBER;
int64_t number_value;
int64_t expr(istream*, bool);

Token_value get_token(istream* input) {
	char ch;
	do {
		if (!input->get(ch)) 
			return curr_tok = END;
	} while (isspace(ch));
	switch (ch) {
		case 0:
			return curr_tok = END;
		case MUL:
		case DIV:
		case PLUS:
		case MINUS:
			return curr_tok = Token_value(ch);
		case NUM0: case NUM1: case NUM2: case NUM3: case NUM4:
		case NUM5: case NUM6: case NUM7: case NUM8: case NUM9:
			input->putback(ch);
			*input >> number_value;
			return curr_tok = NUMBER;
		default:
			cerr << "error" << endl;
			exit(1);
	}
}

int64_t prim(istream* input) {
	get_token(input);
	switch (curr_tok) {
		case NUMBER: {
			int64_t v = number_value;
			get_token(input);
			return v;
		}
		case MINUS:
			return -prim(input);
		case PLUS:
			return prim(input);
		default:
			cerr << "error" << endl;
			exit(1);
	}
}

int64_t term(istream* input) {
	int64_t left = prim(input);
	while (true) {
		switch (curr_tok) {
			case MUL:
				left *= prim(input);
				break;
			case DIV:
				if (int64_t d = prim(input)) {
					left /= d;
					break;
				}
				else {
					cerr << "error" << endl;
					exit(1);
				}
			default:
				return left;
		}
	}
}

int64_t expr(istream* input) {
	int64_t left = term(input);
	while(true) {
		switch (curr_tok) {
		case PLUS:
			left += term(input);
			break;
		case MINUS:
			left -= term(input);
			break;
		default:
			return left;
		}
	}
}

int main(int argc, char* argv[]) {
	istream* input = nullptr;
	if (argc == 2) 
		input = new istringstream(argv[1]);
	else {
		cerr << "error" << endl;
		exit(1);
	}
	while (*input) {
		if (curr_tok == END) 
			break;
		cout << expr(input) << endl;
	}
	delete input;
	return 0;
}