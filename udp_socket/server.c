/**********************************************************
 *   Author        :     wangwenlong
 *   Last modified :     2017-04-07 11:48
 *   Filename      :     server.c
 *   Description   :     
 * *******************************************************/

/*********************************************************
 *  udp server
 *  监听来自client的数据
 *  向客户端发送response
 * *******************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    int sock_fd;
    int ret;
    struct sockaddr_in srvaddr, client_addr;
    fd_set read_set, write_set;
    struct timeval timeout;
    int length;
    char buffer[1024];
    int buffer_len = 1024;
    int recv_len;
    int flags = 0;
    char *message = "server response";
    //socket
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0) {
        printf("[%s] -- [%d] -- socket failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    //name srv socket
    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(PORT);
    srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock_fd, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) < 0) {
        printf("[%s] -- [%d] -- bind failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //select
    FD_ZERO(&read_set);
    FD_SET(sock_fd, &read_set);
    while(1) {
        //recv form client
#if 1
        printf("circle \r\n");
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        ret = select(sock_fd + 1, &read_set, NULL, NULL, &timeout);
        if(ret < 0) {
            printf("[%s] -- [%d] --select failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        else if(ret == 0) {
            printf("select timeout\r\n");
        }
        else {
            //read from sock_fd avaiable
            memset(&client_addr, 0, sizeof(client_addr));
            length = sizeof(client_addr);
            memset(buffer, 0, sizeof(buffer));
            recv_len = recvfrom(sock_fd, buffer, buffer_len, 0, (struct sockaddr *)&client_addr, (socklen_t *)&length);
            if(recv_len > 0) {
                printf("recvfrom IP : %s\r\n", inet_ntoa(client_addr.sin_addr));
                printf("recvfrom PORT : %d\r\n", htons(client_addr.sin_port));
                printf("message len : %d\r\nmessage : %s\r\n", recv_len, buffer);
                flags = 1;
            }
        }
#endif 
        if(1 == flags) {
            flags = 0;
#if 1
            //response to client
            timeout.tv_sec = 10;
            timeout.tv_usec = 0;
            FD_ZERO(&write_set);
            FD_SET(sock_fd, &write_set);

            ret = select(sock_fd+1, NULL, &write_set, NULL, &timeout);
            if(ret < 0) {
                printf("[%s] -- [%d] -- server sendto select failed\r\n", __FUNCTION__, __LINE__);
                return -1;
            }
            else if(ret == 0) {
                printf("select timeout \r\n");
            }
            else {  //表示可以发送数据了
                if(FD_ISSET(sock_fd, &write_set)) {
                    if(-1 == sendto(sock_fd, message, strlen(message), 0, (struct sockaddr *)&client_addr, sizeof(client_addr))){
                        if( EINTR == errno || EINPROGRESS == errno || EAGAIN == errno ) {
                            printf("EINTR == errno || EINPROGRESS == errno || EAGAIN == errno\r\n");
                            continue;
                        }
                        else {
                            printf("[%s] -- [%d] -- sendto failed\r\n", __FUNCTION__, __LINE__);
                        }
                    } 
                }

            }
#endif 

        }
    }
}
