#include <lpc21xx.h>
#include "header.h"

#define select_tx_buf1 (1<<5)
#define start_tx (1<<0)
#define RBS (C2GSR&1)
#define RRB (1<<2)

unsigned int *ptr=(u32 *)0xE0038000;

void can2_init(void)
{
	PINSEL1|=0x14000;
	VPBDIV=1;  
	C2MOD=0x1;
	/*Message Acceptance Settings*/
	ptr[0]=0x20352037;
	ptr[1]=0x21322158;
	ptr[2]=0x23AB23FD;
	SFF_sa=0;
	SFF_GRP_sa=0xC;
	EFF_sa=0xC;
	EFF_GRP_sa=0xC;
	ENDofTable=0xC;
	/*..............................*/   
	AFMR=0x2;
	C2BTR=0x001C001D;
	C2MOD=0;
}

void can2_rx(CAN2 *ptr)
{
	while(RBS==0);
	ptr->id=C2RID;
	ptr->dlc=(C2RFS>>16)&0xF;
	ptr->rtr=(C2RFS>>30)&1;
	if(ptr->rtr==0)	 
	{
		ptr->byteA=C2RDA;
		ptr->byteB=C2RDB;
	}
	C2CMR=RRB;	
}
