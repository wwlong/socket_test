#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <net/if.h>
#include <sys/time.h>
#define DNS_PORT 53
//#define SERVADDR "fe80::5ce2:3aba:6384:f0ab/64" //主机IP
#define SERVADDR "2001:67c:27e4:15::6411"   //dns64 srv 500ms
//#define SERVADDR "2001:67c:27e4::64"   //dns64 srv 不通
//#define SERVADDR "2001:67c:27e4:15::64"   //dns64 srv 1500ms
//#define SERVADDR "2001:67c:27e4::46"   //dns64 srv 不通
//#define SERVADDR "2001:67c:27e4:641::5bef:6015"  //dns64 srv不通

#define MESSAGE "hi there"
// DNS header structure
#pragma pack(push, 1)
struct DNS_HEADER {
    unsigned    short id;           // identification number

    unsigned    char rd     : 1;    // recursion desired
    unsigned    char tc     : 1;    // truncated message
    unsigned    char aa     : 1;    // authoritive answer
    unsigned    char opcode : 4;    // purpose of message
    unsigned    char qr     : 1;    // query/response flag

    unsigned    char rcode  : 4;    // response code
    unsigned    char cd     : 1;    // checking disabled
    unsigned    char ad     : 1;    // authenticated data
    unsigned    char z      : 1;    // its z! reserved
    unsigned    char ra     : 1;    // recursion available

    unsigned    short q_count;      // number of question entries
    unsigned    short ans_count;    // number of answer entries
    unsigned    short auth_count;   // number of authority entries
    unsigned    short add_count;    // number of resource entries
};

// Constant sized fields of query structure
struct QUESTION {
    unsigned short qtype;
    unsigned short qclass;
};

// Constant sized fields of the resource record structure
struct  R_DATA {
    unsigned short type;
    unsigned short _class;
    unsigned int   ttl;
    unsigned short data_len;
};
#pragma pack(pop)

// Pointers to resource record contents
struct RES_RECORD {
    unsigned char*  name;
    struct R_DATA*  resource;
    unsigned char*  rdata;
};

// Structure of a Query
typedef struct {
    unsigned char*       name;
    struct QUESTION*     ques;
} QUERY;

int main(void)
{
    struct sockaddr_in6 v;
    struct sockaddr_in6 server_addr;
    int get_err;
    int sockfd;
    char buffer[1024];
    char recv_buf[65536];
    int srvlen = sizeof(server_addr);
    int i ;
    int ret;
    memset(&v, 0, sizeof(v));
    v.sin6_family = AF_INET6;
    v.sin6_port = htons(DNS_PORT);
    v.sin6_flowinfo = 0;
    printf("[%s] -- [%d] ++\r\n", __FUNCTION__, __LINE__);
    v.sin6_scope_id = if_nametoindex(SERVADDR);
    socklen_t len = sizeof(v);
    inet_pton(AF_INET6,SERVADDR,&v.sin6_addr.__in6_u);

    sockfd = socket(v.sin6_family, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("socket");
        return 1;
    }
    struct DNS_HEADER *query, answer;
    memset(buffer, 0, sizeof(buffer));
    query = (struct DNS_HEADER*)buffer;
    query->id = rand()%65536;
    query->rd = 1;
    query->tc = 0;
    query->aa = 0;
    query->opcode = 0;
    query->qr = 0;
    query->rcode = 0;
    query->z = 0;
    query->ra = 0;
    query->q_count = 0;
    query->ans_count = 0;
    query->auth_count = 0;
    query->add_count = 0;

   struct QUESTION*  qinfo = NULL;
   struct QUERY *question;
    char *qname = "www.google.com";
    char *name = (char *)&buffer[sizeof(struct DNS_HEADER)];
    memcpy(name, qname, strlen(qname));
    
    qinfo = (struct QUESTION*)(buffer + sizeof(struct DNS_HEADER) + strlen(qname));
    qinfo->qtype = htons(0x1c);  //查询 ipv6 address
    qinfo->qclass = htons(1);  // its internet
    
    struct timeval start,end;
    gettimeofday(&start, NULL);
    unsigned long send_packlen = sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION);

    if(sendto(sockfd, buffer, send_packlen, 0, (struct sockaddr *)&v, len) < 0)
    {
        perror("sento failed");
        exit(4);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    ret = recvfrom(sockfd, recv_buf, 1024, 0,
                (struct sockaddr *)&server_addr,
                &srvlen) ;
    if(ret < 0) {
        printf("[%s] -- [%d] -- recvfrom failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    else {
        
        gettimeofday(&end, NULL);

        printf("dns time : %ld ms\r\n", (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000);
        //printf("buffer : %s\r\n", buffer);
        printf("ret : %d\r\nbuffer : \r\n", ret);
        for(i = 0;i < ret;i ++) {
            printf("%02x", recv_buf[i]);
            if((i + 1) % 8 == 0) {
                printf("\r\n");
            }
        }
        printf("\r\n");
        printf("server_addr.sin6_port : %d\r\n", server_addr.sin6_port);
        printf("server_addr.sin6_addr.s6_addr : ");
        for(i = 0;i < 16;i ++) {
            printf("%02x", server_addr.sin6_addr.s6_addr[i]);
            if((i+1)%2 == 0) {
                printf(":");
            }
        }
        printf("\r\n");
        printf("server_addr.sin6_scope_id : %d\r\n", server_addr.sin6_scope_id);

        //先解析header
        
    }

    close(sockfd);

    return 0;
}
