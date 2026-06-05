#include<lpc21xx.h>
#include"header.h"

#define HL ((IOPIN0>>14)&1)
#define LI ((IOPIN0>>15)&1)
#define RI ((IOPIN0>>16)&1)

extern CAN2 data;
extern int flag_LI,flag_RI,flag_HL,flag;

void eint0_HL(void)__irq
{
	flag=1;
	flag_HL=(1^flag_HL);
	if(flag_HL)
	{
		uart0_tx_string("HL_ON--0xAA\r\n");
		data.byteA=0xAA;
		can2_tx(data);
	}
	else
	{
		uart0_tx_string("HL_OFF--0xBB\r\n");
		data.byteA=0xBB;
		can2_tx(data);
	}
	EXTINT=(1<<0);
	VICVectAddr=0;
}

void eint1_LI(void)__irq
{
	flag=1;
	if(flag_RI)
	flag_RI=0;
	flag_LI=(1^flag_LI);
	if(flag_LI)
	{
		uart0_tx_string("LI_ON--0xCC\r\n");
		data.byteA=0xCC;
		can2_tx(data);
	}
	else
	{
		uart0_tx_string("LI_OFF--0xDD\r\n");
		data.byteA=0xDD;
		can2_tx(data);
	}
	EXTINT=(1<<1);
	VICVectAddr=0;
}

void eint2_RI(void)__irq
{
	flag=1;
	if(flag_LI)
	flag_LI=0;
	flag_RI=(1^flag_RI);
	if(flag_RI)
	{
		uart0_tx_string("RI_ON--0xCC\r\n");
		data.byteA=0xEE;
		can2_tx(data);
	}
	else
	{
		uart0_tx_string("RI_OFF--0xDD\r\n");
		data.byteA=0xFF;
		can2_tx(data);
	}
	EXTINT=(1<<2);
	VICVectAddr=0;
}

void config_Vic(void)
{
		VICIntSelect=0;

		VICVectCntl0=(1<<5)|14;
		VICVectAddr0=(int)eint0_HL;

		VICVectCntl1=(1<<5)|15;
		VICVectAddr1=(int)eint1_LI;

		VICVectCntl2=(1<<5)|16;
		VICVectAddr2=(int)eint2_RI;

		VICIntEnable=(7<14);
}

void eint_vic(void)
{
	PINSEL0|=0xA0000000;
	PINSEL1|=0X1;
	EXTMODE=0x7;
	EXTPOLAR=0x0;
}
