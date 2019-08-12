//kaito's bootloader

.text
.global _start
_start:
	b reset
	ldr pc, undefined_instructions
	ldr pc, software_interrrupt
	ldr pc, prefetch_abort
	ldr pc, data_abort
	ldr pc, not_used
	ldr pc, irq
	ldr pc, fiq
	
	
undefined_instructions:
	nop
	
software_interrrupt:
	nop
	
prefetch_abort:
	nop
	
data_abort:
	nop

not_used:
	nop
	
irq:
	nop
	
fiq:
	nop
	
reset:
	bl set_svc
	bl set_peri_port
	bl disable_watchdog
	bl disable_interrupt
	bl disable_mmu
	bl init_clock
	bl mem_init
	bl init_stack
	bl nand_init
	bl copy_to_ram
	
	bl clean_bss
//	bl light_led
	ldr pc, =gboot_main	//在这里把pc指针指向gboot_main函数，地址及直接是函数的名字,即在汇编语言中调用c函数
	
set_svc:
	mrs r0, cpsr	//把cpsr寄存器的值读到r0中
	bic r0, r0, #0x1f	//把后5位清零
	orr r0, r0, #0xd3	//赋值成0b10011(svc模式),顺便disable irq和fiq
	msr cpsr, r0	//把r0的值写回cpsr中
	mov pc, lr
	
set_peri_port:		//外设基地址初始化
	ldr r0, =0x70000000
	orr r0, r0, #0x13
	mcr p15,0,r0,c15,c2,4
	mov pc, lr
	
#define pWTCON 0x7e004000
disable_watchdog:
	ldr r0, =pWTCON		//找出WTCON寄存器
	mov r1, #0x0		//全赋0
	str r1, [r0]		//写回WTCON
	mov pc, lr	
	
#define VIC0INTENCLEAR 0x71200014
#define VIC1INTENCLEAR 0x71300014
disable_interrupt:
	ldr r0, =VIC0INTENCLEAR	//找到关闭irq的寄存器，但是vicxintenable只能开不能关
	mov r1, #0xffffffff;	//写入全1，关闭irq
	str r1,[r0];
	
	ldr r0, =VIC1INTENCLEAR
	mov r1, #0xffffffff;
	str r1,[r0];
	mov pc, lr
	
disable_mmu:			//ARM p740
	mcr p15,0,r0,c7,c7,0
	mrc p15,0,r0,c1,c0,0
	bic r0, r0, #0x00000007
	mcr p15,0,r0,c1,c0,0
	mov pc, lr
	
#define APLL_CON 0x7E00F00C
#define	MPLL_CON 0x7E00F010
#define CLK_DIV0 0x7E00F020	
#define OTHERS 0x7E00F900
#define PLL_VAL ((1<<31)|(266<<16)|(3<<8)|(1<<0))
#define DIV_VAL ((0x0<<0)|(0x1<<8)|(0x1<<9)|(0x3<<12))
#define CLK_SRC 0x7e00f01c
init_clock:
	ldr r0, =CLK_DIV0	//设置分频系数
	ldr r1, =DIV_VAL
	str r1, [r0]
	
	ldr r0, =OTHERS
	ldr r1, [r0]
	bic r1, r1, #0xc0
	str r1, [r0]
	
	ldr r0, =APLL_CON	//配置APLL控制寄存器
	ldr r1, =PLL_VAL
	str r1, [r0]
	
	ldr r0, =MPLL_CON	//配置MPLL控制寄存器
	ldr r1, =PLL_VAL
	str r1, [r0]
	
	ldr r0, =CLK_SRC	//配置时钟输出给MUXmpll和MUXapll用p146
	mov r1, #0x3
	str r1, [r0]
	mov pc, lr
	
copy_to_ram:
//	ldr r0, =0x0c000000	//p188 垫脚石的起始地址
//	ldr r1, =0x50008000	//内存起始地址
//	add r3, r0, #1024*4
	
//copy_loop:
//	ldr r2, [r0], #4	//下面4行循环复制
//	str r2, [r1], #4
//	cmp r0, r3
//	bne copy_loop

	mov r0,#0	//C语言中nand_to_ram的第一个参数
	ldr r1,=_start	//第二个参数
	ldr r2,=bss_end	//第三个参数
	sub r2,r2,r1
	
	mov ip,lr
	bl nand_to_ram
	mov lr,ip
	
	mov pc, lr
	
init_stack:
//	ldr sp, =0x54000000	//把sp指针指向栈底
//	mov pc ,lr

	msr cpsr_c, #0xd2
        ldr sp, =0x53000000 //初始化r13_irq
        msr cpsr_c, #0xd3
	ldr sp, =0x54000000  //初始化R13_svc
	mov pc ,lr
	
clean_bss:	
	ldr r0, =bss_start	//读出bss_start起始地址和终止地址（在连接器脚本里面）
	ldr r1, =bss_end
	cmp r0, r1		//比较，若相等，则不需要初始化bss端
	moveq pc, lr
clean_bss_loop:			//循环清空BSS端内容
	mov r2, #0x0
	str r2, [r0], #4
	cmp r0, r1
	bne clean_bss_loop
	mov pc, lr
	
#define GPMCON 0x7F008820
#define GPMDAT 0x7F008824
light_led:
	ldr r0, =GPMCON
	ldr r1, =0x1111
	str r1, [r0]
	
	ldr r0, =GPMDAT
	mov r1, #0x7
	str r1, [r0]
	mov pc, lr	
	
