#include<lpc21xx.h>
#include"header.h"


#define THRE ((U0LSR>>5)&1)
#define RDR (U0LSR&1)

//init driver
void uart0_init(unsigned int baud)
{
	unsigned int result=0;
	int a[]={15,60,30,15,15};
	unsigned int pclk =0;
	pclk=a[VPBDIV]*1000000;
	result=pclk/(16*baud);
	PINSEL0|=0x05;	
	U0LCR=0x83;
	U0DLL=result&0xFF;
	U0DLM=(result>>8)&0xFF;
	U0LCR^=(1<<7);
	//U0LCR=0x03;
}

//transmit driver
void uart0_tx(unsigned char data)
{
	U0THR=data;
	while(THRE==0);
}

//Transmit string driver
void uart0_tx_string(char *ptr)
{
	while(*ptr)
	{
		U0THR=*ptr;
		while(THRE==0);
		ptr++;
	}
}

//Reciver Driver 
unsigned char  uart0_rx(void)
{
	while(RDR==0);
	return U0RBR;
}

/*ASCII VALUE*/
void uart0_ascii(unsigned char data)
{
	uart0_tx((data/100)+48);
	uart0_tx(((data/10)%10)+48);
	uart0_tx((data%10)+48);
}

/*Decimal to HEX Value */
void uart0_hex(unsigned char data)
{
	unsigned char temp;
	uart0_tx_string("0x0");
	temp=(data&0xf0)>>4;
	uart0_tx(temp+48);
	temp=data&0x0f;
	if(temp>9)
	uart0_tx('A'+(temp-10));
	else
	uart0_tx('0'+temp);
}

/* ASCII Integer Decimal*/
void uart0_int(s32 num)
{
	s32 i=0;
	s8 a[16];
	if(num==0)
	{
		uart0_tx('0');
		return;
	}
	if(num<0)
	{
		uart0_tx('-');
		num=-num;
	}
	while(num>0)
	{
		a[i]=(num%10)+48;
		i++;
		num=num/10;
	}

	for(i--;i>=0;i--)
	{
		uart0_tx(a[i]);
	}
}

/* Reciving String Driver*/
void uart0_rx_string(s8 *ptr,s32 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		while(RDR==0);
		ptr[i]=U0RBR;
		if(ptr[i]=='\r')
			break;
	}
	ptr[i]='\0';
}

void uart0_tx_float(float num)
{
	int i,temp,temp1,l;
	char a[16];
	if(num==0)
	{
		uart0_tx_string("0.000000");
		return;
	}
	if(num<0)
	{
		uart0_tx('-');
		num=-num;
	}
	temp=num;
	if(temp==0)
	{
		uart0_tx('0');
	}
	else
	{
		for(l=0,temp1=temp,i=0;temp1;temp1=temp1/10,i++,l++)
		{
			a[i]=(temp1%10)+48;
		}
		for(i=l-1;i>=0;i--)
		{
			uart0_tx(a[i]);
		}
	}
	uart0_tx('.');
	//i=0;
	temp=(num-temp)*1000000;
	if(temp==0)
	{
		uart0_tx_string("000000");
	}
	else
	{
		for(i=0;temp;temp=temp/10,i++)
		{
			a[i]=(temp%10)+48;
		}
		for(i=5;i>=0;i--)
		{
			uart0_tx(a[i]);
		}
	}
}

void uart0_tx_hex(u32 n)
{
	s32 i,f=1,l;
	u8 t;
	for(i=0;i<8;i++)
	{
		l=4*(7-i);
		t=(n>>l)&0xf;
		if(f)
			if(t)
				f=0;
		if(f==0)
		{
			if(t>9)
				uart0_tx('A'+(t-10));
			else
				uart0_tx(t+48);
		}
	}
}
