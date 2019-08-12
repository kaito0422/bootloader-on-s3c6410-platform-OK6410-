/*kaito's*/

#define SDMA_SEL 		*((volatile unsigned long*)0x7E00F110)
#define DMACConfiguration 	*((volatile unsigned long*)0x7DB00030)
#define DMACC0SrcAddr 		*((volatile unsigned long*)0x7DB00100)
#define DMACC0DestAddr 		*((volatile unsigned long*)0x7DB00104)

#define UTXH0 (volatile unsigned long*)0x7F005020

#define DMACC0Control0 		*((volatile unsigned long*)0x7DB0010c)
#define DMACC0Control1 		*((volatile unsigned long*)0x7DB00110)
#define DMACC0Configuration 	*((volatile unsigned long*)0x7DB00114)

char pk[80]="\n\rDo you know who I really am?\n\r";

void dma_init()
{
	//选择dma控制器
	SDMA_SEL=0b0;
	
	//使能dma控制器
	DMACConfiguration=1;
	
	//初始化源地址
	DMACC0SrcAddr=(unsigned int)pk;
	
	//初始化目的地址
	DMACC0DestAddr=(unsigned int)UTXH0;
	
	//对控制寄存器进行配置
	/*
		1.源地址自增
		2.目的地址固定
		3.目标主机选择AHB主机2
		4.源主机选择AHB主机1
	*/
	DMACC0Control0=(1<<25)|(1<<26)|(1<<31);
	DMACC0Control1=0x32;
	
	
	//流控制和传输类型，目标外设，通道有效
	DMACC0Configuration=(1<<15)|(1<<14)|(1<<11)|(1<<6);
}

void dma_start()
{
	//启动DMA传输
	DMACC0Configuration=0b1;
}