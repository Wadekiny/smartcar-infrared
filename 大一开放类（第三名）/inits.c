#include "inits.h"
#include <STC12C5A60S2.h>
#include "uart.h"
#include "delay.h"
#include "oled.h"
#include "pwm.h"
#include "def.h"

extern float kp;
extern float ki;
extern float kd;
extern float error;
extern float p_error;
extern float pp_error;
extern float I;
extern float D;
extern float pid_out;
extern unsigned char mode;					//工作模式
extern float left_dutyc;			//左电机占空比
extern float right_dutyc;			//右电机占空比
extern float default_dutyc;	//默认占空比

void InitAll()	//初始化 延时1.61262s
{
	PidInit();
	PwmInit();
	OLED_Display_Off();
	OLED_Init();
	OLED_Clear();
	OLED_Display_On();
	UartInit();
	Delay20us();
	Delay100us();
	Delay500us();
	Delay1ms();
	Delay10ms();
	Delay100ms();
	Delay500ms();
	Delay1s();
	DelayMs(1);
	SendStr("OK");
	
	if (mode == MANUAL)
	{
		OLED_ShowString(0,6,"MAN");
	}
	else if (mode == AUTO)
	{
		OLED_ShowString(0,6,"AUTO");
	} 

	T0Init();
}

void PidInit()	//Pid初始化
{
	kp = 2.9;//2.9
	ki = 0;
	kd = 12;//12
	error = 0;
	p_error = 0;
	pp_error = 0;
	I=0;
	D=0;
	pid_out = 0;
	
	left_dutyc 	= default_dutyc+5;	//左电机占空比
	right_dutyc = default_dutyc+5;	//右电机占空比
}

void T0Init()		//10ms
{
    TMOD = 0x01;
    TH0 = 0xdc;
    TL0 = 0x00;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}