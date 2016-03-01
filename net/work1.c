#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

int main(){
	struct servent *serv_p;
	int i;
	for (i = 1; i <= 100 ;i++){
		serv_p = getservbyport(htons(i),"tcp");

		if(serv_p == NULL){
			printf("error\n");
			//exit(1);
		}
		else{
			printf("TCP service for port:%d is: %s \n",i,serv_p->s_name);
		}
	}
}