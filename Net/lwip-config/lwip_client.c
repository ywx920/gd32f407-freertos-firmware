#include "lwip_client.h"

#include "lwip/opt.h"
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include <lwip/sockets.h>

//#include "bsp_Control_xc6s.h"
//#include "myString.h"
//#include "bsp_adc.h"

//#include "bsp_iicBus.h"



#if LWIP_DHCP  
void lwip_dhcp(void *thread_param)
{
    printf("lwipclient Task ok\r\n");
    struct netconn *conn;
    int ret;
    ip4_addr_t ipaddr;

    uint8_t send_buf[]= "This is a TCP Client test...\n";

    printf("des IP adder:%d.%d.%d.%d <---> port:%d\r\n",DEST_IP_ADDR0,DEST_IP_ADDR1,DEST_IP_ADDR2,DEST_IP_ADDR3,DEST_PORT);

    while(1)
    {
        conn = netconn_new(NETCONN_TCP);
        if (conn == NULL)
        {
            printf("create conn failed!\n");
            vTaskDelay(10);
            continue;
        }

        IP4_ADDR(&ipaddr,DEST_IP_ADDR0,DEST_IP_ADDR1,DEST_IP_ADDR2,DEST_IP_ADDR3);

        ret = netconn_connect(conn,&ipaddr,DEST_PORT);
        if (ret == -1)
        {
            printf("Connect failed!\n");
            netconn_close(conn);
            vTaskDelay(10);
            continue;
        }

        printf("Connect to server successful!\n");

        while (1)
        {
            ret = netconn_write(conn,send_buf,sizeof(send_buf),0);

            vTaskDelay(1000);
        }
    }
}
#endif /* LWIP_DHCP */

//以下方法是测试使用的
/*
    tcp 客户端 方式
    服务器端看到端口后可发送数据进行回测
*/
void Tcp_client(void *thread_param)
{
    printf("clienttask run OK\r\n");
    
    #define DEST_PORT 8080
    
    int sock = -1;
    struct sockaddr_in client_addr;

    ip4_addr_t ipaddr;

    uint8_t recvbuf[100]={0};

    printf("des IP:%d.%d.%d.%d \t desport:%d\r\n",IP_S_ADDR0,IP_S_ADDR1,IP_S_ADDR2,IP_S_ADDR3,DEST_PORT);

    IP4_ADDR(&ipaddr,IP_S_ADDR0,IP_S_ADDR1,IP_S_ADDR2,IP_S_ADDR3);

    while(1)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            printf("Socket error\r\n");
            vTaskDelay(10);
            continue;
        } 
        else
        {
            printf("Socket ok\r\n");
        }
        
        client_addr.sin_family = AF_INET;      
        client_addr.sin_port = htons(DEST_PORT);   
        client_addr.sin_addr.s_addr = ipaddr.addr;
        memset(&(client_addr.sin_zero), 0, sizeof(client_addr.sin_zero));    

        if(connect(sock,(struct sockaddr *)&client_addr,sizeof(struct sockaddr)) == -1) 
        {
            printf("Connect failed\r\n");
            closesocket(sock);
            vTaskDelay(10);
            continue;
        }                                           

        printf("Connect to server successful!\r\n");

        while(1)
        {
            int recvsta = recv(sock,recvbuf,100,0);
            if(recvsta <= 0)
            {
                //接收数据错误
                printf("recv erro 1\r\n");
                break;
            }
            else
            {
                printf("recvlen:%d\r\n",recvsta);
								printf("recvbuf:%s\r\n",recvbuf);
                if(write(sock,recvbuf,recvsta) < 0)
                {
                    printf("write erro\r\n");
                    break;
                }
                else
                {
                    printf("write ok\r\n");
                    memset(recvbuf,0,100);
                }
            }
            
            vTaskDelay(100);
        }
        
        closesocket(sock);
    }
}


void UdpServer(void *thread_param)
{
    int udp_port = 1000;//端口

    struct sockaddr_in server_addr,client_addr;
    
    memset(&server_addr,0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(udp_port);
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    
    while(1)
    {
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(sockfd < 0)
        {
            vTaskDelay(10);
            printf("sockfd init error!\r\n");
            continue;
        }
        else
        {
            printf("sockfd init ok!\r\n");
        }
        
        int ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if(ret < 0)
        {
            lwip_close(sockfd);
            sockfd = -1;
            vTaskDelay(10);
            printf("bind init error!\r\n");
            continue;
        }
        else   
        {
            printf("bind init ok!\r\n");
        }
        
        uint8_t *socket_recv_buf = pvPortMalloc(100);//申请内存空间
				uint8_t *sendbuf = pvPortMalloc(100);

        while(-1 != sockfd)
        {
            memset(socket_recv_buf,0,100);//清空数据
            
            uint32_t len = sizeof(client_addr);
            int socketRecvbufdatalen = recvfrom(sockfd, socket_recv_buf, 80, 0, (struct sockaddr *)&client_addr, &len);
            
            socket_recv_buf[socketRecvbufdatalen]='\0';

						printf("\rsocket_recv_buf:%s\r\n",socket_recv_buf);
						
						memset(sendbuf,0,100);
						printf("client ip:%s\r\n",inet_ntoa(client_addr.sin_addr));
						printf("client port:%d\r\n",ntohs(client_addr.sin_port));
						sendto(sockfd,socket_recv_buf,strlen(socket_recv_buf),0,(struct sockaddr*)&client_addr,sizeof(client_addr));
            vTaskDelay(100);
        }
        
        vPortFree(socket_recv_buf);//释放内存空间
        lwip_close(sockfd);//关闭套接字
        
        printf("sockfd close!\r\n");
        
        vTaskDelay(100);
    }
}










