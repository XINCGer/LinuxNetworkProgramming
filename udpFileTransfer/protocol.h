#ifndef _protocol
#define _protocol

#define INFOLEN 1000
#define SHUTDOWN 0
#define DOWNLOAD 1
#define UPLOAD 2
#define YES 3
#define NO 4
#define START 5
#define END 6
#define CONTENT 7
#define OK 8

struct protocol
{
	int command;
	int len;  //length of buf
	int no;
	char buf[INFOLEN];
};
#endif

