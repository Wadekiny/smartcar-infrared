#include <STC12C5A60S2.h>
#include <math.h>
#include "delay.h"
#include "oled.h"
#include "pwm.h"
#include "uart.h"
#include "def.h"
#include "binary.h"
#include "inits.h"
#include "show.h"

#define u8 unsigned char
#define u16 unsigned int

void DetectError();
void CaculateDutyCycle();
void ApplyDutyCycle();
void ModeAuto();
void ModeManual();
void Angle90(u8 dir90);
void Angle45(u8 dir45);

float max_zkb = 0;
u8 angle = 0;		//上一个角
u8 special = 0;		//特殊情况
u8 Tcount = 0;		//T口记录
extern u8 get;		//接收数据
u8 mode = AUTO;		//工作模式
float left_dutyc;		//左电机占空比
float right_dutyc;		//右电机占空比
const float default_dutyc = 59.5;	//默认占空比

//---------------------pid---------------------//
float kp = 2,ki = 0.02,kd = 0; 		//pid系数
float error = 0,p_error = 0;		//两次偏差值
float I=0,D=0;						//积分项，微分项
float pid_out = 0;					//pid输出
float pp_error = 0;
//---------------------pid---------------------//

void main()
{
	InitAll();
	while (1)
	{	
	}
}

void t0timer() interrupt 1
{
	TR0 = 0;
    TH0 = 0xdc;//b8
    TL0 = 0x00;
	Show();
	if (mode == AUTO)
		ModeAuto();
	else if (mode == MANUAL)
		ModeManual();
	TR0 = 1;
}


void CaculateDutyCycle() //计算占左右电机空比
{	
	if (special == 0)
	{
		D = error - p_error;
		pid_out = kp*error + kd*D;
		//更新左右电机占空比 
		left_dutyc  = default_dutyc + pid_out;
		right_dutyc = default_dutyc - pid_out;
	}
	else if (special == 1)
	{
		left_dutyc = 0;
		right_dutyc = default_dutyc+4;
	}
	else if (special == 2)
	{
		left_dutyc = default_dutyc+4;
		right_dutyc = 0;
	}
	if (left_dutyc > max_zkb)
		max_zkb = left_dutyc;
	if (right_dutyc > max_zkb)
		max_zkb = right_dutyc;
	
	if (left_dutyc > 75)
		left_dutyc = 75;
	if (right_dutyc > 75)
		right_dutyc = 75;
	ApplyDutyCycle();
}

void DetectError() //检测偏差
{  
	float detect_error = 0;
	switch (P2 & 0x1f)//取后5位
	{
		//循迹功能
		case B01111: detect_error = -4.5	; break;//5
		case B00111: detect_error = -4; break;//4.5
    	case B10111: detect_error = -3	; break;
		case B10011: detect_error = -1.2; break;			
		case B11011: detect_error =  0	; break;
		case B11001: detect_error =  1.2; break;
		case B11101: detect_error =  3	; break;
		case B11100: detect_error =  4; break;
		case B11110: detect_error =  4.5	; break;
		default : detect_error = error; 
	}
	
	//特殊判断 
	if (special && (irm == 0)) special = 0;
	if (mostl==0 && mostr)		//左侧检测到黑线 左拐
	{
		special = 1;
		if ((P2 & 0x1f)==B11011 || (P2 & 0x1f)==B10111 || (P2 & 0x1f)==B11101 || (P2 & 0x1f)==B10011 || (P2 & 0x1f)==B11001) 
		{
			Delay10ms();
			if ((P2 & 0x1f)==B11011 || (P2 & 0x1f)==B10111 || (P2 & 0x1f)==B11101 || (P2 & 0x1f)==B10011 || (P2 & 0x1f)==B11001) 
				Angle45(0);
		}
		else if ((P2 & 0x1f)==B00011 || (P2 & 0x1f)==B00111 || (P2 & 0x1f)==B00001)	//直角左拐
		{
			Delay10ms();
			if ((P2 & 0x1f)==B00011 || (P2 & 0x1f)==B00111 || (P2 & 0x1f)==B00001)
				Angle90(0);
	
		}
	}
	else if (mostl && mostr==0)	//右侧检测到黑线 右拐
	{
		special = 2;
		if ((P2 & 0x1f)==B11011 || (P2 & 0x1f)==B10111 || (P2 & 0x1f)==B11101 || (P2 & 0x1f)==B10011 || (P2 & 0x1f)==B11001)
		{	
			Delay10ms();
			if ((P2 & 0x1f)==B11011 || (P2 & 0x1f)==B10111 || (P2 & 0x1f)==B11101 || (P2 & 0x1f)==B10011 || (P2 & 0x1f)==B11001)
				Angle45(1);
		}
		else if ((P2 & 0x1f)==B11000 || (P2 & 0x1f)==B11100 || (P2 & 0x1f)==B10000)	//直角右拐
		{
			Delay10ms();
			if ((P2 & 0x1f)==B11000 || (P2 & 0x1f)==B11100 || (P2 & 0x1f)==B10000)
				Angle90(1);
		}
	}
	else if (mostl && mostr && ((P2 & 0x1f)==B10001 || (P2 & 0x1f)==B00000 || (P2 & 0x1f)==B10000 || (P2 & 0x1f)==B00001))	
	{	//死路或终点
		Delay10ms();
		if (mostl && mostr && ((P2 & 0x1f)==B10001 || (P2 & 0x1f)==B00000 || (P2 & 0x1f)==B10000 || (P2 & 0x1f)==B00001))
		{
			Delay100ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();
			if (!irm || !irml || !irmr)//终点
			{
				SetMoto(0,0,0,0);
				while (1);
			}
			else// ((P2 & 0x1f) == B00000)
			{  
				if (angle == 2) angle = 3;
				SetMoto(0,0,0,0);
				left_dutyc = default_dutyc+3;
				right_dutyc = default_dutyc+3;
				ApplyDutyCycle();
				Delay100ms();
				SetMoto(1,0,0,1);
				Delay500ms();
				SetMoto(1,0,1,0);
			}
		}
	}
	else if ((P2&0x1f) == B00000 && !mostl && !mostr)//全黑 t 十字
	{
		Delay10ms();
		if ((P2&0x1f) == B00000 && !mostl && !mostr)//t
		{
			Delay100ms();Delay100ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();Delay10ms();   
			if ((P2&0x1f) == B11111)
			{		
				special = 1+angle;
				Angle90(angle);
				Tcount++;
			}
		}
		else//十字
		{
			if (angle == 3)
			{
				Angle90(0);
			}
			angle = 2;
		}
	}
	//更新error
	p_error = error;
	error = detect_error;
}

