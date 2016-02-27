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
#include "semaphore.h"
#include "servermsg.h"


void trans_process(int semid);
void communicate_process(int connetfd,int qid,struct sockaddr_in client);

int main(){

	struct sockaddr_in server;
	struct sockaddr_in client;
	int listenfd,connetfd;
	char ip[20];
	int port;
	int addrlen;
	struct CLIENTMSG clientMsg;
	int ret,status;
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

	//创建命名管道
	unlink("SERVER");
	mkfifo("SERVER",O_CREAT);
	int rd = open("SERVER",O_RDONLY|O_NONBLOCK);
	int semid;
	key_t k =  ftok(".",'b');
	semid = CreateSem(k,5);
	pid_t pid_1,pid_2;
	pid_1 = fork();
	if(pid_1 == 0){
		trans_process(semid);
		exit(0);
	}
	else if(pid_1 > 0){
		while(1){
			addrlen = sizeof(client);
			if((connetfd = accept(listenfd,(struct sockaddr *)&client,&addrlen))== -1){
			perror("accept() error\n");
			exit(1);
			}
			ret = Sem_P(semid);
			if(ret == 0){
				int qid;
				read(rd,&qid,sizeof(qid));
				//printf("qid1:%d\n",qid );
				pid_2 = fork();
				if (pid_2 > 0){
					close(connetfd);
					waitpid(pid_2,&status,WNOHANG);
					continue;
				}
				else if(pid_2 == 0){
					communicate_process(connetfd,qid,client);
					exit(0);
				}
				else {
					perror("the second fork error\n");
				}
			}
			else {
				clientMsg.OP = EXIT;
				send(connetfd,&clientMsg,sizeof(clientMsg),0);
				close(connetfd);
			}
			waitpid(pid_1,&status,WNOHANG);

		}
	}
	else {
		perror("first time fork error\n");
	}
	/*----------------------close-------------------*/
	close(connetfd);
	close(listenfd);

	return 0;
}


/*----------------------------函数实现区----------------------------*/
void trans_process(int semid){
	struct SERVERMSG ent[5];
	struct MESSAGE sendMsg;
	struct SERVERMSG msg;
	int i;
	for(i=0;i<5;i++){
		ent[i].stat = 0;
	}
	int wfd = open("SERVER",O_WRONLY|O_NONBLOCK);
	for(i=0;i<5;i++){
    	key_t key = ftok(".",(char)i+102);
    	ent[i].qid = msgget(key,IPC_CREAT);
    	write(wfd,&ent[i].qid,sizeof(ent[i].qid));
	}
	unlink("CLIENT");
	mkfifo("CLIENT",O_CREAT);
	int rfd = open("CLIENT",O_RDONLY|O_NONBLOCK);
	int len;
	while(1){
		bzero(&msg,sizeof(msg));
		len = read(rfd,&msg,sizeof(msg));
		//printf(" %d,%s ,%s\n",msg.OP,msg.username,msg.buf );
		//sleep(3);
		if(len > 0){
			if(msg.OP == USER){
				for(i=0;i<5;i++){
					if(ent[i].qid == msg.qid){
						bcopy(msg.username,ent[i].username,strlen(msg.username));
						ent[i].client = msg.client;
						ent[i].stat = 1;
						break;
					}
				}
			 }
			else if(msg.OP == EXIT){
				for(i=0;i<5;i++){
					if(ent[i].qid == msg.qid){
						ent[i].stat = 0;
						write(wfd,&ent[i].qid,sizeof(ent[i].qid));
						Sem_V(semid);
						break;
					}
				}
			 }
			 //bzero(&sendMsg,sizeof(sendMsg));
			 sendMsg.msg = msg;
			 for(i=0;i<5;i++){
			 	if(ent[i].stat == 1){
			 		printf("stat 1...\n");
			 		int m_len = sizeof(msg);
			 		int sta=msgsnd(ent[i].qid,&sendMsg,len,0);
			 		//printf("flag:%d\n",sta );
			 	}
			 }
		}
		else {
			continue;
		}
	} 

}

void communicate_process(int connetfd,int qid,struct sockaddr_in client){
	struct CLIENTMSG sendMsg;
	struct CLIENTMSG recvMsg;
	struct MESSAGE server_Msg;
	struct SERVERMSG client_sndMsg;
	struct SERVERMSG msg;
	int status;
	int wfd = open("CLIENT",O_WRONLY|O_NONBLOCK);
	pid_t pid;
	pid = fork();
	if(pid < 0){
		perror("communicate_process fork error\n");
	}
	else if (pid == 0){
		bzero(&sendMsg,sizeof(sendMsg));
		sendMsg.OP = OK;
		send(connetfd,&sendMsg,sizeof(sendMsg),0);
		while(1){
			int m_len = sizeof(msg);
			bzero(&server_Msg,sizeof(server_Msg));
			int sta=msgrcv(qid,&server_Msg,m_len,0,0);
			//printf("flag:%d\n",sta );
			//printf("send..%d,%s,%s\n",server_Msg.msg.OP,server_Msg.msg.username,server_Msg.msg.buf );
			bzero(&sendMsg,sizeof(sendMsg));
			bcopy(server_Msg.msg.username,sendMsg.username,strlen(server_Msg.msg.username));
			sendMsg.OP = server_Msg.msg.OP;
			bcopy(server_Msg.msg.buf,sendMsg.buf,strlen(server_Msg.msg.buf));
			//printf("send..%d,%s,%s\n",sendMsg.OP,sendMsg.username,sendMsg.buf );
			send(connetfd,&sendMsg,sizeof(sendMsg),0);
		}
	}
	else{
		while(1){
			bzero(&recvMsg,sizeof(recvMsg));
			int len =recv(connetfd,&recvMsg,sizeof(recvMsg),0);
			if(len > 0){
				if(recvMsg.OP == USER){
					printf("user %s login from ip:%s,port:%d\n",recvMsg.username,inet_ntoa(client.sin_addr),ntohs(client.sin_port) );
					client_sndMsg.OP = USER;
				}
				else if(recvMsg.OP == EXIT){
					printf("user %s is logout\n",recvMsg.username );
					client_sndMsg.OP = EXIT;
					write(wfd,&client_sndMsg,sizeof(client_sndMsg));
					break;
				}
				else if(recvMsg.OP == MSG){
					client_sndMsg.OP = MSG;
				}
				bzero(&client_sndMsg,sizeof(client_sndMsg));
				bcopy(recvMsg.username,client_sndMsg.username,strlen(recvMsg.username));
				bcopy(recvMsg.buf,client_sndMsg.buf,strlen(recvMsg.buf));
				client_sndMsg.client = client;
				//printf("qid2:%d\n",qid );
				client_sndMsg.qid = qid;
				client_sndMsg.OP = recvMsg.OP;
				write(wfd,&client_sndMsg,sizeof(client_sndMsg));
				
			}
			else{
				continue;
			}
		}
		kill(pid,SIGKILL);
		waitpid(pid,&status,WNOHANG);
		close(wfd);
		close(connetfd);
	}
}