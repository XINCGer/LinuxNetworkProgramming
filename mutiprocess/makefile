main:server.o client.o semaphore.o
	gcc server.o semaphore.o -oserver
	gcc client.o -oclient
server.o:server.c semaphore.h clientmsg.h servermsg.h
	gcc -c server.c
client.o:client.c clientmsg.h
	gcc -c client.c
semaphore.o:semaphore.h semaphore.c
	gcc -c semaphore.c
clean:
	rm -rf *.o
