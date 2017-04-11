/*
 * server.c
 *
 *  Created on: 19-Mar-2017
 *      Author: rp
	this is server for IPv6
 */
#include "sock.h"

int main(int argc,char *argv[])
{
	if(argc<2)
	{
		printf("usage : ./server <Port Number>");
		return 0;
	}
	int sfd,nsfd;
	char s[25];
#if H_IPV4
	sfd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in v;
	v.sin_family = AF_INET;
	v.sin_port = htons(atoi(argv[1]));
	v.sin_addr.s_addr = inet_addr(ADDR_4);
	socklen_t len = sizeof(v);
#elif H_IPV6
	sfd = socket(AF_INET6,SOCK_STREAM,0);
	struct sockaddr_in6 v;
	v.sin6_family = AF_INET6;
	v.sin6_port = htons(atoi(argv[1]));
	v.sin6_flowinfo = 0;
	v.sin6_addr = in6addr_any;
	socklen_t len = sizeof(v);
#endif
	if(bind(sfd,(struct sockaddr *)&v,len)<0)
	{
		perror("bind");
		return 0;
	}
	listen(sfd,5);
	nsfd=accept(sfd,(struct sockaddr *)&v,&len);
	memset(s,0,sizeof(s));
	read(nsfd,s,sizeof(s));
	printf("%s\n",s);
	return 0;
}
