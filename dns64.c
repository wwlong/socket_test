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
#define SOCKET_MAX_IP_COUNT (20)
// DNS header structure
// Pointers to resource record contents
struct RES_RECORD {
    unsigned char*  name;
    struct R_DATA*  resource;
    unsigned char*  rdata;
};

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


// Structure of a Query
typedef struct {
    unsigned char*       name;
    struct QUESTION*     ques;
} QUERY;

int min(int a, int b) {
    if(a > b) {
        return b;
    }
    else {
        return a;
    }
}
unsigned char* ReadName(unsigned char* _reader, unsigned char* _buffer, int* _count) {
    unsigned char* name;
    unsigned int p = 0, jumped = 0, offset;
    const unsigned int INIT_SIZE = 256, INCREMENT = 64;
    int timesForRealloc = 0;
    int i , j;

    *_count = 1;
    name   = (unsigned char*)malloc(INIT_SIZE);

    if (NULL == name) {
        
        return NULL;
    }

    name[0] = '\0';

    // read the names in 3www6google3com format
    while (*_reader != 0) {
        if (*_reader >= 192) {  // 192 = 11000000 ,如果该字节前两位bit为11，则表示使用的是地址偏移来表示name
            offset = (*_reader) * 256 + *(_reader + 1) - 49152;  // 49152 = 11000000 00000000  计算相对于报文起始地址的偏移字节数，即去除两位为11的bit，剩下的14位表示的值
            _reader = _buffer + offset - 1;
            jumped = 1;  // we have jumped to another location so counting wont go up!
        } else
            name[p++] = *_reader;

        _reader = _reader + 1;

        if (jumped == 0) *_count = *_count + 1;  // if we have not jumped to another location then we can count up

        if (*_count >= (int)(INIT_SIZE + INCREMENT * timesForRealloc)) {
            timesForRealloc++;

            unsigned char* more_name = NULL;
            more_name = (unsigned char*)realloc(name, (INIT_SIZE + INCREMENT * timesForRealloc));

            if (NULL == more_name) {
             
                free(name);
                return NULL;
            }

            name = more_name;
        }
    }

    name[p] = '\0';  // string complete

    if (jumped == 1) *_count = *_count + 1;  // number of steps we actually moved forward in the packet

    // now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int)strlen((const char*)name); i++) {
        p = name[i];

        for (j = 0; j < (int)p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }

        name[i] = '.';
    }

    name[i - 1] = '\0';  // remove the last dot
    return name;
}

// this will convert www.google.com to 3www6google3com
void ChangetoDnsNameFormat(unsigned char* _qname, char *_hostname) {
    int lock = 0 , i;
    char host[1024];
    memset(host, 0, sizeof(1024));
    memcpy(host, _hostname, strlen((const char *)_hostname));
    memcpy(host+strlen((const char *)_hostname), ".", 1);
    printf("before : %s\r\n", host);
    for (i = 0; i < (int)strlen((const char *)host); i++) {
        if (host[i] == '.') {
            *_qname++ = i - lock;

            for (; lock < i; lock++) {
                *_qname++ = host[lock];
            }

            lock++;
        }
    }
    *_qname++ = '\0';

    printf("after : %s\r\n", _qname);
}


