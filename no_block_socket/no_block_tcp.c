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
#include <stdlib.h>
//#define PORT 8080
#define SRV_IP "192.168.3.215"
//#define SRV_IP "192.168.2.48"
#define PORT 8080

void non_block()
{

}
int main()
{
    connect_w_to();
    return 0; 
}
