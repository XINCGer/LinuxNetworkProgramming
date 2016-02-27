#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include "clientmsg.h"

int main(){
	int sockfd;
	char ip[20];
	int port;
	int status;
	pid_t pid;
	struct sockaddr_in server;
	struct CLIENTMSG clientMsg;

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
	recv(sockfd,&clientMsg,sizeof(clientMsg),0);
	if(clientMsg.OP == OK){
		int len;
		pid = fork();
		if(pid == 0){
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
			exit(EXIT_SUCCESS); 
		}
		else if(pid > 0){
			printf("Please input the username:\n");
			scanf("%s",clientMsg.username);
			clientMsg.OP = USER;
			send(sockfd,&clientMsg,sizeof(clientMsg),0);
			while(1){
				clientMsg.OP = MSG;
				scanf("%s",clientMsg.buf);
				if(strcmp("bye",clientMsg.buf) == 0){
					clientMsg.OP = EXIT;
					send(sockfd,&clientMsg,sizeof(clientMsg),0);
					break;
				}
				send(sockfd,&clientMsg,sizeof(clientMsg),0);

			}
			kill(pid,SIGKILL);
			waitpid(pid,&status,WNOHANG);
		}
		else{
			perror("fork error!\n");
		}
	}
	else{
		printf("以达到最大连接数！\n");
	}
	/*------------------------close--------------------------*/
	close(sockfd);

	return 0;
}
