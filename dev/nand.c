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
	//选中芯片
	select_chip();
	
	//清除RB
	clean_RB();
	
	//发送复位信号0xff
	send_cmd(0xff);
	
	//等待RB从0变1
	wait_RB();
	
	//取消选中芯片
	disselect_chip();
}

void nand_init()
{
/*
HCLK的频率为100MHZ，周期就为10ns
TACLS > 0 ns
TWRPH0	> 15ns
TWRPH1 > 5ns
 
TACLS的值 = HCLK x TACLS > 0ns
TWRPH0的值 = HCLK x (TWRPH0 + 1) > 15ns
TWRPH1的值 = HCLK x (TWRPH1 +1) > 5ns 
*/
#define TACLS  1
#define TWRPH0 2
#define TWRPH1 1
	//设置时间参数
	NFCONF&=~((7<<12)|(7<<8)|(7<<4));		//先清零
	NFCONF|=(TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	
	//使能nandflash controller
	NFCONT=1|(1<<1);
	
	//复位
	nand_reset();
}

void nf_pageread(unsigned long addr,unsigned char* buff)
{
	int i;
	
	//选中芯片
	select_chip();
	
	//清除RB
	clean_RB();
	
	//发送命令0x00
	send_cmd(0x00);
	
	//发送列地址
	send_addr(0x00);
	send_addr(0x00);
	
	//发送行地址
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)&0xff);
	
	//发送命令0x30
	send_cmd(0x30);
	
	//等待RB信号从0变成1（空闲）
	wait_RB();
	
	//读数据
	for(i=0;i<4*1024;i++)
	buff[i]=NFDATA;
	
	//取消选中芯片
	disselect_chip();
}

void nand_to_ram(unsigned long start_addr,unsigned char* sdram_addr,int size)
{
	int i;	//i为页号、sdram_addr为内存中的位置、size拷贝数据的大小
	
/*
   S3C6410启动时拷贝的8K代码不是存储在Nand flash的第一页上，
   而是存储在Nand flash的前4页上，每页2K，总共8K，
  
*/
	
	for(i=0;i<4;i++,sdram_addr+=1024*2)
	{
		nf_pageread(i,sdram_addr);	//把nandflash里面地址为i的内容读到内存的sdram_addr的位置处
	}
	
	size-=1024*8;	//要拷贝的数据已经有8K拷贝完了
	
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
	
	//选中芯片
	select_chip();
	
	//清除RB
	clean_RB();
	
	//发送命令60h
	send_cmd(0x60);
	
	//发送3个行地址
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)&0xff);
	
	//发送命令d0h
	send_cmd(0xd0);
	
	//等待RB
	wait_RB();
	
	//发送命令70h
	send_cmd(0x70);
	
	//读取擦除结果
	dat=NFDATA;
	
	//取消选择芯片
	disselect_chip();
	
	return dat;
}

int nf_pagewrite(unsigned long addr,unsigned char* buff)
{
	int i,dat;
	
	//选中芯片
	select_chip();
	
	//清除RB
	clean_RB();
	
	//发送命令80h
	send_cmd(0x80);
	
	//发送2个列地址
	send_addr(0x00);
	send_addr(0x00);
	
	//发送3个行地址
	send_addr(addr&0xff);
	send_addr((addr>>8)&0xff);
	send_addr((addr>>16)&0xff);
	
	//往数据寄存器写入数据，写一页
	for(i=0;i<4*1024;i++)
	NFDATA=buff[i];
	
	//发送命令10h
	send_cmd(0x10);
	
	//等待RB
	wait_RB();
	
	//发送命令70h
	send_cmd(0x70);
	
	//读取写入数据
	dat=NFDATA;
	
	//取消选中芯片
	disselect_chip();
	
	return dat;
}