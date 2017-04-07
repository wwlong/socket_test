/**********************************************************
 *   Author        :     wangwenlong
 *   Last modified :     2017-04-07 11:19
 *   Filename      :     client.c
 *   Description   :     
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
#define IPADDR "192.168.3.215"
int main()
{
    int sock_fd;
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
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(IPADDR);
    servaddr.sin_port = htons(PORT);

    fd_set write_set, read_set;
    struct timeval timeout;
    int ret;
    FD_ZERO(&write_set);
    FD_SET(sock_fd, &write_set);
    FD_ZERO(&read_set);
    FD_SET(sock_fd, &read_set);

    while(1) {
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
            }

        }
#if 0
        //recv response
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
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
