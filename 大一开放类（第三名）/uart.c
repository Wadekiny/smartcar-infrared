#include <STC12C5A60S2.h>
#include "uart.h"
#include "def.h"

unsigned char index = 0;
unsigned char gets[3];
unsigned char get;
extern float kp;
extern float ki; 
extern float kd;
bit get9;

void UartInit()
{
	//不用PCON的SMOD 不加倍

	/*SCON八位数据 如果用第九位设置成d0--(为了蓝牙推荐设置成50)*/
	SCON |= 0x50; //设置模式1 允许接收 禁止多机
	AUXR |= 0X11;//使用独立波特率 12分频
	//装初值 公式：
	//RELOAD = 256-(INT)(SYSclk/Baud0/32/T*2^SOMD+0.5)
	BRT=0Xfd;
	EA = 1;	//总中断
	ES = 1;	//串口中断 用来接收数据
}

void SendChar(unsigned char dat)
{
	SBUF = dat;		//送SBUF
	while(!TI);		//等待送完
    TI = 0;
}
void SendStr(unsigned char *s)
{
	while(*s != '\0')         
	{
		SendChar(*s);
		s++;
	}
}

void uart_isr() interrupt 4	//接收
{
	if (RI)
	{
		RI = 0;
		//gets[index++] = SBUF;
		get = SBUF;
		if (get == 0xff)
		{
			kp -= 0.1;
		}
		/*
		if (index == 3)
		{
			index = 0;
			kp = gets[0];
			ki = gets[1];
			kd = gets[2];
		}
		*/
	}
}