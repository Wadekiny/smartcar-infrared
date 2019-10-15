#include "pwm.h"
#include <STC12C5A60S2.h>
#define HIGH 0x1a   //90zkb
#define LOW 0X80 	//50zkb
#define NORMAL 0x4d	//70zkb

//pwm0左电机 P1.3
//pwm1右电机 P1.4
void PwmInit()
{
	CCON = 0;
	CL = 0;
	CH = 0;
	CMOD = 0X02;

	CCAP0H = CCAP0L = 0xff;
	CCAPM0 = 0X42;
	CCAP1H = CCAP1L = 0xff;
	CCAPM1 = 0X42;

	CR = 1;
}


void LeftSet(unsigned char set){ CCAP0H = CCAP0L = set; }
void RightSet(unsigned char set){ CCAP1H = CCAP1L = set; }