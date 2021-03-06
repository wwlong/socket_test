/**********************************************************
 *   Author        :     wangwenlong
 *   Last modified :     2017-04-06 10:22
 *   Filename      :     DNS_TEST.c
 *   Description   :     测试DNS
 * *******************************************************/
/* *******************************************************
 *  向google的DNS64发起DNS请求
 *  1.  本地为IPV4环境下请求
 *  2.  本地为IPV6环境下请求
 *  解析返回的IP地址
 * *******************************************************/

/*******************************************************
 *  UDP client
 *  向服务器请求数据
 *  接收来自服务器的响应
 * *****************************************************/
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define IPADDR "2001:67c:27e4:15::6411"

#ifndef DNS_P_DICTSIZE
#define DNS_P_DICTSIZE	16
#endif
#if defined __GNUC__
#define DNS_PRAGMA_EXTENSION __extension__
#else
#define DNS_PRAGMA_EXTENSION
#endif

struct dns_header {
		unsigned qid:16;

#if (defined BYTE_ORDER && BYTE_ORDER == BIG_ENDIAN) || (defined __sun && defined _BIG_ENDIAN)
		unsigned qr:1;
		unsigned opcode:4;
		unsigned aa:1;
		unsigned tc:1;
		unsigned rd:1;

		unsigned ra:1;
		unsigned unused:3;
		unsigned rcode:4;
#else
		unsigned rd:1;
		unsigned tc:1;
		unsigned aa:1;
		unsigned opcode:4;
		unsigned qr:1;

		unsigned rcode:4;
		unsigned unused:3;
		unsigned ra:1;
#endif

		unsigned qdcount:16;
		unsigned ancount:16;
		unsigned nscount:16;
		unsigned arcount:16;
}; /* struct dns_header */
struct dns_packet {
	unsigned short dict[DNS_P_DICTSIZE];
	struct dns_p_memo {
		struct dns_s_memo {
			unsigned short base, end;
		} qd, an, ns, ar;

		struct {
			unsigned short p;
			unsigned short maxudp;
			unsigned ttl;
		} opt;
	} memo;

	struct { struct dns_packet *cqe_next, *cqe_prev; } cqe;

	size_t size, end;

	int:16; /* tcp padding */

	DNS_PRAGMA_EXTENSION union {
		struct dns_header header;
		unsigned char data[1];
	};
}; /* struct dns_packet */


int main()
{
    int sock_fd;
    int sock_fd_recv;
    struct sockaddr_in servaddr, recvfrom_addr;
    char *message = "message from client";
    char buffer[1024];
    int buffer_len = 1024;
    int length, recv_len;
    //socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0) {
        printf("[%s] -- [%d] -- socket failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    } 
    //name it
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET6;
    servaddr.sin_addr.s_addr = inet_addr6(IPADDR);
    servaddr.sin_port = htons(PORT);

    fd_set write_set, read_set;
    struct timeval timeout;
    int ret;
    FD_ZERO(&write_set);
    FD_SET(sock_fd, &write_set);
    while(1) {
#if 1
        //send message
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        ret = select(sock_fd+1, NULL, &write_set, NULL, &timeout);
        if(ret < 0) {
            printf("[%s] -- [%d] -- select failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        else if(ret == 0) {
            printf("select timeout \r\n");
        }
        else {  //表示可以发送数据了
            if(FD_ISSET(sock_fd, &write_set)) {
                if(-1 == sendto(sock_fd, message, strlen(message), 0, (struct sockaddr *)&servaddr, sizeof(servaddr))){
                    if( EINTR == errno || EINPROGRESS == errno || EAGAIN == errno ) {
                        printf("EINTR == errno || EINPROGRESS == errno || EAGAIN == errno\r\n");
                        continue;
                    }
                    else {
                        printf("[%s] -- [%d] -- sendto failed\r\n", __FUNCTION__, __LINE__);
                    }
                }
                else {
                    //send success
                } 
            }

        }
#endif 
#if 1
        //recv response
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        FD_ZERO(&read_set);
        FD_SET(sock_fd, &read_set);


        ret = select(sock_fd + 1, &read_set, NULL, NULL, &timeout);
        if(ret < 0) {
            printf("[%s] -- [%d] -- select failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        else if(0 == ret) {
            printf("recv select timeout\r\n"); 
        }
        else {
            //recv from socket avaiable
            memset(&recvfrom_addr, 0, sizeof(recvfrom_addr));
            length = sizeof(recvfrom_addr);
            memset(buffer, 0, sizeof(buffer));
            recv_len = recvfrom(sock_fd, buffer, buffer_len, 0, (struct sockaddr *)&recvfrom_addr, (socklen_t *)&length);
            if(recv_len > 0) {
                printf("recvfrom IP : %s\r\n", inet_ntoa(recvfrom_addr.sin_addr));
                printf("recvfrom PORT : %d\r\n", htons(recvfrom_addr.sin_port));
                printf("message len : %d\r\nmessage : %s\r\n", recv_len, buffer);
            }
        }
#endif 

    }
}
