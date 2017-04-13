#ifndef DANA_DNS_H_
#define DANA_DNS_H_
#include <stdint.h>
struct socket_ipinfo_t
{
    int  size;
    struct  in6_addr v6_addr[20];
}socket_ipinfo_t;


//同步API：
struct ip_list {
    char ip[40]; //IPV6 hex 表示最多16+7
    int32_t size;  
};
struct dns_result{
	int32_t addrtype; //AF_INET or AF_INET6
    int32_t num;
	struct ip_list *ipaddr_list;
};
struct dns_result* dana_getaddrinfo_syn(
	char *domain,  //域名或者IP，
	int32_t addrtype, //AF_INET or AF_INET6
	int32_t timeout_ms  //超时时间 ms
	)
	
//异步API：
//回调函数接口
int32_t get_dns_result(struct dns_result *);
int32_t dana_getaddrinfo_asyn(
	char *domain,  //域名或者IP
	int32_t addrtype, //AF_INET or AF_INET6
	int (*get_dns_result)(struct dns_result *dns_ret)
)

#endif 