//函数原型声明
void PrepareDnsQueryPacket(unsigned char* _buf, struct DNS_HEADER* _dns, unsigned char* _qname, char * _host) {
    struct QUESTION*  qinfo = NULL;
    // Set the DNS structure to standard queries
    _dns->id = 1875;
    _dns->qr = 0;      // This is a query
    _dns->opcode = 0;  // This is a standard query
    _dns->aa = 0;      // Not Authoritative
    _dns->tc = 0;      // This message is not truncated
    _dns->rd = 1;      // Recursion Desired
    _dns->ra = 0;      // Recursion not available!
    _dns->z  = 0;
    _dns->ad = 0;
    _dns->cd = 0;
    _dns->rcode = 0;
    _dns->q_count = htons(1);   // we have only 1 question
    _dns->ans_count  = 0;
    _dns->auth_count = 0;
    _dns->add_count  = 0;
    // point to the query portion
    _qname = (unsigned char*)&_buf[sizeof(struct DNS_HEADER)];
    ChangetoDnsNameFormat(_qname, _host);  // 将传入的域名host转换为标准的DNS报文可用的格式，存入qname中
    qinfo = (struct QUESTION*)&_buf[sizeof(struct DNS_HEADER) + (strlen((const char*)_qname) + 1)];  // fill it

    //qinfo->qtype = htons(0x1c);  //只查询 ipv4 address
    qinfo->qtype = htons(0x1c);  //查询 ipv6 address
    qinfo->qclass = htons(1);  // its internet
}
void ReadRecvAnswer(unsigned char* _buf, struct DNS_HEADER* _dns, unsigned char* _reader, struct RES_RECORD* _answers) {
    // reading answers
    int i, j, stop = 0;
    int answer_count = min(SOCKET_MAX_IP_COUNT, (int)ntohs(_dns->ans_count));

    for (i = 0; i < answer_count; i++) {
        _answers[i].name = ReadName(_reader, _buf, &stop);
        _reader = _reader + stop;

        _answers[i].resource = (struct R_DATA*)(_reader);
        _reader = _reader + sizeof(struct R_DATA);//指针偏移

       // if (ntohs(_answers[i].resource->type) == 1) {  // if its an ipv4 address
         if (ntohs(_answers[i].resource->type) == 0x1c) {  // if its an ipv6 address
           _answers[i].rdata = (unsigned char*)malloc(ntohs(_answers[i].resource->data_len));
          
            if (NULL == _answers[i].rdata) 
            {
                return;
            }

            for (j = 0 ; j < ntohs(_answers[i].resource->data_len) ; j++)
                { _answers[i].rdata[j] = _reader[j];
                  printf("%02x",_reader[j]);
                  if((j+1)%2 == 0 &&j <= 14) {
                    printf(":");
                  }
             }
              printf("\n");
            _answers[i].rdata[ntohs(_answers[i].resource->data_len)] = '\0';
            _reader = _reader + ntohs(_answers[i].resource->data_len);
        } else {
            _answers[i].rdata = ReadName(_reader, _buf, &stop);
            _reader = _reader + stop;
        }
    }
}

int main(void)
{
    struct sockaddr_in6 v;
    struct sockaddr_in6 server_addr;
    int get_err;
    int sockfd;
    const unsigned int BUF_LEN = 65536;
    unsigned char send_buf[65536] = {0};
    unsigned char recv_buf[65536] = {0};
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
    struct DNS_HEADER *query, *answer;
    memset(send_buf, 0, sizeof(send_buf));

    char *_host = "www.baidu.com";
    struct DNS_HEADER* dns = (struct DNS_HEADER*)send_buf;
    unsigned char* qname = (unsigned char*)&send_buf[sizeof(struct DNS_HEADER)];
    PrepareDnsQueryPacket(send_buf, dns, qname, (char*)_host);

    struct timeval start,end;
    gettimeofday(&start, NULL);
    unsigned long send_packlen = sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION);

    if(sendto(sockfd, send_buf, send_packlen, 0, (struct sockaddr *)&v, len) < 0)
    {
        perror("sento failed");
        exit(4);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    ret = recvfrom(sockfd, recv_buf, 65536, 0,
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
        for(i = 0;i < 1024;i ++) {
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
        //解析header
        answer = (struct DNS_HEADER *)recv_buf;
        printf("answer-> id: %d\r\n", answer-> id);
        printf("answer-> rd : %d\r\n", answer-> rd);
        printf("answer-> tc : %d\r\n",answer-> tc);
        printf("answer-> aa : %d\r\n", answer-> aa);
        printf("answer-> opcode : %d\r\n", answer-> opcode );
        printf("answer-> qr : %d\r\n", answer-> qr );
        printf("answer-> rcode : %d\r\n", answer-> rcode );
        printf("answer-> cd : %d\r\n", answer-> cd);
        printf("answer-> ad : %d\r\n", answer-> ad );
        printf("answer-> z : %d\r\n", answer-> z);
        printf("answer-> ra : %d\r\n", answer-> ra);
        printf("answer-> q_count : %d\r\n", answer-> q_count);
        printf("answer-> ans_count : %d\r\n", answer-> ans_count);
        printf("answer-> auth_count : %d\r\n", answer-> auth_count);
        printf("answer-> add_count : %d\r\n", answer-> add_count);

        for(i = sizeof(struct DNS_HEADER);i < ret - sizeof(struct DNS_HEADER);i ++) {
            printf("%02x ", recv_buf[i]);
            if((i+1)%8 == 0) {
                printf("\r\n");
            }
        }
        printf("\r\n");

    }
    struct RES_RECORD answers[SOCKET_MAX_IP_COUNT];  // the replies from the DNS server
    memset(answers, 0, sizeof(struct RES_RECORD)*SOCKET_MAX_IP_COUNT);

    unsigned char* reader = &recv_buf[sizeof(struct DNS_HEADER) + (strlen((const char*)qname) + 1) + sizeof(struct QUESTION)];
    dns = (struct DNS_HEADER*)recv_buf;   // 指向recv_buf的header

    ReadRecvAnswer(recv_buf, dns, reader, answers);

    close(sockfd);

    return 0;
}
