#define EINT0CON0 (volatile unsigned long*)0x7F008900
#define EINT0MASK (volatile unsigned long*)0x7F008920
#define EINT0PEND (volatile unsigned long*)0x7F008924

#define VIC0INTENABLE (volatile unsigned long*)0x71200010
#define VIC0VECTADDR0 (volatile unsigned long*)0x71200100
#define VIC0VECTADDR1 (volatile unsigned long*)0x71200104
#define VIC0ADDRESS (volatile unsigned long*)0x71200F00
#define VIC1ADDRESS (volatile unsigned long*)0x71300F00
#define VIC1VECTADDR30   *((volatile unsigned long*)0x71300178)

#define GPNDAT	*((unsigned long volatile *)0x7F008834)
#define GPFCON  *((unsigned long volatile *)0x7F0080A4)

#define ADCTSC		*((volatile unsigned long*)0x7E00B004)
#define ADCUPDN 	*((volatile unsigned char*)0x7E00B014)

#define RED	0xFF3030
#define GREEN	0x9ACD32
#define BLUE	0x436EEE

void irq1()
{
	__asm__( 
    
    "sub lr, lr, #4\n"  
    "stmfd sp!, {r0-r12, lr}\n"       
    : 
    : 
   );
	
	int i = 1000000;
//    led_off();
	if(((*EINT0PEND)&0b1) != 0)
	{
		led_on();
		i = 1000000;
		change_color(RED);
		while(i--);
		led_off();
	}
	else if(((*EINT0PEND)&0b10) != 0)
	{
		led_on();
		i = 1000000;
		change_color(GREEN);
		while(i--);
		led_off();
	}
	else if(((*EINT0PEND)&0b100) != 0)
	{
		led_on();
		i = 1000000;
		change_color(BLUE);
		while(i--);
		led_off();
	}
	else if(((*EINT0PEND)&0b1000) != 0)
	{
		draw_bmp();
	}
	
    /* ����ж� */
    *EINT0PEND = ~0x0;  
    *VIC0ADDRESS = 0; 
    *VIC1ADDRESS = 0; 
   
    __asm__( 
    "ldmfd sp!, {r0-r12, pc}^ \n"       
    : 
    : 
  );
}

void irq2()
{
	__asm__( 
    
    "sub lr, lr, #4\n"  
    "stmfd sp!, {r0-r12, lr}\n"       
    : 
    : 
   );

	
	if(((*EINT0PEND)&0b10000) != 0)
	{
		draw_white();
	}
	else if(((*EINT0PEND)&0b100000) != 0)
	led_off();

	
    /* ����ж� */
    *EINT0PEND = ~0x0;  
    *VIC0ADDRESS = 0; 
    *VIC1ADDRESS = 0; 
   
    __asm__( 
    "ldmfd sp!, {r0-r12, pc}^ \n"       
    : 
    : 
  );
}

void irq5()
{
	__asm__( 
    
    "sub lr, lr, #4\n"  
    "stmfd sp!, {r0-r12, lr}\n"       
    : 
    : 
   );

//    led_on();
	ts_handler();

/*
	ADCTSC=0x1d3;
	while((ADCUPDN&(0b1<<1)) == 0);
	led_on();
*/	
    /* ����ж� */
    *EINT0PEND = ~0x0;  
    *VIC0ADDRESS = 0; 
    *VIC1ADDRESS = 0; 
   
    __asm__( 
    "ldmfd sp!, {r0-r12, pc}^ \n"       
    : 
    : 
  );
}

void irq_init()
{
	*EINT0CON0=(0b010<<0)|(0b010<<4)|(0b010<<8);		//�����ⲿ�жϵ�16���ж��е�EXINT0��EXINT4λ�½��ش����ж�
	*EINT0MASK=0;					//ȡ������EXINT��mask����
	
	/***
	��OK6410���棨s3c6410 p410���������ж�Դ��VIC0��VIC1��
	��ͬ���ж϶�Ӧ�ڲ�ͬ���ж�Դ������������ʽ�����ж�ʱҲ�᲻һ��
	��EXINT����0~3����EINT0��4~11����EINT1
	***/
	
	*VIC0INTENABLE|=(0b1<<0)|(0b1<<1);	//ʹ��VIC0��Ӧ���жϣ��ܹ�32��������ֻʹ��������
	
	*VIC0VECTADDR0=irq1;			//�û�����keyʱ��CPU�ͻ��Զ��Ľ�VIC0VECTADDR0��ֵ����VIC0ADDRESS����ת�������ַȥִ
	*VIC0VECTADDR1 = irq2;
//	*VIC0VECTADDR1=irq5;
	VIC1VECTADDR30=irq5; 			
	
	//����VIC��ʹ��VIC�ж�
	
    __asm__( 
    
    "mrc p15,0,r0,c1,c0,0\n"
    "orr r0,r0,#(1<<24)\n"
    "mcr p15,0,r0,c1,c0,0\n"

    "mrs r0,cpsr\n"
    "bic r0, r0, #0x80\n"
    "msr cpsr_c, r0\n"            
    : 
    : 
  );
  
}