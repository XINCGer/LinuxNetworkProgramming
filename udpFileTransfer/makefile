main:udpserver.o udpclient.o
	gcc -o udpserver udpserver.o 
	gcc -o udpclient udpclient.o 
udpserver.o:udpserver.c protocol.h 
	gcc -c udpserver.c  
udpclient.o: udpclient.c protocol.h 
	gcc -c udpclient.c 
clean:
	rm -rf *.o
