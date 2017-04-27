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
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFFER_LEN 65536
#define flag_dbg(fmt...)   \
    do {\
        printf("[%s] -- [%d]: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)

void *connection_handler(void *socket_fd) 
{
    pthread_detach(pthread_self());
    int ret;
    int recv_len;
    int send_len;
    char buffer[BUFFER_LEN];
    int buffer_len = BUFFER_LEN;
    fd_set write_set,read_set;
    struct timeval timeout;
    char *send_str = "hello,I am server";
    int sock_fd = *(int *)socket_fd;
    FILE *fp = fopen("test_video.264", "w+");
    if(NULL == fp) {
        printf("fopen failed\r\n");
        return NULL;
    }
    timeout.tv_sec = 100;
    timeout.tv_usec = 0;
    FD_ZERO(&read_set);
    FD_SET(sock_fd , &read_set);
    FD_ZERO(&write_set);
    FD_SET(sock_fd, &write_set);

    while(1) {
        sleep(1);
        //read data from socket
        while(1) {
            ret = select(sock_fd + 1, &read_set, NULL, NULL, &timeout);
            flag_dbg("timeout.tv_sec : %ld\r\ntimeut.tv_usec : %ld\r\n", timeout.tv_sec ,timeout.tv_usec);

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
                if(FD_ISSET(sock_fd, &read_set)) {
                    memset(buffer, 0, BUFFER_LEN);
                    flag_dbg("errno : %d -- %s\r\n", errno, strerror(errno));
                    recv_len = recv(sock_fd, buffer, BUFFER_LEN, 0);
                    printf("recv_len : %d\r\n", recv_len);
                    if(-1 == recv_len) {
                        if(EAGAIN == errno || EWOULDBLOCK == errno) {
                            printf("data not recv all \r\n");
                            continue;
                        }
                        else {
                            flag_dbg("recv failed\r\n");
                            break;
                            //return ;
                        }
                    }
                    else if(recv_len >= 1){
                        printf("recv data :data_len : %d\r\n", recv_len);
                        //fwrite(buffer, recv_len, 1, fp);
                        break;
                    }
                    else {
                        printf("[%s] -- [%d] -- recv failed\r\n", __FUNCTION__, __LINE__);
                        break;
                    }
                }
            }

        }
#if 1
        //write data to socket
        while(1) {
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
                if(FD_ISSET(sock_fd, &write_set)) {
                    send_len = send(sock_fd, send_str, strlen(send_str) , 0);
                    flag_dbg("send_len : %d errno : %d\r\n", send_len, errno);
                    if(-1 == send_len) {
                        if(EAGAIN == errno || EWOULDBLOCK == errno) {
                            continue;
                        }
                        else {
                            break;
                        }
                    }
                    else {

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
    int32_t ret;
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
    int32_t opt = 1;
    ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if(ret < 0) {
        printf("setsockopt failed\r\n");
    }
 
    if(bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("[%s] -- [%d] -- bind failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
//    ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
//    if(ret < 0) {
//        printf("setsockopt failed\r\n");
//    }

    //listen 
    listen(sock_fd, 10);
    //accept
    int len = sizeof(struct sockaddr_in);
    memset(&client, 0, sizeof(client));
    while((new_sock = accept(sock_fd, (struct sockaddr *)&client, (socklen_t *)&len)))
    {
        printf("accept a new  client\r\n");
        printf("ip : %s\r\n", inet_ntoa(client.sin_addr));
        printf("port : %d\r\n", htons(client.sin_port));
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