void ModeAuto()
{
	
	if (get == MANUAL || get == STOP)
	{
		mode = MANUAL;
		SetMoto(0,0,0,0);
		OLED_SetClear(0,50,6,7);
		OLED_ShowString(0,6,"MAN");
		PidInit();
		return;
	}
	
	SetMoto(1,0,1,0);
	DetectError();
	CaculateDutyCycle();
}

void ApplyDutyCycle()	//应用占空比
{
	u8 lll,rrr;
	lll = 255-left_dutyc*255/100.0;
	rrr = 255-right_dutyc*255/100.0;
	LeftSet(lll);
	RightSet(rrr);
}

void ModeManual()
{
	left_dutyc = 70;
	right_dutyc = 70;
	ApplyDutyCycle();
	switch (get)
	{
		case LEFT  : SetMoto(0,0,1,0); break;
		case RIGHT : SetMoto(1,0,0,0); break;
		case GO    : SetMoto(1,0,1,0); break;
		case BACK  : SetMoto(0,1,0,1); break;
		case STOP  : SetMoto(0,0,0,0); break;
		case AUTO  :
			mode = AUTO; 
			SetMoto(1,0,1,0);
			PidInit();
			OLED_SetClear(0,50,6,7);
			OLED_ShowString(0,6,"AUTO");
			return;
		default : SetMoto(1,1,1,1);
	}
}

void Angle90(u8 dir90) //dir=0=left dir=1=right
{
	angle = dir90;
	SetMoto(0,0,0,0);
	left_dutyc = default_dutyc+2;
	right_dutyc = default_dutyc+2;
	ApplyDutyCycle();
	if (dir90 == 0){ SetMoto(0,1,1,0); }
	else           { SetMoto(1,0,0,1); }
	Delay100ms();Delay100ms();Delay100ms();Delay100ms();
	SetMoto(1,0,1,0);
}

void Angle45(u8 dir45)
{
	//倒车
	SetMoto(0,0,0,0);
	left_dutyc = default_dutyc;
	right_dutyc = default_dutyc;
	ApplyDutyCycle();
	SetMoto(0,1,0,1);
	Delay100ms();
	Delay10ms();
	//锐角转弯
	SetMoto(0,0,0,0);
	if (dir45 == 0)
	{
		left_dutyc = 0;
		right_dutyc = default_dutyc+3;
	}
	else
	{
		left_dutyc = default_dutyc+3;
		right_dutyc = 0;
	}	
	ApplyDutyCycle();
	SetMoto(1,0,1,0);
	Delay1s();
}