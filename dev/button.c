#define GPNCON (volatile unsigned long*)0x7F008830

void button_init()
{
	*GPNCON = 0xAAA;	//设置GPIO为外部中断
}
