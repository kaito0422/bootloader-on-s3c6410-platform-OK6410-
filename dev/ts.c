/******************************************************************************************
1.检测到ts中断
2.开启x，y坐标自动转换
3.等待转换完成
4.获取坐标
5.求出LCD显示坐标
6.显示
7.循环检测弹起中断
    若不弹起：则继续获取x，y坐标
    若弹起：  则退出ts_handle
8.清除中断标志位
******************************************************************************************/
#define ADCCON	 	*((volatile unsigned long*)0x7E00B000)
#define ADCTSC		*((volatile unsigned long*)0x7E00B004)
#define ADCDAT0 	*((volatile unsigned long*)0x7E00B00C)
#define ADCDAT1 	*((volatile unsigned long*)0x7E00B010)
#define ADCCLRINT 	*((volatile unsigned char*)0x7E00B018)
#define ADCCLRINTPNDNUP *((volatile unsigned char*)0x7E00B020)
#define ADCUPDN 	*((volatile unsigned char*)0x7E00B014)
#define VIC1INTENABLE	*((volatile unsigned long*)0x71300010)
#define VIC0INTENABLE	*((volatile unsigned long*)0x71200010)
#define ADCDLY 		*((volatile unsigned long*)0x7E00B008)
#define VIC0ADDRESS 	*((volatile unsigned long*)0x71200F00)
#define VIC1ADDRESS 	*((volatile unsigned long*)0x71300F00)

#define RED	0xFF3030;
#define GREEN	0x9ACD32;
#define BLUE	0x436EEE;

int xdat = 0, ydat = 0;
int kbt = 0;
int xold=0, yold=0;
unsigned long COLOR = 0xFF3030;

void change_color(unsigned long x)
{
	COLOR = x;
}

void delay()
{
	int i, j;
	i=100;
	while(i--)
	{
		j=90;
		while(j--);
	}
}

void ts_init()
{
	ADCCON=(1<<14)|(0<<6);
	
	ADCCLRINT=0;
	ADCCLRINTPNDNUP=0;
	
	ADCDLY=0xffff;
	
	VIC1INTENABLE=(0b1<<30);

	ADCTSC=0xd3;
//	ADCUPDN = 0b00;
}

/* draw a point on the screen */
void draw_point(int x, int y)
{
	int i;
	for(i = 0; i < 3; i++)
	{
		point(y+0, x+i, COLOR);
		point(y+1, x+i, COLOR);
		point(y+2, x+i, COLOR);
	}
}

/* to make the flag more obvious */
void dfs(int first_x, int first_y, int second_x, int second_y)
{
	kbt++;
	if(kbt > 12)
	return;
	int tmp, x, y, xx, yy;
	x = (first_x + second_x)/2;
	y = (first_y + second_y)/2;
	x = x > 0 ? x : (-x);
	y = y > 0 ? y : (-y);
	xx = first_x - second_x;
	yy = first_y - second_y;
	xx = xx > 0 ? xx : (-xx);
	yy = yy > 0 ? yy : (-yy);
	
	if(!((xx < 3) || (yy < 3)))
	{
		draw_point(x, y);
		dfs(first_x, first_y, x, y);
		dfs(x, y, second_x, second_y);
	}
	else
		return;
}

void draw_line_2(int first_x, int first_y, int second_x, int second_y)
{
	int dx=0, dy=0, x=0, y=0;
	int k=1;
	
	dx=second_x-first_x;
	dy=second_y-first_y;
	
	while(dx*k<dy)
		k++;
	
	for(dx=3;dx<(second_x-first_x);dx=dx+3)
	{
		x=first_x+dx;
		y=first_y+dx*k;
		draw_point(x,y);
	}
}

void connect_lint(int first_x, int first_y, int second_x, int second_y)
{
	int x, y, x1, y1, x2, y2;
	int x3,x4,x5,x6;
	int y3,y4,y5,y6;
	
	x=(second_x+first_x)/2;
	y=(second_y+first_y)/2;
	x1=(x+first_x)/2;
	y1=(y+first_y)/2;
	x2=(x+second_x)/2;
	y2=(y+second_y)/2;
	
	x3=(first_x+x1)/2;
	x4=(x+x1)/2;
	x5=(x+x2)/2;
	x6=(x2+second_x)/2;
	
	y3=(first_y+y1)/2;
	y4=(y1+y)/2;
	y5=(y+y2)/2;
	y6=(y2+second_y)/2;
	
	draw_point(x,y);
	draw_point(x1,y1);
	draw_point(x2,y2);
	draw_point(x3,y3);
	draw_point(x4,y4);
	draw_point(x5,y5);
	draw_point(x6,y6);
}

void thumb(void)
{
    int x0, y0, x1, y1;

    xold = xdat;
	yold = ydat;
	x0 = (xdat - 190)*480/(850 - 190);
	y0 = (ydat - 345)*275/(690 - 345);

	ADCTSC=(1<<2);
	ADCCON=(0b1<<0)|(1<<14)|(49<<6);
    while((ADCCON&(0b1<<15)) == 0);

    xdat=ADCDAT0&0x3ff;
	ydat=ADCDAT1&0x3ff;

    if(xdat-xold>5 || xold-xdat>5 || ydat-yold>5 || yold-ydat>5)
		return ;
	else
	{
		xold = xdat;
		yold = ydat;		
	}

    x1 = (xdat - 190)*480/(850 - 190);
	y1 = (ydat - 345)*275/(690 - 345);

    kbt = 0;
//	dfs(x0, y0, x1, y1);
//	draw_line_2(x0, y0, x1, y1);
	connect_lint(x0, y0, x1, y1);
}

void ts_handler()
{
    int x1, y1;

    /* turn on auto transmission */
    ADCTSC = (1<<2);
	ADCCON = (0b1<<0) | (1<<14) | (49<<6);

    /* wait for the transmission */
    while((ADCCON&(0b1<<15)) == 0);

    /* get the x, y data */
    xdat = ADCDAT0&0x3ff;
	ydat = ADCDAT1&0x3ff;

    /* get the LCD location */
    x1 = (xdat - 190)*480/(850 - 190);
	y1 = (ydat - 345)*275/(690 - 345);

    draw_point(x1, y1);

    ADCTSC=0x1d3;

    while(1)
    {
    	if(((ADCUPDN&(0b1<<1)) != 0))	//检测到笔尖弹起
    	break;
        thumb();
        led_off();
    	ADCTSC=0x1d3;
    	delay();
    }
    
    ADCUPDN = 0;

    /* clear the ts flags */
    ADCCLRINT = 0;
	ADCCLRINTPNDNUP = 0;
	ADCUPDN = 0;
	VIC0ADDRESS = 0;
	VIC1ADDRESS = 0;

    /* configure to wait for the next ts */
    ADCTSC = 0xd3;
}