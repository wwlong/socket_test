/**********************************************************
 *   Author        :     wangwenlong
 *   Last modified :     2017-04-20 17:55
 *   Filename      :     igmp_test.c
 *   Description   :     
 * *******************************************************/
#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
void igmp_test()
{
    char *ip = "234.5.6.7";
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        printf("[%s] -- [%d] -- socket failed\r\n", __FUNCTION__, __LINE__);
        return;
    } 
    struct ip_mreq groupaddr;
    memset(&groupaddr, 0, sizeof(groupaddr));
    groupaddr.imr_multiaddr.s_addr = inet_addr(ip);
    groupaddr.imr_interface.s_addr = htonl(INADDR_ANY); 
    
    if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&groupaddr, sizeof(groupaddr)) != 0) {
        printf("[%s] -- %d -- setsockopt failed\r\n", __FUNCTION__, __LINE__);
        return ;
    } 
    int a,b,c,d;
    char ip_str[17];
//    for(a = 224; a <= 239; a ++) {
//        for(b = 0;b <= 255;b ++) {
//            for(c = 0;c <= 255;c ++) {
//                for(d = 0; d <= 255;d ++) {
//                    memset(ip_str, 0, sizeof(ip_str));
//                    sprintf(ip_str, "%d.%d.%d.%d", a, b, c, d);
//                    printf("ip ; %s\r\n", ip_str);
//                    memset(&groupaddr, 0, sizeof(groupaddr));
//                    groupaddr.imr_multiaddr.s_addr = inet_addr(ip_str);
//                    groupaddr.imr_interface.s_addr = htonl(INADDR_ANY); 
//                    if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&groupaddr, sizeof(groupaddr)) != 0) {
//                        printf("[%s] -- %d -- setsockopt failed\r\n", __FUNCTION__, __LINE__);
//                        printf("setsockopt : %s\r\n", strerror(errno));
//                    }else {
//                        printf("ip ; %s\r\n", ip_str);
//                    }
//
//                }
//            }
//        }
//    } 
    printf("setsockopt IGMP success\r\n");
}

int main()
{
    igmp_test();
    return 0;
}
