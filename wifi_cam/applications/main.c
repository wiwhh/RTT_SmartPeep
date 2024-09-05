/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-06-29     Rbb666       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "LCD_ST7735.h"
#include "drv_gpio.h"
#include <lwip/sockets.h>
#include "portable.h"

#define min(X,Y) ((X)<(Y)?(X):(Y))

#define PORT              5001
#define RECV_DATA         2048
#define targetIP         "192.168.15.100"
#define targetPort         1024
#define TOTAL_LENGTH      96*96*2 // RGB565格式，每像素2字节
#define PACKETS             12

uint8_t receivedData[TOTAL_LENGTH];

void Udp_thread_entry(void* paramenter)
{
    int sock = -1;
    char *recv_data;
    struct sockaddr_in udp_addr,seraddr;
    int recv_data_len;
    socklen_t addrlen;
    recv_data = (char *)pvPortMalloc(RECV_DATA);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("Socket error\n");
        return ;
    }

    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(PORT);
    memset(&(udp_addr.sin_zero), 0, sizeof(udp_addr.sin_zero));
    if (bind(sock, (struct sockaddr *)&udp_addr, sizeof(struct sockaddr)) == -1)
    {
        printf("Unable to bind\n");
        return ;
    }
    seraddr.sin_family = AF_INET;
    seraddr.sin_addr.s_addr = inet_addr(targetIP);
    seraddr.sin_port = htons(targetPort);
    socklen_t client_addr_len = sizeof(struct sockaddr_in);
    uint8_t camflag = 0xAA;
    uint8_t sendflag = 0;
    int flags;

    if (ioctlsocket(sock, FIONBIO, (char *)&flags) < 0) {
        perror("ioctl failed");
        // 处理错误
    }
    rt_time_t last_tick,now_tick;
    /* 获取当前的滴答数 */
    last_tick = rt_tick_get();
    while (1)
    {
        now_tick = rt_tick_get();
        if(now_tick-last_tick > 1800)
        {
            last_tick = now_tick;
            sendto(sock, &camflag, 1, 0, (struct sockaddr*)&seraddr,client_addr_len);
            //printf("camflag,send\r\n");
        }
        recv_data_len=recvfrom(sock, recv_data, 1441, 0, (struct sockaddr*)&seraddr, &addrlen);
        if(recv_data_len>0)
        {
            uint8_t packetNumber = recv_data[0]; // 包序号
            int start = packetNumber * (1441 - 1);
            int end = start + recv_data_len - 1;
            int len = end - start;
            memcpy(&receivedData[start], &recv_data[1], len);
            //printf("Received packet %d of size %d\n", packetNumber, len);
        }
    }
}

void Lcd_show_thread_entry(void* paramenter)
{
    while(1)
    {
        LCD_DrawPicture(receivedData);
        rt_thread_delay(1800);
    }
}

//void sendaa_thread_entry(void* paramenter)
//{
//    int sock = -1;
//    struct sockaddr_in udp_addr,seraddr;
//    uint8_t camflag = 0xAA;
//    sock = socket(AF_INET, SOCK_DGRAM, 0);
//    if (sock < 0)
//    {
//        printf("Socket error\n");
//        return ;
//    }
//    socklen_t client_addr_len = sizeof(struct sockaddr_in);
//    seraddr.sin_family = AF_INET;
//    seraddr.sin_addr.s_addr = inet_addr(targetIP);
//    seraddr.sin_port = htons(targetPort);
//    while(1)
//    {
//        sendto(sock, &camflag, 1, 0, (struct sockaddr*)&seraddr,client_addr_len);
//        rt_thread_delay(2000);
//    }
//}

int main(void)
{

    rt_thread_t Udp_thread, Lcd_thread;
    printf("hellow world\r\n");
    Udp_thread = rt_thread_create("Udp_thread", Udp_thread_entry, NULL, 2048 , 25, 1);
    Lcd_thread = rt_thread_create("Lcd_thread", Lcd_show_thread_entry, NULL, 2048 , 24, 2);
    rt_thread_startup(Udp_thread);
    rt_thread_startup(Lcd_thread);


}
