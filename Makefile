target: 	sender recv


sender:		sender.o
		g++ -m64 -std=c++11 -Wall -o sender sender.o


recv: 		recv.o
		g++ -m64 -std=c++11 -Wall -o recv recv.o


sender.o:	sender.cpp
		g++ -c -m64 -std=c++11 -Wall -o sender.o sender.cpp


recv.o: 	recv.cpp
		g++ -c -m64 -std=c++11 -Wall -o recv.o recv.cpp


clean: 
		rm -rf *.O sender recv

