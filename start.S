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
	ldr pc, =gboot_main	//�������pcָ��ָ��gboot_main��������ַ��ֱ���Ǻ���������,���ڻ�������е���c����
	
set_svc:
	mrs r0, cpsr	//��cpsr�Ĵ�����ֵ����r0��
	bic r0, r0, #0x1f	//�Ѻ�5λ����
	orr r0, r0, #0xd3	//��ֵ��0b10011(svcģʽ),˳��disable irq��fiq
	msr cpsr, r0	//��r0��ֵд��cpsr��
	mov pc, lr
	
set_peri_port:		//�������ַ��ʼ��
	ldr r0, =0x70000000
	orr r0, r0, #0x13
	mcr p15,0,r0,c15,c2,4
	mov pc, lr
	
#define pWTCON 0x7e004000
disable_watchdog:
	ldr r0, =pWTCON		//�ҳ�WTCON�Ĵ���
	mov r1, #0x0		//ȫ��0
	str r1, [r0]		//д��WTCON
	mov pc, lr	
	
#define VIC0INTENCLEAR 0x71200014
#define VIC1INTENCLEAR 0x71300014
disable_interrupt:
	ldr r0, =VIC0INTENCLEAR	//�ҵ��ر�irq�ļĴ���������vicxintenableֻ�ܿ����ܹ�
	mov r1, #0xffffffff;	//д��ȫ1���ر�irq
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
	ldr r0, =CLK_DIV0	//���÷�Ƶϵ��
	ldr r1, =DIV_VAL
	str r1, [r0]
	
	ldr r0, =OTHERS
	ldr r1, [r0]
	bic r1, r1, #0xc0
	str r1, [r0]
	
	ldr r0, =APLL_CON	//����APLL���ƼĴ���
	ldr r1, =PLL_VAL
	str r1, [r0]
	
	ldr r0, =MPLL_CON	//����MPLL���ƼĴ���
	ldr r1, =PLL_VAL
	str r1, [r0]
	
	ldr r0, =CLK_SRC	//����ʱ�������MUXmpll��MUXapll��p146
	mov r1, #0x3
	str r1, [r0]
	mov pc, lr
	
copy_to_ram:
//	ldr r0, =0x0c000000	//p188 ���ʯ����ʼ��ַ
//	ldr r1, =0x50008000	//�ڴ���ʼ��ַ
//	add r3, r0, #1024*4
	
//copy_loop:
//	ldr r2, [r0], #4	//����4��ѭ������
//	str r2, [r1], #4
//	cmp r0, r3
//	bne copy_loop

	mov r0,#0	//C������nand_to_ram�ĵ�һ������
	ldr r1,=_start	//�ڶ�������
	ldr r2,=bss_end	//����������
	sub r2,r2,r1
	
	mov ip,lr
	bl nand_to_ram
	mov lr,ip
	
	mov pc, lr
	
init_stack:
//	ldr sp, =0x54000000	//��spָ��ָ��ջ��
//	mov pc ,lr

	msr cpsr_c, #0xd2
        ldr sp, =0x53000000 //��ʼ��r13_irq
        msr cpsr_c, #0xd3
	ldr sp, =0x54000000  //��ʼ��R13_svc
	mov pc ,lr
	
clean_bss:	
	ldr r0, =bss_start	//����bss_start��ʼ��ַ����ֹ��ַ�����������ű����棩
	ldr r1, =bss_end
	cmp r0, r1		//�Ƚϣ�����ȣ�����Ҫ��ʼ��bss��
	moveq pc, lr
clean_bss_loop:			//ѭ�����BSS������
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
	