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
void communicate_process(int m_index);
int IsOverName(char name[]);
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
		bzero(ent[i].username,sizeof(ent[i].username));
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
void communicate_process(int m_index){
	struct CLIENTMSG sendMsg;
	struct CLIENTMSG recvMsg;
	sendMsg.OP = OK;
	send(ent[m_index].sockfd,&sendMsg,sizeof(sendMsg),0);
	while(1){
		bzero(&sendMsg,sizeof(sendMsg));
		bzero(&recvMsg,sizeof(recvMsg));
		int len =recv(ent[m_index].sockfd,&recvMsg,sizeof(recvMsg),0);
		if(len > 0){
			if(recvMsg.OP == USER){
				printf("user %s login from ip:%s,port:%d\n",recvMsg.username,inet_ntoa(ent[m_index].client.sin_addr),ntohs(ent[m_index].client.sin_port) );
				if(IsOverName(recvMsg.username)){
					sendMsg.OP = OVERNAME;
					ent[m_index].stat =0;
					send(ent[m_index].sockfd,&sendMsg,sizeof(sendMsg),0);
					pthread_exit((void*)1);
				}
				bcopy(recvMsg.username,ent[m_index].username,strlen(recvMsg.username));
			}
			else if(recvMsg.OP == EXIT){
				printf("user %s is logout\n",recvMsg.username );
				sendMsg.OP = EXIT;
				ent[m_index].stat = 0;
				bzero(ent[m_index].username,sizeof(ent[m_index].username));
				int j;
				for(j =0;j<5;j++){
					sendMsg.m_list[j].stat = ent[j].stat;
					bcopy(ent[j].username,sendMsg.m_list[j].username,strlen(ent[j].username));
				}
				bcopy(recvMsg.username,sendMsg.username,strlen(recvMsg.username));
				int i;
				for(i=0;i<5;i++){
				 	if(ent[i].stat == 1){
				 		
				 		send(ent[i].sockfd,&sendMsg,sizeof(sendMsg),0);
				 	}
			 	}
				break;
			}
			sendMsg.OP = recvMsg.OP;
			int j;
			for(j =0;j<5;j++){
				sendMsg.m_list[j].stat = ent[j].stat;
				bcopy(ent[j].username,sendMsg.m_list[j].username,strlen(ent[j].username));
			}
			bcopy(recvMsg.username,sendMsg.username,strlen(recvMsg.username));
			bcopy(recvMsg.buf,sendMsg.buf,strlen(recvMsg.buf));
			if(recvMsg.OP == PRIVATE){
				printf("private\n");
				send(ent[recvMsg.index].sockfd,&sendMsg,sizeof(sendMsg),0);
			}
			if(recvMsg.OP == PUBLIC || recvMsg.OP == USER){
				printf("public\n");
				int i;
				for(i=0;i<5;i++){
				 	if(ent[i].stat == 1){
				 		send(ent[i].sockfd,&sendMsg,sizeof(sendMsg),0);
				 	}
			 	}
			}


		}
	}
}

int IsOverName(char name[]){
	int i;
	for(i=0;i<5;i++){
		if(strcmp(ent[i].username,name) == 0){
			return 1;
		}
	}
	return 0;
}
