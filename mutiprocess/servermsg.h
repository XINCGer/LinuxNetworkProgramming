//SERVERMSG for communicate to translate
//MESSAGE for translate to communicate
#ifndef _servermsg
#define _servermsg

#include <netinet/in.h>
#include "clientmsg.h"


#ifndef CMSGLEN
#define CMSGLEN 100
#endif


struct SERVERMSG{
	int OP;
	char username[20];
	char buf[CMSGLEN];
	struct sockaddr_in client;
	int stat;
	int qid;
};

struct MESSAGE{
	long msgtype;
	struct SERVERMSG msg;
};

#endif
