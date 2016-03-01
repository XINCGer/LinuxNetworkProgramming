#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>

int main(int argc,const char **argv){
	struct hostent *hp;
	char **p;
	char str[128];
	if(argc != 2){
		printf("cnashucuowu!\n");
		exit(1);
	}
	hp = gethostbyname(argv[1]);
	if (hp == NULL){
		printf("host information not found!\n");
		exit(2);
	}
	printf("hostname :%s\n",hp->h_name);
	printf("alias:");
	char **q;
	for(q = hp->h_aliases;*q != NULL;q++){
		printf("%s \t",*q );
	}
	printf("\n");
	printf("addrlist:");
	for(q = hp->h_addr_list;*q != NULL;q++){
		printf("%s\t",inet_ntop(hp->h_addrtype,*q,str,sizeof(str)) );
	}
	printf("\n");
	return 0;
}