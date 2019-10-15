#ifndef __DEF_H
#define __DEF_H

#define SetMoto(L1,L2,R1,R2);	l1 = (L1); \
								l2 = (L2); \
								r1 = (R1); \
								r2 = (R2);

#define LEFT 	'0'
#define RIGHT 	'1'
#define GO		'2'
#define BACK	'3'
#define LEFT45 	4
#define RIGHT45	5
#define STOP	'6'
#define AUTO	'7'
#define MANUAL	'8'
								
#define KP		10
#define KI		11
#define KD		12

sbit r1 = P1^0;
sbit r2 = P1^1;
sbit l2 = P1^5;
sbit l1 = P1^6;

sbit enl = P1^3;
sbit enr = P1^4;

sbit mostr = P2^5;
sbit mostl = P2^6;
sbit irm = P2^2;
sbit irml= P2^3;
sbit irmr= P2^1;

#endif