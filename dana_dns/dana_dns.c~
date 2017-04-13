#include "dana_dns.h"

struct dns_result* dana_getaddrinfo_syn(
	char *domain,  //域名或者IP，
	int32_t addrtype, //AF_INET or AF_INET6
	int32_t timeout_ms  //超时时间 ms
	)
{
    char dnsserver[64]="2001:67c:27e4:15::64";
    char ip[16];
    struct dns_result   *ip_ret = NULL;
    int i;
    memset(ip, 0, sizeof(ip));
    if(AF_INET == addrtype) {
        if(false == danasrvip_query(domain, timeout_ms * 1000, ip, sizeof(ip))) {
            printf("[%s] -- [%d] -- danasrvip_query failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        else {   //danasrvip_query success
            ip_ret =  (struct dns_result *)malloc(sizeof(struct dns_result));
            if(NULL == ip_ret) {
                printf("[%s] -- [%d] -- malloc failed\r\n", __FUNCTION__, __LINE__);
                return NULL;
            }
            memset(ip_ret, 0, sizeof(struct dns_result));
            ip_ret->ipaddr_list = (char **)malloc(strlen(ip)+1);
            if(NULL == ip_ret->ipaddr_list) {
                printf("[%s] -- [%d] -- malloc failed\r\n");
                return -1;
            }

            ip_ret->addrtype = AF_INET;
            ip_ret->num = 1;
            memset(ip_ret->ipaddr_list, 0, strlen(ip)+1);
            strncpy(ip_ret->ipadd_list[0], ip, strlen(ip));
            return ip_ret;
        }
    }
    else if(AF_INET6 == addrtype){
        struct socket_ipinfo_t *test_ipv6=(struct socket_ipinfo_t *)malloc(sizeof(socket_ipinfo_t));
        if(0 != getaddrinfo_v6(domain, test_ipv6,3000/*ms*/)) {
            printf("[%s] -- [%d] -- getaddrinfo_v6 failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        else {  //getaddrinfo_v6成功
            ip_ret =  (struct dns_result *)malloc(sizeof(struct dns_result));
            if(NULL == ip_ret) {
                printf("[%s] -- [%d] -- malloc failed\r\n", __FUNCTION__, __LINE__);
                return NULL;
            }
            memset(ip_ret, 0, sizeof(struct dns_result));
            ip_ret->ipaddr_list = (char **)malloc((test_ipv6->size)*(strlen(ip)+1));
            if(NULL == ip_ret->ipaddr_list) {
                printf("[%s] -- [%d] -- malloc failed\r\n");
                return -1;
            }
            ip_ret->addrtype = AF_INET6;
            ip_ret->num = test_ipv6->size;
            for(i = 0;i < ip_ret->num; i ++) {
                char *ip_str = inet_ntop(AF_INET6, &test_ipv6->v6_addr[i].s6_addr, v6_ip, sizeof(v6_ip));
                strncpy(ip_ret->ipaddr_list[i], ip_str, strlen(ip_str));
            }
            return ip_ret;
        }
    }
    else {
        printf("[%s] -- [%d] -- addrtype invalid\r\n", __FUNCTION__, __LINE__);
        //error
        return NULL;
    }
}
//bool danasrvip_query(const char *domain_name, const uint32_t timeout_usec, char *ip, const size_t ip_size)
struct addrinfo_asyn{
    char domain[256];  //域名或者IP
	int32_t addrtype;//AF_INET or AF_INET6
    int32_t (*get_dns_result)(struct dns_result *dns_ret);
};

typedef int32_t(*get_dns_result)(struct dns_result *);

int32_t get_dns_result(struct dns_result *dns_ret)
{
    if(NULL == dns_ret) {
        printf("[%s] -- [%d] -- invalid param\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    int i = 0;
    printf("addrtype : %d\r\n", dns_ret->addrtype);
    printf("num : %d\r\n", dns_ret->num);
    for(i = 0;i < dns_ret->num;i ++) {
       printf("ip : %s\r\nlen : %d\r\n", dns_ret->ipaddr_list.ip, dns_ret->ipaddr_list.size); 
    }
   
   return 0; 
}
void *th_dana_getaddrinfo_asyn(void *data) {
    pthread_detach(pthread_self());
    struct addrinfo_asyn *addrinfo= (struct addrinfo_asyn*)data; 
    if(NULL == addrinfo){
        return NULL;
    }

    char *domain = addrinfo->domain;
    int32_t addrtype = addrinfo->addrtype;

    char ip[16];
    struct dns_result   *ip_ret = NULL;
    int i;
    memset(ip, 0, sizeof(ip));
    if(AF_INET == addrtype) {
        if(false == danasrvip_query(domain, timeout_ms * 1000, ip, sizeof(ip))) {
            printf("[%s] -- [%d] -- danasrvip_query failed\r\n", __FUNCTION__, __LINE__);

            return NULL;
        }
        else {   //danasrvip_query success
            ip_ret =  (struct dns_result *)malloc(sizeof(struct dns_result));
            if(NULL == ip_ret) {
                printf("[%s] -- [%d] -- malloc failed\r\n", __FUNCTION__, __LINE__);
                return NULL;
            }
            memset(ip_ret, 0, sizeof(struct dns_result));
            ip_ret->ipaddr_list = (char **)malloc(strlen(ip)+1);
            if(NULL == ip_ret->ipaddr_list) {
                printf("[%s] -- [%d] -- malloc failed\r\n");

                return NULL;

            }

            ip_ret->addrtype = AF_INET;
            ip_ret->num = 1;
            memset(ip_ret->ipaddr_list, 0, strlen(ip)+1);
            strncpy(ip_ret->ipadd_list[0], ip, strlen(ip));
            addfinfo->get_dns_result(ip_ret);
            return NULL;
            //return ip_ret;
        }
    }
    else if(AF_INET6 == addrtype){
        struct socket_ipinfo_t *test_ipv6=(struct socket_ipinfo_t *)malloc(sizeof(socket_ipinfo_t));
        if(0 != getaddrinfo_v6(domain, test_ipv6,3000/*ms*/)) {
            printf("[%s] -- [%d] -- getaddrinfo_v6 failed\r\n", __FUNCTION__, __LINE__);
            return -1;
        }
        else {  //getaddrinfo_v6成功
            ip_ret =  (struct dns_result *)malloc(sizeof(struct dns_result));
            if(NULL == ip_ret) {
                printf("[%s] -- [%d] -- malloc failed\r\n", __FUNCTION__, __LINE__);
                return NULL;
            }
            memset(ip_ret, 0, sizeof(struct dns_result));
            ip_ret->ipaddr_list = (char **)malloc((test_ipv6->size)*(strlen(ip)+1));
            if(NULL == ip_ret->ipaddr_list) {
                printf("[%s] -- [%d] -- malloc failed\r\n");

                return NULL;

            }
            ip_ret->addrtype = AF_INET6;
            ip_ret->num = test_ipv6->size;
            for(i = 0;i < ip_ret->num; i ++) {
                char *ip_str = inet_ntop(AF_INET6, &test_ipv6->v6_addr[i].s6_addr, v6_ip, sizeof(v6_ip));
                strncpy(ip_ret->ipaddr_list[i], ip_str, strlen(ip_str));
            }

            addfinfo->get_dns_result(ip_ret);
            return NULL;
        }
    }
    else {
        printf("[%s] -- [%d] -- addrtype invalid\r\n", __FUNCTION__, __LINE__);
        //error
        return NULL;
    }
}

int32_t dana_getaddrinfo_asyn(
	char *domain,  //域名或者IP
	int32_t addrtype, //AF_INET or AF_INET6
	int (*get_dns_result)(struct dns_result *dns_ret)
)
{
    struct addrinfo_asyn addrinfo;
    memset(&addrinfo, 0, sizeof(addrinfo));
    strncpy(addrinfo.domain, domain, strlen(domain));
    addrinfo.addrtype = addrtype;
    addrinfo.get_dns_result = get_dns_result;

    pthread_t ipquery_t;
    pthread_attr_t ipquery_attr;
    pthread_attr_init(&ipquery_attr);
#define ASYNC_STACK_SIZE 5*1024
    void *async_stack = malloc(ASYNC_STACK_SIZE);
    if(NULL == async_stack) {
        printf("[%s] -- [%d] -- malloc failed\r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    memset(async_stack, 0, ASYNC_STACK_SIZE);
    pthread_attr_setstack(&ipquery_attr, async_stack, ASYNC_STACK_SIZE);
    if(0!= pthread_create(&ipquery_t, &ipquery_attr, th_dana_getaddrinfo_asyn, &addrinfo)){
        printf("[%s] -- [%d] -- pthread_create failed\r\n", __FUNCTION__, __LINE__);
        return -1; 
    }
    return 0;
}

