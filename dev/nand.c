#define NFCONT (*((volatile unsigned long*)0x70200004))
#define NFSTAT (*((volatile unsigned char*)0x70200028))
#define NFCMMD (*((volatile unsigned char*)0x70200008))
#define NFADDR (*((volatile unsigned char*)0x7020000C))
#define NFDATA (*((volatile unsigned char*)0x70200010))

#define NFCONF (*((volatile unsigned long*)0x70200000))

void select_chip()
{
	NFCONT&=~(1<<1);
}

void disselect_chip()
{
	NFCONT|=(1<<1);
}

void clean_RB()
{
	NFSTAT|=(1<<4);
}

void send_cmd(unsigned char cmd)
{
	NFCMMD=cmd;
}
void wait_RB()
{
	while(!(NFSTAT&0x1));
}

void send_addr(unsigned char addr)
{
	NFADDR=addr;
}

void nand_reset()
{
	//ѡ��оƬ
	select_chip();
	
	//���RB
	clean_RB();
	
	//���͸�λ�ź�0xff
	send_cmd(0xff);
	
	//�ȴ�RB��0��1
	wait_RB();
	
	//ȡ��ѡ��оƬ
	disselect_chip();
}

void nand_init()
{
/*
HCLK��Ƶ��Ϊ100MHZ�����ھ�Ϊ10ns
TACLS > 0 ns
TWRPH0	> 15ns
TWRPH1 > 5ns
 
TACLS��ֵ = HCLK x TACLS > 0ns
TWRPH0��ֵ = HCLK x (TWRPH0 + 1) > 15ns
TWRPH1��ֵ = HCLK x (TWRPH1 +1) > 5ns 
*/
#define TACLS  1
#define TWRPH0 2
#define TWRPH1 1
	//����ʱ�����
	NFCONF&=~((7<<12)|(7<<8)|(7<<4));		//������
	NFCONF|=(TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	
	//ʹ��nandflash controller
	NFCONT=1|(1<<1);
	
	//��λ
	nand_reset();
}

void nf_pageread(unsigned long addr,unsigned char* buff)
{
	int i;
	
	//ѡ��оƬ
	select_chip();
	
	//���RB
	clean_RB();
	
	//��������0x00
	send_cmd(0x00);
	
	//�����е�ַ
	send_addr(0x00);
	send_addr(0x00);
	
	//�����е�ַ
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)&0xff);
	
	//��������0x30
	send_cmd(0x30);
	
	//�ȴ�RB�źŴ�0���1�����У�
	wait_RB();
	
	//������
	for(i=0;i<4*1024;i++)
	buff[i]=NFDATA;
	
	//ȡ��ѡ��оƬ
	disselect_chip();
}

void nand_to_ram(unsigned long start_addr,unsigned char* sdram_addr,int size)
{
	int i;	//iΪҳ�š�sdram_addrΪ�ڴ��е�λ�á�size�������ݵĴ�С
	
/*
   S3C6410����ʱ������8K���벻�Ǵ洢��Nand flash�ĵ�һҳ�ϣ�
   ���Ǵ洢��Nand flash��ǰ4ҳ�ϣ�ÿҳ2K���ܹ�8K��
  
*/
	
	for(i=0;i<4;i++,sdram_addr+=1024*2)
	{
		nf_pageread(i,sdram_addr);	//��nandflash�����ַΪi�����ݶ����ڴ��sdram_addr��λ�ô�
	}
	
	size-=1024*8;	//Ҫ�����������Ѿ���8K��������
	
	for( i=4; size>0;)
	{
	    nf_pageread(i,sdram_addr);	
	    size -= 4096;
	    sdram_addr += 4096;
	    i++;
	}
	
}

int nf_erase(unsigned long addr)
{
	int dat;
	
	//ѡ��оƬ
	select_chip();
	
	//���RB
	clean_RB();
	
	//��������60h
	send_cmd(0x60);
	
	//����3���е�ַ
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)&0xff);
	
	//��������d0h
	send_cmd(0xd0);
	
	//�ȴ�RB
	wait_RB();
	
	//��������70h
	send_cmd(0x70);
	
	//��ȡ�������
	dat=NFDATA;
	
	//ȡ��ѡ��оƬ
	disselect_chip();
	
	return dat;
}

int nf_pagewrite(unsigned long addr,unsigned char* buff)
{
	int i,dat;
	
	//ѡ��оƬ
	select_chip();
	
	//���RB
	clean_RB();
	
	//��������80h
	send_cmd(0x80);
	
	//����2���е�ַ
	send_addr(0x00);
	send_addr(0x00);
	
	//����3���е�ַ
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)&0xff);
	
	//�����ݼĴ���д�����ݣ�дһҳ
	for(i=0;i<4*1024;i++)
	NFDATA=buff[i];
	
	//��������10h
	send_cmd(0x10);
	
	//�ȴ�RB
	wait_RB();
	
	//��������70h
	send_cmd(0x70);
	
	//��ȡд������
	dat=NFDATA;
	
	//ȡ��ѡ��оƬ
	disselect_chip();
	
	return dat;
}