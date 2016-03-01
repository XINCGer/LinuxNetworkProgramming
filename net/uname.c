#include <stdio.h>
#include <sys/utsname.h>

int main(int argc,char **argv){
	struct utsname hostname;
	if(uname(&hostname) <0){
		exit(1);
	}
	printf("%s\t %s\t%s\t%s\t%s\n",hostname.sysname,hostname.nodename,hostname.release,hostname.version,hostname.machine );
	return 0;
}