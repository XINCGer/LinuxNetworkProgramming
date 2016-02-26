#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "protocol.h"

/*-----------------------变量声明区------------------*/
	int socketfd;
	int addrlen;
	struct sockaddr_in server;
	struct sockaddr_in client;
	struct protocol sentbuf;
	struct protocol recvbuf;
	int num;
	char ip[20];
	int port;
	int choice;

int main(){

	/*-------------create UDP socket------------*/
	if((socketfd = socket(AF_INET,SOCK_DGRAM,0)) == -1){
		perror("socket error\n");
		exit(1);
	}

	/*-----------------IO-----------------------*/
	printf("Please input the ip:\n");
	scanf("%s",ip);
	printf("Please input the port:\n");
	scanf("%d",&port);

	/*-----------------bind----------------------*/
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip);
	if (bind(socketfd,(struct sockaddr *)&server,sizeof(server)) == -1){
		perror("bind error\n");
		exit(1);
	}

	/*---------------------调试信息------------
	addrlen = sizeof(client);
	recvfrom(socketfd,&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&client,&addrlen);
	num = strlen(recvbuf.buf);
	recvbuf.buf[num] = '\0';
	printf("command %d\n",recvbuf.command );
	printf("len %d\n",recvbuf.len );
	printf("no %d\n", recvbuf.no);
	printf("buf %s\n",recvbuf.buf ); */

	addrlen = sizeof(client);
	while(1){
	bzero(&recvbuf,sizeof(recvbuf));
	num =recvfrom(socketfd,&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&client,&addrlen);
	choice = recvbuf.command;
	if(choice == DOWNLOAD){
		char buf[100];
		int fd;
		fd = open((recvbuf.buf),O_RDONLY);
		if(fd <0){
			sentbuf.command = NO;
			sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&client,sizeof(client));
			printf("no such file!\n");
			exit(1);
		}
		else{
			sentbuf.command = YES;
			sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&client,sizeof(client));
			recvfrom(socketfd,&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&client,&addrlen);
			if(recvbuf.command == START){
				int no =0;
				while((num = read(fd,sentbuf.buf,INFOLEN)) >0){
					sentbuf.no = no;
					sentbuf.command = CONTENT;
					sentbuf.len = strlen(sentbuf.buf);
					sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&client,sizeof(client));
					no++;
					bzero(&sentbuf,sizeof(sentbuf));
				}
				bzero(&sentbuf,sizeof(sentbuf));
				sentbuf.command = END;
				sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&client,sizeof(client));
			}
		}
	}
	else if(choice == UPLOAD){
		printf("The client want to upload the file:  %s\n",recvbuf.buf);
		printf("Please choice start or no? \n");
		printf("5 :start,  4: no\n");
		scanf("%d",&sentbuf.command);
		sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&client,sizeof(client));
		if(sentbuf.command ==START){
			int no =0;
			int fd =open(recvbuf.buf,O_CREAT | O_TRUNC |O_WRONLY,0644);
			if(fd < 0){
				perror("create file error\n");
				exit(1);
			}
			bzero(&recvbuf,sizeof(recvbuf));
			while((num = recvfrom(socketfd,&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&server,&addrlen)) >0){
				if(recvbuf.command == CONTENT){
					if(no == recvbuf.no){
					write(fd,recvbuf.buf,recvbuf.len);
					printf("kkk%s\n",recvbuf.buf );
					bzero(&recvbuf,sizeof(recvbuf));
				}
				else{
					perror("The file no is not same.Some message is missed! error occured! \n");
					break;
				}
				}
				if(recvbuf.command == END){
					close(fd);
					printf("transmission is successful!\n");
					break;
				}
			}
		}
		else if(sentbuf.command == NO){
			printf("Not to trans the file\n");
		}
		else{
			perror("error! wrong choice!\n");
			exit(1);
		}

	}
	else if (recvbuf.command == SHUTDOWN){
		printf("Now the server is shutdown!\n");
		break;
	}
}
	
	/*----------------------close ----------*/
	close(socketfd);
	return 0;
}
