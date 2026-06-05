//Can Interrupt

#include<lpc21xx.h>
#include"header.h"

extern int flag;
extern CAN2 data;

void Can2_RX_HANDLER(void)__irq
{
	flag=1;
	data.id=C2RID;
	data.dlc=(C2RFS>>16)&0x0f;
	data.rtr=(C2RFS>>30)&1;
	if(data.rtr==0)
	{
		data.byteA=C2RDA;
		data.byteB=C2RDB;
	}
	C2CMR=(1<<2);
	VICVectAddr=0;
}
void config_vic_Can2(void)
{
	VICIntSelect=0;
	VICVectCntl0=(1<<5)|27;
	VICVectAddr0=(int)Can2_RX_HANDLER;
	VICIntEnable=(1<<27);
}

void en_can2_interr(void)
{
	C2IER=0x1;
}
