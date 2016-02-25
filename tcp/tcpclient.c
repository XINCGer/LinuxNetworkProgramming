/*       TcpClient.c
     copyright@赵顺 2015/09/21
 */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

int main(){
	int sockfd;
	char wrbuf[100];
	char ip[20];
	int port;
	int revlen;
	char rebuf[100];
	struct sockaddr_in server;

	/*---------------------socket---------------------*/
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1){
		perror("socket error\n");
		exit(1);
	}

	/*---------------------connect--------------------*/
	printf("Please input the ip:\n");
	scanf("%s",ip);
	printf("Please input the port:\n");
	scanf("%d",&port);
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_aton(ip,&server.sin_addr);
	if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))== -1){
		perror("connect() error\n");
		exit(1);
	}

	/*-----------------------read and write------------------*/
	while(1){
	bzero(wrbuf,sizeof(wrbuf));
	bzero(rebuf,sizeof(rebuf));	
	printf("Please input the info:\n");
	scanf("%s",wrbuf);
	if((memcmp("bye",wrbuf,3))== 0){
		write(sockfd,wrbuf,strlen(wrbuf));
		printf("Bye-bye then close the connect...\n");
		break;
	}
	//printf("%s\n",wrbuf);
	write(sockfd,wrbuf,strlen(wrbuf));
	revlen = read(sockfd,rebuf,sizeof(rebuf));
	rebuf[revlen] = '\0';
	printf("The info from server is: %s\n",rebuf);
	}
	/*------------------------close--------------------------*/
	close(sockfd);

	return 0;
}
