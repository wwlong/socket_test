/*
 * sock.h
 *
 *  Created on: 19-Mar-2017
 *      Author: Rahul Patel
 */

#ifndef SOCK_H_
#define SOCK_H_
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<net/if.h>
#define H_IPV4 0
#define H_IPV6 1
#define ADDR_4 "0.0.0.0"
#define ADDR_6 IN6ADDR_ANY_INIT
#define ADDR4_LOOP "127.0.0.1"
#define ADDR6_LOOP IN6ADDR_LOOPBACK_INIT


#endif /* SOCK_H_ */
