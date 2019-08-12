#define GPMCON (volatile unsigned long*)0xA0008820	
#define GPMDAT (volatile unsigned long*)0xA0008824
//ʹ��mmuʱ����ַҪ�ĳ������ַ
//���ö�ʽת��

#define MMU_SECDESC (3<<10)|(0<<5)|(1<<4)|(2<<0)
#define MMU_SECDESC_wb (3<<10)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(2<<0)

void create_page_table()
{
	unsigned long *ttb=(unsigned long*)0x50000000;	//��ҳ��Ļ���ַTTBָ���ڴ���ʼ��ַ
	unsigned long vaddr,paddr;			//�����ַ�������ַ����
	
	vaddr=0xA0000000;
	paddr=0x7f000000;	//��Ӧ��GPMCON�������ַ
	*(ttb+(vaddr>>(32-12)))=(paddr&0xfff00000)|MMU_SECDESC;		//д�������Ӧ������
	
	vaddr=0x50000000;
	paddr=0x50000000;
	while(vaddr<0x54000000)
	{
		*(ttb+(vaddr>>(32-12)))=(paddr&0xfff00000)|MMU_SECDESC_wb;	//д�������Ӧ������
		vaddr+=100000;							//ѭ���������ű�д��
		paddr+=100000;
	}
	
}

void mmu_enable()
{
	__asm__(
    
    /*����TTB*/
    "ldr    r0, =0x50000000\n"                  
    "mcr    p15, 0, r0, c2, c0, 0\n"    
    
    /*������Ȩ�޼��*/
    "mvn    r0, #0\n"                   
    "mcr    p15, 0, r0, c3, c0, 0\n"    
    
    
   /*ʹ��MMU*/				
    "mrc    p15, 0, r0, c1, c0, 0\n"    	//����cp15�Ĵ���r1
    "orr    r0, r0, #0x0001\n"          
    "mcr    p15, 0, r0, c1, c0, 0\n"    
    : 
    : 
  );
}

void mmu_init()
{
	create_page_table();
	mmu_enable();
}