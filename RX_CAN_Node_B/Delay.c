#include<LPC21XX.H>
void delay_sec(unsigned int sec)
{
int a[]={15,60,30,15,15};
unsigned int pclk;
volatile unsigned char vpbdiv;
if(VPBDIV>4)
vpbdiv=VPBDIV%4;
else
vpbdiv=VPBDIV;
pclk=a[vpbdiv]*1000000-1;
T0PC=0;
T0PR=pclk;
T0TC=0;
T0TCR=1;
while(T0TC<sec);

T0TCR=0;
}

void delay_ms(unsigned int ms)
{
int a[]={15,60,30,15,15};
unsigned int pclk;
volatile unsigned char vpbdiv;
if(VPBDIV>4)
vpbdiv=VPBDIV%4;
else
vpbdiv=VPBDIV;
pclk=a[vpbdiv]*1000-1;
T0PC=0;
T0PR=pclk;
T0TC=0;
T0TCR=1;
while(T0TC<ms);
T0TCR=0;
}				
