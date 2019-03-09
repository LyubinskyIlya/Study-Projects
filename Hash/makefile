all: compile run1 run2 run3 run4 run5 run6 print1 run7 run8 run9 run10 run11 run12 print2
compile:
	g++ -Wall -O2 hash.cpp -o g512sum -D test -std=c++11
	g++ -Wall -O2 hash.cpp -D g256 -D test -o g256sum -std=c++11

run1: 
	./g256sum M1 

run2: 
	./g256sum M2 

run3: 
	./g256sum M3 

run4: 
	./g256sum M4

run5: 
	./g256sum M5

run6: 
	./g256sum M6

print1:
	cat g256sum.result

run7: 
	./g512sum M1

run8: 
	./g512sum M2

run9: 
	./g512sum M3

run10: 
	./g512sum M4

run11: 
	./g512sum M5

run12: 
	./g512sum M6

print2:
	cat g512sum.result
