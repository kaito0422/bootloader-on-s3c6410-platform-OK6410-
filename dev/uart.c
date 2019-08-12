#define GPACON *((volatile unsigned short*)0x7F008000)
#define ULCON0 *((volatile unsigned long*)0x7F005000)
#define UCON0  *((volatile unsigned long*)0x7F005004)
#define UBRDIV0 *((volatile unsigned short*)0x7F005028)
#define UDIVSLOT0 *((volatile unsigned short*)0x7F00502C)
#define UTRSTAT0 *((volatile unsigned long*)0x7F005010)
#define UTXH0 *((volatile unsigned char*)0x7F005020)
#define URXH0 *((volatile unsigned char*)0x7F005024)
#define UFCON0 *((volatile unsigned long*)0x7F005008)
#define UFSTAT0 *((volatile unsigned long*)0x7F005018)

#define PCLK 66500000
#define BAUD 115200

void uart_init()
{
	//引脚设置
//	GPACON&=~0xff;
	GPACON|=0x22;
	
	//数据格式设置
	ULCON0=0x3;
	
	//工作模式设置
	UCON0=0x5;
	UFCON0=0x01;
	
	//波特率设置
	UBRDIV0=(int)(PCLK/(BAUD*16)-1);
//	UDIVSLOT0=0x0D5D5;
	UDIVSLOT0=0x1;
}



void putc(unsigned char ch)
{
	while((UFSTAT0&(0b1<<14)));
	UTXH0=ch;
}




unsigned char getc()
{
	unsigned char dat;
	while((UFSTAT0&0x7f)==0);
	dat=URXH0;
	if ((dat==0x0d)||(dat==0x0a))
	{
	    putc(0x0d);
	    putc(0x0a);	
	}      	
	else
	putc(dat);
	return dat;
}


