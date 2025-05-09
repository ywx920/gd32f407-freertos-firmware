#ifndef _LWIP_CLIENT_H_
#define _LWIP_CLIENT_H_

#include "main.h"
#include "lwipopts.h"

#if LWIP_DHCP  
void lwip_dhcp(void *thread_param);
#endif /* LWIP_DHCP */

void Tcp_client(void *thread_param);

void Udp_client(void *thread_param);

void UdpServer(void *thread_param);

void Tcp_server(void *thread_param);

void UdpSocket(void *thread_param);

#endif //_LWIP_CLIENT_H_
