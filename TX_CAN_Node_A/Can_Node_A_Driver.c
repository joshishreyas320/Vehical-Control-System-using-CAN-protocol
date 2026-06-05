#include <lpc21xx.h>
#include "header.h"

#define select_tx_buf1 (1<<5)
#define start_tx (1<<0)
#define RBS (C2GSR&1)
#define RRB (1<<2)

void can2_init(void)
{
	PINSEL1|=0x14000;
	VPBDIV=1;  
	C2MOD=0x1;    
	AFMR=0x2;
	C2BTR=0x001C001D;
	C2MOD=0;
}

void can2_tx(CAN2 v)
{
	C2TFI1=(v.dlc<<16); 
	C2TID1=v.id;
	if(v.rtr==0)
	{
		C2TFI1&=~(1<<30);
		C2TDA1=v.byteA;
		C2TDB1=v.byteB;
	}
	else
		C2TFI1|=(1<<30);

	C2CMR=(start_tx|select_tx_buf1);
	while(((C2GSR>>3)&1)==0);
	uart0_tx_string("case-1\r\n");
}
