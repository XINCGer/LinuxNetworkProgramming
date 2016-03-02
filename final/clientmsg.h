//CLIENTMSG between server and client
#ifndef _clientmsg
#define _clientmsg

//USER MSG EXIT for OP of CLIENTMSG
#define EXIT -1
#define USER 1
#define OK 3
#define PUBLIC 4
#define PRIVATE 5
#define OVERNAME 6

#ifndef CMSGLEN
#define CMSGLEN 100
#endif

struct ONLINELIST
{
	int stat;
	char username[20];
};

struct CLIENTMSG{
	int OP;
	int index;
	struct ONLINELIST m_list[5];
	char username[20];
	char buf[CMSGLEN];
};

#endif
