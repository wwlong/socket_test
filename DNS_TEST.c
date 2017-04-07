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
/********************************************************
 *  1.  创建socket
 *  %
 * ******************************************************/
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

#define PORT 8000
#define IPADDR "192.168.3.215"
int main()
{
    int sock_fd;
    struct sockaddr_in servaddr;
    char *message = "message from client";
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
    
    fd_set write_set;
    struct timeval timeout;
    fd_set read_set;
    int ret;
    FD_ZERO(&write_set);
    FD_SET(sock_fd, &write_set);
    FD_ZERO(&read_set);
    FD_SET(sock_fd, &read_set);
    while(1) {
        //send DNS 查询信息
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
        //recv response
        
        timeout.tv_sec = 1;
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
            
        }
    }
}
