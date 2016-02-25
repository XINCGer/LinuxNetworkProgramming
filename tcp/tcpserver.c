/*     TcpServer.c  
     copyright@赵顺 2015/09/21
 */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int main(){

	struct sockaddr_in server;
	struct sockaddr_in client;
	int listenfd,connetfd;
	char ip[20];
	int port;
	int addrlen;
	char rebuf[100];
	char wrbuf[100];
	char tmp[100];
	int revlen;
	/*---------------------socket-------------------*/
	if((listenfd = socket(AF_INET,SOCK_STREAM,0))== -1){
		perror("socket() error\n");
		exit(1);
	}

	/*----------------------IO-----------------------*/
	printf("Please input the ip:\n");
	scanf("%s",ip);
	printf("Please input the port:\n");
	scanf("%d",&port);

	/*---------------------bind----------------------*/
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip);
	if(bind(listenfd,(struct sockaddr *)&server,sizeof(server))== -1){
		perror("bind() error\n");
		exit(1);
	}

	/*----------------------listen-------------------*/
	if (listen(listenfd,5)== -1){
		perror("listen() error\n");
		exit(1);
	}

	/*----------------------accept------------------*/
	addrlen = sizeof(client);
	if((connetfd = accept(listenfd,(struct sockaddr *)&client,&addrlen))== -1){
		perror("accept() error\n");
		exit(1);
	}
	/*---------------------show client---------------*/
	printf("connect successful!\n");
	printf("the client ip is %s,port is %d\n",inet_ntoa(client.sin_addr),ntohs(port));

	/*----------------------read and write----------*/
	int serial = 0;
	while(1){
	bzero(rebuf,sizeof(rebuf));
	revlen = read(connetfd,rebuf,sizeof(rebuf));
	if((memcmp("bye",rebuf,3))== 0){
		printf("Bye-bye then close the connect...\n");
		break;
	}
	bzero(wrbuf,sizeof(wrbuf));
	bzero(tmp,sizeof(tmp));
	sprintf(tmp,"%d",serial);
	strcat(tmp,rebuf);
	bcopy(tmp,wrbuf,strlen(tmp));
	write(connetfd,wrbuf,sizeof(wrbuf));
	rebuf[revlen] = '\0';
	printf("the info from client is:%s\n",rebuf);
	serial++;
	}

	/*----------------------close-------------------*/
	close(connetfd);
	close(listenfd);

	return 0;
}
