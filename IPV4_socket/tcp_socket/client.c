/**********************************************************
 *   Author        :     wangwenlong
 *   Last modified :     2017-04-06 20:20
 *   Filename      :     client.c
 *   Description   :     
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
#include <arpa/inet.h>
#include <fcntl.h>
//#define PORT 8080
#define SRV_IP "192.168.3.215"
//#define SRV_IP "192.168.2.48"
#define PORT 8080
int test_base_api() {
    //建立TCP
    // socket
    {
        int sock_fd;
        struct sockaddr_in server;
        int ret ;
        uint32_t retry_times = 0;
        fd_set wset;
        struct timeval timeout;
        int32_t connect_error = -1;
        int32_t opt_len = 4;
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(socket < 0) {
            printf("[%s] -- [%d] -- socket create failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
#if 0
        if (-1 == fcntl(sock_fd, F_SETFL, O_NONBLOCK | fcntl(sock_fd, F_GETFL, 0))) {
            close(sock_fd);
            printf("[%s] -- [%d] -- fcntl failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
#endif 
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(SRV_IP);
        server.sin_port = htons(PORT);
        memset(&timeout, 0, sizeof(timeout));
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        while(1) {
            ret = connect(sock_fd, (struct sockaddr*)&server, sizeof(server));
            if(ret < 0) {
                printf("dana_tcp_connect connect failed\r\n");
                printf("dana_tcp_connect errno: %d %s\r\n", errno, strerror(errno));
                if(errno == EAGAIN && retry_times ++ < 3) {
                    usleep(30*1000);
                    continue;
                }
                else if(errno == EINPROGRESS) {
                    printf("dana_tcp_connect errno == EINPROGRESS \r\n");
                    FD_ZERO(&wset);
                    FD_SET(sock_fd, &wset);

                    ret = select(sock_fd, NULL, &wset, NULL, &timeout);
                    if(ret < 0) {
                        printf("dana_tcp_connet select failed\r\n");
                        return -1;
                    }
                    else if(0 == ret ){
                        printf("dana_tcp_connect select timeout\r\n");
                        break;
                    }
                    else {
                        //select success 
                        if(FD_ISSET(sock_fd, &wset)) {
                            ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &connect_error, &opt_len);
                            if(-1 == ret) {
                                printf("%s getsockopt failed %d\r\n", __FUNCTION__, __LINE__);
                                printf("dana_tcp_connect errno: %d %s\r\n", errno, strerror(errno));
                            }
                            printf("dana_tcp_connect connect_error: %d %s\r\n", connect_error, strerror(connect_error));
                            printf("dana_tcp_connect errno: %d %s\r\n", errno, strerror(errno));
                            if(EINTR == connect_error || ETIMEDOUT == connect_error) {
                                continue;
                            }
                        }
                        else {
                            printf("%s -- %d -- FD_ISSET failed\r\n", __FUNCTION__,__LINE__);
                        }
                    }
                } 
            } 
            else {
                printf("connect success\r\n");
                break;
            }

        }
        ret = select(sock_fd, NULL, &wset, NULL, &timeout);
        if(ret < 0) {
            printf("dana_tcp_connet select failed\r\n");
            return -1;
        }
        else if(0 == ret ){
            printf("dana_tcp_connect select timeout\r\n");
            break;
        }
        else {
            //select success 
            if(FD_ISSET(sock_fd, &wset)) {
                ret = getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &connect_error, &opt_len);
                if(-1 == ret) {
                    printf("%s getsockopt failed %d\r\n", __FUNCTION__, __LINE__);
                    printf("dana_tcp_connect errno: %d %s\r\n", errno, strerror(errno));
                }
                printf("dana_tcp_connect connect_error: %d %s\r\n", connect_error, strerror(connect_error));
                printf("dana_tcp_connect errno: %d %s\r\n", errno, strerror(errno));
                if(EINTR == connect_error || ETIMEDOUT == connect_error) {
                    continue;
                }
            }
            else {
                printf("%s -- %d -- FD_ISSET failed\r\n", __FUNCTION__,__LINE__);
            }
        }

    }
}

int main()
{
test_base_api();
while(1);
    int sock_fd;
    struct sockaddr_in server;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket < 0) {
        printf("[%s] -- [%d] -- socket create failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SRV_IP);
    server.sin_port = htons(PORT);

    //connet to remote server
    if(connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("[%s] -- [%d] -- connet to remote server failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    //使用select实现非阻塞socket
    int ret;
    int recv_len;
    char buffer[1024];
    int buffer_len = 1024;
    fd_set write_set,read_set;
    struct timeval timeout;
    char *send_str = "hello,I am client";
    while(1) {
        //read data from socket
#if 1
        FD_ZERO(&read_set);
        FD_SET(sock_fd , &read_set);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        ret = select(sock_fd + 1, &read_set, NULL, NULL, &timeout);
        if(ret < 0) {
            printf("[%s] -- [%d] -- select failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        else if(ret == 0) {
            printf("[%s] -- [%d] -- select timeout\r\n", __FUNCTION__, __LINE__);
        }
        else {
            printf("data from server is avaiable now\r\n");
            //开始recv from server
            while(1) {
                if(FD_ISSET(sock_fd, &read_set)) {
                    memset(buffer, 0, sizeof(buffer));
                    recv_len = recv(sock_fd, buffer, 3, 0);
                    if(-1 == recv_len && (EINTR == errno || EINPROGRESS == errno || EAGAIN == errno)) {
                        printf("data not recv all : %s\r\n",buffer);
                        memset(buffer, 0, sizeof(buffer));
                        continue;
                    }
                    else if(recv_len >= 1){
                        printf("recv data : %s\r\ndata_len : %d\r\n", buffer, recv_len);
                        memset(buffer, 0, sizeof(buffer));
                        if((EINTR == errno || EINPROGRESS == errno || EAGAIN == errno)) {
                            printf("(EINTR == errno || EINPROGRESS == errno || EAGAIN == errno)\r\n");
                        }
                        //perror("error\r\n");
                        break;
                    }
                    else {
                        printf("[%s] -- [%d] -- recv failed\r\n", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
        }

#endif 
        //write data to socket
        FD_ZERO(&write_set);
        FD_SET(sock_fd, &write_set);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        ret = select(sock_fd + 1, NULL, &write_set, NULL, &timeout);
        if(ret < 0) {
            printf("[%s] -- [%d] -- select failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        else if(ret == 0) {
            printf("[%s] -- [%d] -- select timeout\r\n", __FUNCTION__, __LINE__);
        }
        else {
            printf("send data to server\r\n");
            //开始recv from server
            while(1) {
                if(FD_ISSET(sock_fd, &write_set)) {
                    recv_len = send(sock_fd, send_str, strlen(send_str), 0);
                    if(-1 == recv_len && (EINTR == errno || EINPROGRESS == errno || EAGAIN == errno)) {
                        continue;
                    }
                    else if(recv_len >= 1){
                        break;
                    }
                    else {
                        printf("[%s] -- [%d] -- send failed\r\n", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
        }
    } 
}
