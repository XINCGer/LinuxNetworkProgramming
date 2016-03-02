main:server.o client.o 
	gcc server.o -oserver -lpthread
	gcc client.o -oclient -lpthread
server.o:server.c clientmsg.h
	gcc -c server.c
client.o:client.c clientmsg.h
	gcc -c client.c
clean:
	rm -rf *.o
