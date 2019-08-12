#define GPMCON (volatile unsigned long*)0xA0008820	
#define GPMDAT (volatile unsigned long*)0xA0008824
//使用mmu时，地址要改成虚拟地址
//采用段式转换

#define MMU_SECDESC (3<<10)|(0<<5)|(1<<4)|(2<<0)
#define MMU_SECDESC_wb (3<<10)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(2<<0)

void create_page_table()
{
	unsigned long *ttb=(unsigned long*)0x50000000;	//把页表的基地址TTB指向内存起始地址
	unsigned long vaddr,paddr;			//虚拟地址和物理地址变量
	
	vaddr=0xA0000000;
	paddr=0x7f000000;	//对应于GPMCON的物理地址
	*(ttb+(vaddr>>(32-12)))=(paddr&0xfff00000)|MMU_SECDESC;		//写表里面对应的内容
	
	vaddr=0x50000000;
	paddr=0x50000000;
	while(vaddr<0x54000000)
	{
		*(ttb+(vaddr>>(32-12)))=(paddr&0xfff00000)|MMU_SECDESC_wb;	//写表里面对应的内容
		vaddr+=100000;							//循环，把整张表写好
		paddr+=100000;
	}
	
}

void mmu_enable()
{
	__asm__(
    
    /*设置TTB*/
    "ldr    r0, =0x50000000\n"                  
    "mcr    p15, 0, r0, c2, c0, 0\n"    
    
    /*不进行权限检查*/
    "mvn    r0, #0\n"                   
    "mcr    p15, 0, r0, c3, c0, 0\n"    
    
    
   /*使能MMU*/				
    "mrc    p15, 0, r0, c1, c0, 0\n"    	//配置cp15寄存器r1
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