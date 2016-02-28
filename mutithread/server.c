#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <pthread.h>
#include "clientmsg.h"

struct Entity{
	int sockfd;
	char username[20];
	char buf[CMSGLEN];
	struct sockaddr_in client;
	int stat;
};

void *func(void *arg);
void communicate_process(int index);
struct Entity ent[5];

int main(){

	struct sockaddr_in server;
	struct sockaddr_in client;
	int listenfd,connetfd;
	char ip[20];
	int port;
	int addrlen;
	struct CLIENTMSG clientMsg;
	pthread_t tid;
	int *arg;
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
	int i;
	for(i=0;i<5;i++){
		ent[i].stat = 0;
	}
	while(1){
		addrlen = sizeof(client);
		if((connetfd = accept(listenfd,(struct sockaddr *)&client,&addrlen))== -1){
			perror("accept() error\n");
			exit(1);
		}
		int index = 5;
		for(i=0;i<5;i++){
			if(ent[i].stat == 0){
				index = i;
				break;
			}
		}
		if(index <= 4){	
			printf("connetfd:%d\n",connetfd );
			ent[index].client = client;
			ent[index].sockfd = connetfd;
			ent[index].stat = 1;
			arg = malloc(sizeof(int));
			*arg = index;
			pthread_create(&tid,NULL,func,(void *)arg);

		}
		else{
			bzero(&clientMsg,sizeof(clientMsg));
			clientMsg.OP = EXIT;
			send(connetfd,&clientMsg,sizeof(clientMsg),0);
			close(connetfd);
		}

	}

	/*----------------------close-------------------*/
	
	close(listenfd);

	return 0;
}


/*----------------------------函数实现区----------------------------*/
void *func(void *arg){
	int *info ;
	info = (int *)arg;
	communicate_process( *info);
	pthread_exit(NULL);
}
void communicate_process(int index){
	struct CLIENTMSG sendMsg;
	struct CLIENTMSG recvMsg;
	printf("sockfd:%d\n",ent[index].sockfd );
	sendMsg.OP = OK;
	send(ent[index].sockfd,&sendMsg,sizeof(sendMsg),0);

	while(1){
		bzero(&sendMsg,sizeof(sendMsg));
		bzero(&recvMsg,sizeof(recvMsg));
		int len =recv(ent[index].sockfd,&recvMsg,sizeof(recvMsg),0);
		if(len > 0){
			if(recvMsg.OP == USER){
				printf("user %s login from ip:%s,port:%d\n",recvMsg.username,inet_ntoa(ent[index].client.sin_addr),ntohs(ent[index].client.sin_port) );
				bcopy(recvMsg.username,ent[index].username,strlen(recvMsg.username));
				sendMsg.OP = USER;
			}
			else if(recvMsg.OP == EXIT){
				printf("user %s is logout\n",recvMsg.username );
				sendMsg.OP = EXIT;
				ent[index].stat = 0;
				int i;
				for(i=0;i<5;i++){
				 	if(ent[i].stat == 1){
				 		
				 		send(ent[i].sockfd,&sendMsg,sizeof(sendMsg),0);
				 	}
			 	}
				break;
			}
			else if(recvMsg.OP == MSG){
				sendMsg.OP = MSG;
			}
			bcopy(recvMsg.username,sendMsg.username,strlen(recvMsg.username));
			bcopy(recvMsg.buf,sendMsg.buf,strlen(recvMsg.buf));
			int i;
			for(i=0;i<5;i++){
			 	if(ent[i].stat == 1){
			 		printf("stat 1...\n");
			 		send(ent[i].sockfd,&sendMsg,sizeof(sendMsg),0);
			 	}
			 }
		}
		else{
			continue;
		}
	}
}
