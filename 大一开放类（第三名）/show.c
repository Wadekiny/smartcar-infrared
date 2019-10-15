#include <STC12C5A60S2.h>
#include "show.h"
#include "oled.h"

extern float left_dutyc;
extern float right_dutyc;
extern u8 Tcount;
extern float max_zkb;
extern u8 angle;
//------------------------Show Init------------------------//
void Show()
{
	OLED_ShowString(0,0,"Left  ");
	OLED_ShowString(0,2,"Right ");
	OLED_ShowNum(50,0,(u8)left_dutyc,3,16);
	OLED_ShowNum(50,2,(u8)right_dutyc,3,16);
	
	//P2-IR 低五位	//4 3 2 1 0//
	OLED_ShowNum(50,6,P2>>4 &1,1,16);
	OLED_ShowNum(60,6,P2>>3&1,1,16);
	OLED_ShowNum(70,6,P2>>2&1,1,16);
	OLED_ShowNum(80,6,P2>>1&1,1,16);
	OLED_ShowNum(90,6,P2>>0&1,1,16);
	OLED_ShowString(110,6,"IR");

	//P1-MOTO	//6 5 - 4 3 - 1 0//
	OLED_ShowNum(0,4,P1>>6 &1,1,16);
	OLED_ShowNum(10,4,P1>>5&1,1,16);
	OLED_ShowChar(20,4,'-');
	OLED_ShowNum(30,4,P1>>4&1,1,16);
	OLED_ShowNum(40,4,P1>>3&1,1,16);
	//OLED_ShowNum(50,4,P1>>2&1,1,16);
	OLED_ShowChar(50,4,'-');
	OLED_ShowNum(60,4,P1>>1&1,1,16);
	OLED_ShowNum(70,4,P1>>0&1,1,16);
	OLED_ShowString(90,4,"MOTO");
	
	
	OLED_ShowNum(90,0,Tcount,1,16);
	OLED_ShowNum(90,2,angle,3,16);
}