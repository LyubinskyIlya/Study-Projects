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

void get_token(istringstream &input, Token_value &curr_tok, int64_t &number_value) {
	char ch;
	do {
		if (!input.get(ch)) {
			curr_tok = END;
			return;
		}
	} while (isspace(ch));
	switch (ch) {
		case 0:
			curr_tok = END;
			return;
		case MUL:
		case DIV:
		case PLUS:
		case MINUS:
			curr_tok = Token_value(ch);
			return;
		case NUM0: case NUM1: case NUM2: case NUM3: case NUM4:
		case NUM5: case NUM6: case NUM7: case NUM8: case NUM9:
			input.putback(ch);
			input >> number_value;
			curr_tok = NUMBER;
			return;
		default:
			throw runtime_error("error");
	}
}

int64_t prim(istringstream &input, Token_value &curr_tok, int64_t &number_value) {
	get_token(input, curr_tok, number_value);
	switch (curr_tok) {
		case NUMBER: {
			int64_t v = number_value;
			get_token(input, curr_tok, number_value);
			return v;
		}
		case MINUS:
			return -prim(input, curr_tok, number_value);
		case PLUS:
			return prim(input, curr_tok, number_value);
		default:
			throw runtime_error("error");
	}
}

int64_t term(istringstream &input, Token_value &curr_tok, int64_t &number_value) {
	int64_t left = prim(input, curr_tok, number_value);
	while (true) {
		switch (curr_tok) {
			case MUL:
				left *= prim(input, curr_tok, number_value);
				break;
			case DIV:
				if (int64_t d = prim(input, curr_tok, number_value)) {
					left /= d;
					break;
				}
				else {
					throw runtime_error("Div by 0");
				}
			default:
				return left;
		}
	}
}

int64_t expr(istringstream &input, Token_value &curr_tok, int64_t &number_value) {
	int64_t left = term(input, curr_tok, number_value);
	while(true) {
		switch (curr_tok) {
		case PLUS:
			left += term(input, curr_tok, number_value);
			break;
		case MINUS:
			left -= term(input, curr_tok, number_value);
			break;
		default:
			return left;
		}
	}
}

int main(int argc, char* argv[]) {
	Token_value curr_tok = NUMBER;
	int64_t number_value = 0;
	try {
		if (argc == 2) {
			istringstream input(argv[1]);
			cout << expr(input, curr_tok, number_value) << endl;
		}
		else 
			throw logic_error("Expected 2 args");
	}
	catch (logic_error &s) {
		cerr << s.what() << endl;
		return 1;
	}
	catch (runtime_error &s) {
		cerr << s.what() << endl;
		return 2;
	}
	return 0;
}
