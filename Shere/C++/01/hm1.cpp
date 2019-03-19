#include <iostream>
#include <math.h>
#include "numbers.dat"

using namespace std;

bool issimple(int a){
	if (a > 1) {
		for (int i = 2; i <= sqrt(a); i++)
			if (a % i == 0)
				return false;
		return true;
	}
	else
		return false;
}

int binfind(int target, const int arr[], const int size, bool first) {  // return index
	int st = 0, end = size;
	while (1) {
		if (st <= end) {
			if (arr[(end+st)/2] < target)
				st = (st+end)/2 + 1;
			else if (arr[(st+end)/2] > target)
				end = (st + end)/2 - 1;
			else {
				st = end = (st + end)/2;
				break;
			}
		}
		else {
			return -1;
		}
	}
	if (arr[st] == target) {
		if (first) {
			while(arr[st] == target)
				st--;
			return st + 1;
		}
		else {
			while(arr[end] == target)
				end++;
			return end - 1;
		}
	}
	else 
		return -1;
}

int main(int argc, char * argv[]){
	if ( (argc < 3) || (argc % 2 == 0) )
		return 255;
	for (int i = 1; i < argc; i = i + 2) {
		int n = atoi(argv[i]), m = atoi(argv[i+1]);
		int c = 0;
		n = binfind(n, Data, Size, true);
		m = binfind(m, Data, Size, false);
		if (n != -1 && m != -1){
			for (int j = n; j <= m; j++)
				if (issimple(Data[j]))
					c++;
		}
		cout << c << endl; 
	}
	return 0;
}