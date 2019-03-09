all: compile run0 run1 run2 run3 run4 run5 run6 run7 run8 run9 run10 run11 run12 clear
compile:
	g++ -Wall -O2 magma.cpp -o magma -std=c++11
	g++ -Wall -O2 strtobin.cpp -o stb -std=c++11

run0:
	./magma -h

run1:  #ebc gost12
	echo ffeeddccbbaa99887766554433221100f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff > tochange.txt
	./stb < tochange.txt > key.bin
	echo fedcba9876543210 > tochange.txt
	./stb < tochange.txt > in.bin
	echo 4ee901e5c2d8ca3d > tochange.txt
	./stb < tochange.txt > exp.bin
	./magma -e --ecb -k key.bin -i in.bin -o out.bin
	hd exp.bin
	hd out.bin

run2: #ebc e
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41 > tochange.txt
	./stb < tochange.txt > in.bin
	echo 2b073f0494f372a0de70e715d3556e4811d8d9e9eacfbc1e7c68260996c67efb > tochange.txt
	./stb < tochange.txt > exp.bin
	./magma -e --ecb -k key.bin < in.bin > out.bin #check without -i -o
	hd exp.bin
	hd out.bin

run3: #ebc d
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41 > tochange.txt
	./stb < tochange.txt > exp.bin
	./magma -d --ecb -k key.bin -i out.bin -o out1.bin
	hd exp.bin
	hd out1.bin

run4: #ctr e
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41 > tochange.txt
	./stb < tochange.txt > in.bin
	echo 4e98110c97b7b93c3e250d93d6e85d69136d868807b2dbef568eb680ab52a12d  > tochange.txt
	./stb < tochange.txt > exp.bin
	echo 12345678 > tochange.txt
	./stb < tochange.txt > iv.bin
	./magma -e --ctr -i in.bin -o out.bin -v iv.bin -k key.bin
	hd exp.bin
	hd out.bin

run5: #ctr d
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41  > tochange.txt
	./stb < tochange.txt > exp.bin
	echo 12345678 > tochange.txt
	./stb < tochange.txt > iv.bin
	./magma -d --ctr -i out.bin -o out1.bin -v iv.bin -k key.bin
	hd exp.bin
	hd out1.bin

run6: #ofb e
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41 > tochange.txt
	./stb < tochange.txt > in.bin
	echo db37e0e266903c830d46644c1f9a089ca0f83062430e327ec824efb8bd4fdb05  > tochange.txt
	./stb < tochange.txt > exp.bin
	echo 1234567890abcdef234567890abcdef1 > tochange.txt
	./stb < tochange.txt > iv.bin
	./magma -e --ofb -i in.bin -o out.bin -k key.bin -v iv.bin
	hd exp.bin
	hd out.bin

run7: #ofb d
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41  > tochange.txt
	./stb < tochange.txt > exp.bin
	./magma -d --ofb -i out.bin -o out1.bin -k key.bin -v iv.bin
	hd exp.bin
	hd out1.bin
 
run8: #cbc e
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41 > tochange.txt
	./stb < tochange.txt > in.bin
	echo 96d1b05eea683919aff76129abb937b95058b4a1c4bc001920b78b1a7cd7e667  > tochange.txt
	./stb < tochange.txt > exp.bin
	echo 1234567890abcdef234567890abcdef134567890abcdef12 > tochange.txt
	./stb < tochange.txt > iv.bin
	./magma -e --cbc -i in.bin -o out.bin -v iv.bin -k key.bin
	hd exp.bin
	hd out.bin

run9: #cbc d
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41  > tochange.txt
	./stb < tochange.txt > exp.bin
	./magma -d --cbc -i out.bin -o out1.bin -v iv.bin -k key.bin
	hd exp.bin
	hd out1.bin

run10: #cfb e
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41 > tochange.txt
	./stb < tochange.txt > in.bin
	echo db37e0e266903c830d46644c1f9a089c24bdd2035315d38bbcc0321421075505  > tochange.txt
	./stb < tochange.txt > exp.bin
	echo 1234567890abcdef234567890abcdef1 > tochange.txt
	./stb < tochange.txt > iv.bin
	./magma -e --cfb -i in.bin -o out.bin -v iv.bin -k key.bin
	hd exp.bin
	hd out.bin

run11: #cfb d
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41  > tochange.txt
	./stb < tochange.txt > exp.bin
	./magma -d --cfb -i out.bin -o out1.bin -v iv.bin -k key.bin
	hd exp.bin
	hd out1.bin

run12: #mac
	echo 92def06b3c130a59db54c704f8189d204a98fb2e67a8024c8912409b17b57e41 > tochange.txt
	./stb < tochange.txt > in.bin
	echo 154e7210  > tochange.txt
	./stb < tochange.txt > exp.bin
	./magma --mac -i in.bin -o out.bin -k key.bin
	hd exp.bin
	hd out.bin	

clear:
	rm exp.bin
	rm out.bin
	rm out1.bin
	rm magma
	rm in.bin
	rm iv.bin
	rm key.bin
	rm stb
	rm tochange.txt