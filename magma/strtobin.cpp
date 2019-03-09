#include <cstring>
#include <iostream>
using namespace std;

int main(int argc, char * argv[]){
	char s[256];
	cin >> s;
	string s1	;
	s1[2]='\0';
	for (unsigned int i=0; i<strlen(s); i=i+2)  {
		s1[0]=s[i];
		s1[1]=s[i+1];
		int8_t x = std::stoi(s1, nullptr, 16);
		cout << (char)x;
	}
	return 0;
}