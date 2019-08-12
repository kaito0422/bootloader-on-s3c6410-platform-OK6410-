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
	//ѡ��dma������
	SDMA_SEL=0b0;
	
	//ʹ��dma������
	DMACConfiguration=1;
	
	//��ʼ��Դ��ַ
	DMACC0SrcAddr=(unsigned int)pk;
	
	//��ʼ��Ŀ�ĵ�ַ
	DMACC0DestAddr=(unsigned int)UTXH0;
	
	//�Կ��ƼĴ�����������
	/*
		1.Դ��ַ����
		2.Ŀ�ĵ�ַ�̶�
		3.Ŀ������ѡ��AHB����2
		4.Դ����ѡ��AHB����1
	*/
	DMACC0Control0=(1<<25)|(1<<26)|(1<<31);
	DMACC0Control1=0x32;
	
	
	//�����ƺʹ������ͣ�Ŀ�����裬ͨ����Ч
	DMACC0Configuration=(1<<15)|(1<<14)|(1<<11)|(1<<6);
}

void dma_start()
{
	//����DMA����
	DMACC0Configuration=0b1;
}