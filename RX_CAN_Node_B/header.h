#include<lpc21xx.h>
#include<string.h>

//delay functions
extern void delay_sec(unsigned int);
extern void delay_ms(unsigned int);

/* Defining data type shortcut*/
typedef unsigned char u8;
typedef char s8;
typedef unsigned short int u16;
typedef short int s16;
typedef unsigned int u32;
typedef int s32;

//lcd functions
extern void lcd_cmd(unsigned char cmd);
extern void lcd_data(unsigned char data);
extern void lcd_init(void);
extern void lcd_string(char *ptr);
extern void lcd_integer(int num);
extern void lcd_float(float f);
extern void lcd_cgram(void);

//uart functions
extern void uart0_init(unsigned int);
extern void uart0_tx(unsigned char );
extern void uart0_tx_string(char *);
extern unsigned char uart0_rx(void);
extern void uart0_ascii(unsigned char );
extern void uart0_hex(unsigned char);
extern void uart0_ascii(unsigned char);
extern void uart0_int(s32);
extern void uart0_rx_string(s8 *,s32 );
extern void uart0_tx_float(float );
extern void uart0_tx_hex(u32 );

//CAN Structuire

typedef struct CAN2_msg
{
	u32 id;
	u32 byteA;
	u32 byteB;
	u8 dlc;
	u8 rtr;
}CAN2;

//Can Rx driver

extern void can2_rx(CAN2 *ptr);
extern void can2_init(void);

//Can Rx Interrupt
extern void Can2_RX_HANDLER(void)__irq;
extern void config_vic_Can2(void);
extern void en_can2_interr(void);
