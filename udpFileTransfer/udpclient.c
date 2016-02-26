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
	struct protocol sentbuf;
	struct protocol recvbuf;
	int num;
	char ip[20];
	int port;
	int choice;
	char filename[100];
/*------------------------函数声明区------------------*/
void ShowMenu();
void DownLoad();
void UpLoad();
void ShutDown();


int main() {

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

	/*--------------------recvfrom and sendto----------*/
	bzero(&server,sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(ip);
	addrlen = sizeof(server);

	/*--------------------调试信息---------------------------
	recvbuf.command = DOWNLOAD;
	recvbuf.no =0;
	printf("input the message:\n");
	scanf("%s",recvbuf.buf);
	recvbuf.len = strlen(recvbuf.buf);
	sendto(socketfd,&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&server,sizeof(server)); */


	while (1){
	ShowMenu();
	scanf("%d",&choice);
	if(choice == SHUTDOWN){
		ShutDown();
		break;
	}
	else if(choice == DOWNLOAD){
		DownLoad();
	}
	else if(choice == UPLOAD){
		UpLoad();
	}
	else{
		printf("Please make a right choice!\n");
	}
} 

	/*-----------------close------------------------*/
	close(socketfd);
	return 0;
}


void ShowMenu(){
	printf("=====================================\n");
	printf("         Please make a choice:       \n");
	printf("         0:  ||shutdown||            \n");
	printf("         1:  ||download||            \n");
	printf("         2:  || upload ||            \n");
	printf("=====================================\n");
}
void DownLoad(){
	bzero(&recvbuf,sizeof(recvbuf));
	bzero(&sentbuf,sizeof(sentbuf));
	bzero(filename,sizeof(filename));
	printf("Please input the file name:\n");
	scanf("%s",sentbuf.buf);
	sentbuf.command = DOWNLOAD;
	sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&server,sizeof(server));
	bcopy(sentbuf.buf,filename,strlen(sentbuf.buf));
	recvfrom(socketfd,&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&server,&addrlen);
	if(recvbuf.command == YES){
		printf("YES!\n");
		int choice_1;
		printf("Press 5 to start  transmission!\n");
		scanf("%d",&choice_1);
		if(choice_1 == START){
			sentbuf.command = START;
			sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&server,sizeof(server));
			int no =0;
			int fd =open(filename,O_CREAT | O_TRUNC |O_WRONLY,0644);
			if(fd < 0){
				perror("create file error\n");
				exit(1);
			}
			bzero(&recvbuf,sizeof(recvbuf));
			while((num = recvfrom(socketfd,&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&server,&addrlen)) >0){
				if(recvbuf.command == CONTENT){
					if(no == recvbuf.no){
					write(fd,recvbuf.buf,recvbuf.len);
					//printf("kkk%s\n",recvbuf.buf );
					bzero(&recvbuf,sizeof(recvbuf));
				}
				no++;
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
	}
	else if(recvbuf.command ==NO){
		perror("NO such file on server!\n");
	}
	else{
		perror("recvbuf.command error\n");
		exit(1);
	}
}
void UpLoad(){
	bzero(&recvbuf,sizeof(recvbuf));
	bzero(&sentbuf,sizeof(sentbuf));
	bzero(filename,sizeof(filename));
	printf("Please input the file name:\n");
	scanf("%s",sentbuf.buf);
	sentbuf.command = UPLOAD;
	sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&server,sizeof(server));
	bcopy(sentbuf.buf,filename,strlen(sentbuf.buf));
	//判断要传输的文件是否存在
	int fd;
	fd = open(filename,O_RDONLY);
	if(fd < 0){
		perror("The file you want to trans is not exist!\n");
		exit(1);
	}
	recvfrom(socketfd,&recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&server,&addrlen);
	if(recvbuf.command == START){
			int no =0;
			while((num = read(fd,sentbuf.buf,INFOLEN)) >0){
				sentbuf.no = no;
				sentbuf.command = CONTENT;
				sentbuf.len = strlen(sentbuf.buf);
				sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&server,sizeof(server));
				no++;
				bzero(&sentbuf,sizeof(sentbuf));
			}
			bzero(&sentbuf,sizeof(sentbuf));
			sentbuf.command = END;
			sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&server,sizeof(server));
	}
	else if(recvbuf.command == NO){
		printf("not transmission\n");
	}
	else {
		perror("error! wrong choice!\n");
	}
}

void ShutDown(){
	printf("client is shutdown now!\n");
	bzero(&sentbuf,sizeof(sentbuf));
	sentbuf.command == SHUTDOWN;
	sendto(socketfd,&sentbuf,sizeof(sentbuf),0,(struct sockaddr *)&server,sizeof(server));
}