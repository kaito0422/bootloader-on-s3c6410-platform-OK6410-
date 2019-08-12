.text
.global mem_init
mem_init:
@set data pin
ldr r0, =0x7e00f120
mov r1, #0x0
str r1, [r0]

ldr r0, =0x7e001004
mov r1, #0x4
str r1, [r0]


    ldr r0, =0x7e001010  @refresh period register
    ldr r1, =( ( 7800 / ( 1000000000/133000000 ) + 1 ) )      @设置周期
    str r1, [r0]

    ldr r0, =0x7e001014 @CAS latency  set 3 
    mov r1, #0x6
    str r1, [r0]

    ldr r0, =0x7e001018  @set DQSS 1  range from 0.75 to 1.25
    mov r1, #0x1
    str r1, [r0]

    ldr r0, =0x7e00101c  @set MRD 2 only 2
    mov r1, #0x2
    str r1, [r0]

    ldr r0, =0x7e001020   @set RAS 45us
    ldr r1, =(45/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e001024   @set RC 68us
    ldr r1, =(68/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e001028   @set DRC 23us
    ldr r1, =(23/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e00102c   @set RFC 80us
    ldr r1, =(80/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e001030   @set RP 23us
    ldr r1, =(23/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e001034   @set RRD 15us
    ldr r1, =(15/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e001038   @set WR 15us
    ldr r1, =(15/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e00103c   @t_wtr瀵勫瓨鍣?
    mov r1, #0x07
    str r1, [r0]

    ldr r0, =0x7e001040   @t_xp瀵勫瓨鍣?
    mov r1, #0x02
    str r1, [r0]

    ldr r0, =0x7e001044   @set XSR 120us
    ldr r1, =(120/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e001048   @set ESR 120us
    ldr r1, =(120/(1000/133)+1)
    str r1, [r0]

    ldr r0, =0x7e00100c   @memory configuration
    ldr r1, =0x00010012   @set bit 16 4 1 0
    str r1, [r0]

    ldr r0, =0x7e00104c   @memory configuration2
    ldr r1, =0x0b45  @set bit just follow the table in the manual
    str r1, [r0]

    ldr r0, =0x7e001200   @设置片选 bank-row内存地址50000000
    ldr r1, =0x150f8  @第8位用来屏蔽地址
    str r1, [r0]

    ldr r0, =0x7e001304   @全选0，使用最小延时
    mov r1, #0x0
    str r1, [r0]

@issue NOP
ldr r0, =0x7e001008
ldr r1, =0xc0000
str r1, [r0]
@issue prechargeall
ldr r0, =0x7e001008
ldr r1, =0x0
str r1, [r0]
@issue autorefresh
ldr r0, =0x7e001008
ldr r1, =0x4000
str r1, [r0]
@issue autorefresh
ldr r0, =0x7e001008
ldr r1, =0x4000
str r1, [r0]
@issue mrs
ldr r0, =0x7e001008
ldr r1, =0x0a0000
str r1, [r0]
@issue mrs
ldr r0, =0x7e001008
ldr r1, =0x0800032
str r1, [r0]
ldr r0, =0x7e001004
mov r1, #0x0
str r1, [r0]
check_ready:
ldr r0, =0x7e001000
ldr r1, [r0]
mov r2, #0x3
and r1,r1,r2
cmp r1, #0x1
bne check_ready
mov pc, lr
