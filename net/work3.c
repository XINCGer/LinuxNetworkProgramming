#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

int main(){
	struct servent *serv_p;
	char *serv_name[] = {"http","domain","ftp","smtp","pop3",NULL};
	char **p ;
	for(p =serv_name;*p != NULL;p++){
		serv_p = getservbyname(*p,"tcp");
		if(serv_p == NULL){
			printf("error\n");
		}
		else{
			printf("the service: %s use port :%d\n",*p,ntohs(serv_p->s_port) );
		}
	}

}