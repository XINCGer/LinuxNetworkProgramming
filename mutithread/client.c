#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "clientmsg.h"

struct ARG{
	int sockfd;
	struct CLIENTMSG clientMsg;
};

void *func(void *arg);
void process_cli(int sockfd,struct CLIENTMSG clientMsg);
int main(){
	int sockfd;
	char ip[20];
	int port;
	pthread_t tid;
	struct sockaddr_in server;
	struct CLIENTMSG clientMsgSend;
	struct ARG *arg;
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
	recv(sockfd,&clientMsgSend,sizeof(clientMsgSend),0);
	if(clientMsgSend.OP == OK){
		//创建一个线程
		arg = (struct ARG *)malloc(sizeof(struct ARG));
		arg->sockfd = sockfd;
		pthread_create(&tid,NULL,func,(void *)arg);
		//主线程
		printf("Please input the username:\n");
		scanf("%s",clientMsgSend.username);
		clientMsgSend.OP = USER;
		send(sockfd,&clientMsgSend,sizeof(clientMsgSend),0);
		while(1){
			clientMsgSend.OP = MSG;
			scanf("%s",clientMsgSend.buf);
			if(strcmp("bye",clientMsgSend.buf) == 0){
				clientMsgSend.OP = EXIT;
				send(sockfd,&clientMsgSend,sizeof(clientMsgSend),0);
				break;
			}
			send(sockfd,&clientMsgSend,sizeof(clientMsgSend),0);
		}
		pthread_cancel(tid);
	} 
	else{
		printf("以达到最大连接数！\n");
	}
	/*------------------------close--------------------------*/
	close(sockfd);

	return 0;
}


void *func(void *arg){
	struct ARG  *info;
	info = (struct ARG *)arg;
	process_cli(info->sockfd,info->clientMsg);
	free(arg);
	pthread_exit(NULL);
}
void process_cli(int sockfd,struct CLIENTMSG clientMsg){
	int len;
	while(1){
				bzero(&clientMsg,sizeof(clientMsg));
				len =recv(sockfd,&clientMsg,sizeof(clientMsg),0);
				if(len > 0){
					if(clientMsg.OP ==USER){
						printf("the user %s is login.\n",clientMsg.username );
					}
					else if(clientMsg.OP == EXIT){
						printf("the user %s is logout.\n",clientMsg.username);
					}
					else if(clientMsg.OP == MSG){
						printf("%s: %s\n",clientMsg.username,clientMsg.buf );
					}
				}	
	}
}