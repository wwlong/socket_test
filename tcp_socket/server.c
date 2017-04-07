/**********************************************************
 *   Author        :     wangwenlong
 *   Last modified :     2017-04-06 20:02
 *   Filename      :     server.c
 *   Description   :     
 * *******************************************************/

/**********************************************************
 *  tcp server
 *
 * ********************************************************/

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
#define PORT 8080

void *connection_handler(void *socket_fd) 
{
    pthread_detach(pthread_self());
    int ret;
    int recv_len;
    char buffer[1024];
    int buffer_len = 1024;
    fd_set write_set,read_set;
    struct timeval timeout;
    char *send_str = "hello,I am server";
    int sock_fd = *(int *)socket_fd;
    while(1) {
        //read data from socket
        FD_ZERO(&read_set);
        FD_SET(sock_fd , &read_set);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        ret = select(sock_fd + 1, &read_set, NULL, NULL, &timeout);
        if(ret < 0) {
            printf("[%s] -- [%d] -- select failed\r\n", __FUNCTION__, __LINE__);
            return NULL;
        }
        else if(ret == 0) {
            printf("[%s] -- [%d] -- select timeout\r\n", __FUNCTION__, __LINE__);
        }
        else {
            printf("data from client is avaiable now\r\n");
            //开始recv from server
            while(1) {
                if(FD_ISSET(sock_fd, &read_set)) {
                    memset(buffer, 0, strlen(buffer));
                    recv_len = recv(sock_fd, buffer, 17, 0);
                    if(-1 == recv_len && (EINTR == errno || EINPROGRESS == errno || EAGAIN == errno)) {
                        printf("data not recv all : %s\r\n",buffer);
                        continue;
                    }
                    else if(recv_len >= 1){
                        printf("recv data : %s\r\ndata_len : %d\r\n", buffer, recv_len);
                        if((EINTR == errno || EINPROGRESS == errno || EAGAIN == errno)) {
                            printf("(EINTR == errno || EINPROGRESS == errno || EAGAIN == errno)\r\n");
                        }
                        break;
                    }
                    else {
                        printf("[%s] -- [%d] -- recv failed\r\n", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }
        }
#if 0
        //write data to socket
        FD_ZERO(&write_set);
        FD_SET(sock_fd, &write_set);
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        ret = select(sock_fd + 1, NULL, &write_set, NULL, &timeout);
        if(ret < 0) {
            printf("[%s] -- [%d] -- select failed\r\n", __FUNCTION__, __LINE__);
            return NULL;
        }
        else if(ret == 0) {
            printf("[%s] -- [%d] -- select timeout\r\n", __FUNCTION__, __LINE__);
        }
        else {
            printf("write to client \r\n");
            //开始recv from server
            while(1) {
                if(FD_ISSET(sock_fd, &write_set)) {
                    recv_len = send(sock_fd, send_str, strlen(send_str), 0);
                    if(-1 == recv_len && (EINTR == errno || EINPROGRESS == errno || EAGAIN == errno)) {
                        printf("data not recv all : %s\r\nrecv_len : %lu\r\n",buffer, strlen(buffer));
                        memset(buffer, 0, strlen(buffer));
                        continue;
                    }
                    else if(recv_len >= 1){
                        printf("recv data : %s\r\ndata_len : %d\r\n", buffer, recv_len);
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
    } 
}
int main()
{
    int sock_fd, new_sock;
    struct sockaddr_in server, client;
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0) {
        printf("[%s] -- [%d] -- socket create failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    //bind 
    if(bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("[%s] -- [%d] -- bind failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    //listen 
    listen(sock_fd, 10);
    //accept
    int len = sizeof(struct sockaddr_in);
    memset(&client, 0, sizeof(client));
    while((new_sock = accept(sock_fd, (struct sockaddr *)&client, (socklen_t *)&len)))
    {
        printf("accept a new  client\r\n");
        pthread_t sniffer_thread;
        if(0 != pthread_create(&sniffer_thread, NULL, connection_handler, (void *)&new_sock)) {
            printf("[%s] -- [%d] -- pthread_create sniffer_thread failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
    }
    if(new_sock < 0) {
        printf("[%s] -- [%d] -- accept failed\r\n", __FUNCTION__, __LINE__);\
        return -1;
    } 
    return 0;
}
