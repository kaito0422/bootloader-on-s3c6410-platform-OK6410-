#define ADCCON	 	*((volatile unsigned long*)0x7E00B000)
#define ADCTSC		*((volatile unsigned long*)0x7E00B004)
#define ADCDAT0 	*((volatile unsigned long*)0x7E00B00C)
#define ADCDAT1 	*((volatile unsigned long*)0x7E00B010)
#define ADCCLRINT 	*((volatile unsigned char*)0x7E00B018)
#define ADCCLRINTPNDNUP *((volatile unsigned char*)0x7E00B020)
#define ADCUPDN 	*((volatile unsigned char*)0x7E00B014)
#define VIC1INTENABLE	*((volatile unsigned long*)0x71300010)
#define VIC0INTENABLE	*((volatile unsigned long*)0x71200010)
#define VIC0ADDRESS 	*((volatile unsigned long*)0x71200F00)
#define VIC1ADDRESS 	*((volatile unsigned long*)0x71300F00)

unsigned char p[13]="I am iron man";

void gboot_main()
{
	int num, j = 1000000;
	
	unsigned char i=0;
	unsigned char temp[4*1024];
	
#ifdef MMU_ON
	mmu_init();
#endif
	
	led_off();
	button_init();
	irq_init();

	uart_init();
	
	dma_init();
	dma_start();
	
	lcd_init();
	lcd_test();
	
	putc('K');
	
	ts_init();
	
/*	
	printf("\n\r***************GBOOT**************\n\r");
	printf("1.Barry!\n\r");
	printf("2.Tony!\n\r");
	printf("3.Kaito!\n\r");
	printf("**********************************");
*/
	printf("\n\r please touch the screen:");
	
	
	while(1)
	{
/*		j = 1000000;
		led_on();
		while(j--);
		j = 1000000;
		led_off();
		while(j--);
*/		
	}

}