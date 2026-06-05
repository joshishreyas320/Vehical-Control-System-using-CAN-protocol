#include<lpc21xx.h>
#include"header.h"

#define HL (1<<17)
#define LI (1<<18)
#define RI (1<<19)
#define Buzz (1<<20)

CAN2 data;

int flag=0,f_HL=0,f_LI=0,f_RI=0;

main()
{
	can2_init();
	config_vic_Can2();
	en_can2_interr();
	IODIR0|=HL|LI|RI;
	IOSET0|=HL|LI|RI;
	IODIR0|=Buzz;
	IOSET0|=Buzz;
	can2_init();

	while(1)
	{
		if(flag==1)
		{
			flag=0;
			/*---------For Hedlight----------*/

				if(data.byteA==0xAA)
				{
					f_HL=1;
				}
				else if(data.byteA==0xBB)
				{
					f_HL=0;
				}
			
			/*--------------------------------*/
			/*---------For Left Indicator----------*/

				if(data.byteA==0xCC)
				{
					if(f_RI)
						f_RI=0;
					f_LI=1;
				}
				else if(data.byteA==0xDD)
				{
					f_LI=0;
				}

			/*--------------------------------*/
			/*---------For Right Indicator----------*/
				if(data.byteA==0xEE)
				{
					if(f_LI)
						f_LI=0;
					f_RI=1;
				}
				else if(data.byteA==0xFF)
				{
					f_RI=0;
				}
			/*--------------------------------*/
			
			/*HL_SW*/
			if(f_HL)
			{
				IOCLR0=HL;
			}
			else
			{
				IOSET0=HL;
			}
			/*LI_SW*/
			if(f_LI)
			{
				while(flag==0)
				{
					IOCLR0=LI;
					IOCLR0=Buzz;
					delay_ms(200);
					IOSET0=LI;
					IOSET0=Buzz;
					delay_ms(200);
				}
			}
			else
			{
				IOSET0=LI;
				IOSET0=Buzz;
			}
			/*RI_SW*/
			if(f_RI)
			{
				while(flag==0)
				{
					IOCLR0=RI;
					IOCLR0=Buzz;
					delay_ms(200);
					IOSET0=RI;
					IOSET0=Buzz;
					delay_ms(200);
				}
			}
			else
			{
				IOSET0=RI;
				IOSET0=Buzz;
			}
		}
	}
}
	