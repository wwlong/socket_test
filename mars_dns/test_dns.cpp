#include "dnsquery.h"
int main()
{
 struct socket_ipinfo_t *test_ipv6;
  char host[64]="conn-policy.ictun.com";
  //char dnsserver[64]="2001:67c:27e4::64";
  char dnsserver[64]="2001:67c:27e4:15::6411";
  char v6_ip[64] = {0};
  socket_gethostbyname(host, test_ipv6,3000/*ms*/,dnsserver);
  //for(int i=0;i<(test_ipv6->size);i++)
  //{
     //inet_ntop(AF_INET6, &test_ipv6->v6_addr[i], v6_ip, sizeof(v6_ip));
     //printf("%s\n:",v6_ip);
  //}
return 0;
}
