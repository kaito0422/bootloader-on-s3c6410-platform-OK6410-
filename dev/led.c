#define GPMCON (volatile unsigned long*)0x7f008820	
#define GPMDAT (volatile unsigned long*)0x7f008824

void led_on()
{
	*GPMCON=0x1111;
	*GPMDAT=0x9;
}

void led_off()
{
	*GPMCON=0x1111;
	*GPMDAT=0xf;
}