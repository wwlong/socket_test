#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 12345
#define SERVADDR "fe80::5ce2:3aba:6384:f0ab/64"
#define MESSAGE "hi there"


int main(void)
{
	struct addrinfo hints = {0};
	struct addrinfo *res;
	struct sockaddr_in6 v;
    struct sockaddr_in6 server_addr;
	int get_err;
	int sockfd;
	char buffer[1024];
    int srvlen = sizeof(server_addr);
    int i ;
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
#if 0	
	get_err = getaddrinfo(SERVADDR, PORT, &hints, &res);
	if(get_err)
	{
		fprintf(stderr, "getaddrinfo:%s\n", gai_strerror(get_err));
		return 1;
	}
#endif
    memset(&v, 0, sizeof(v));
    //printf("[%s] -- [%d] ++\r\n", __FUNCTION__, __LINE__);
	v.sin6_family = AF_INET6;
    //printf("[%s] -- [%d] ++\r\n", __FUNCTION__, __LINE__);
	v.sin6_port = htons(PORT);
    //printf("[%s] -- [%d] ++\r\n", __FUNCTION__, __LINE__);
	v.sin6_flowinfo = 0;
    printf("[%s] -- [%d] ++\r\n", __FUNCTION__, __LINE__);
	v.sin6_scope_id = if_nametoindex(SERVADDR);
	//inet_pton(AF_INET6,argv[2],&v.sin_addr.s_addr);
	socklen_t len = sizeof(v);
    inet_pton(AF_INET6,SERVADDR,&v.sin6_addr.__in6_u);

	sockfd = socket(v.sin6_family, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		perror("socket");
		return 1;
	}
	if(sendto(sockfd, MESSAGE, sizeof(MESSAGE), 0, &v, len) < 0)
	{
		perror("sento failed");
		exit(4);
	}
    memset(&server_addr, 0, sizeof(server_addr));
    if (recvfrom(sockfd, buffer, 1024, 0,
		 (struct sockaddr *)&server_addr,
		 &srvlen) < 0) {
      perror("recvfrom failed");
      exit(4);
    } 
    else {
        printf("buffer : %s\r\n", buffer);
        printf("server_addr.sin6_port : %d\r\n", server_addr.sin6_port);
        printf("server_addr.sin6_addr.s6_addr : ");
        for(i = 0;i < 16;i ++) {
            printf("%2x", server_addr.sin6_addr.s6_addr[i]);
            if((i+1)%4 == 0) {
                printf(" : ");
            }
        }
        printf("\r\n");
        printf("server_addr.sin6_scope_id : %d\r\n", server_addr.sin6_scope_id);
    }

	close(sockfd);

	return 0;
}
