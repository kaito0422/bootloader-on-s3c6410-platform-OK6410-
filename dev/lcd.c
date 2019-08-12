#define GPICON 		*((volatile unsigned long*)0x7F008100)
#define GPJCON 		*((volatile unsigned long*)0x7F008120)

#define MIFPCON		*((volatile unsigned long*)0x7410800C)
#define SPCON 		*((volatile unsigned long*)0x7F0081A0)
#define VIDCON0		*((volatile unsigned long*)0x77100000)
#define VIDCON1 	*((volatile unsigned long*)0x77100004)
#define VIDTCON0	*((volatile unsigned long*)0x77100010)
#define VIDTCON1  	*((volatile unsigned long*)0x77100014)
#define VIDTCON2	*((volatile unsigned long*)0x77100018)
#define WINCON0		*((volatile unsigned long*)0x77100020)
#define VIDOSD0A	*((volatile unsigned long*)0x77100040)
#define VIDOSD0B	*((volatile unsigned long*)0x77100044)
#define VIDOSD0C	*((volatile unsigned long*)0x77100048)
#define VIDW00ADD0B0	*((volatile unsigned long*)0x771000A0)
#define VIDW00ADD1B0	*((volatile unsigned long*)0x771000D0)

#define WPALCON 	*((volatile unsigned long *)0x771001A0)

#define FRAME_BUFFER	0x54000000
 
#define VBPD 1
#define VFPD 1
#define VSPW 9
#define HBPD 1
#define HFPD 1
#define HSPW 40

#define LINEVAL 271
#define HOZVAL 479

#define LeftTopX 0
#define LeftTopY 0
#define RightBotX 479
#define RightBotY 271

#define FRAME_BUFFER 0x54000000

extern unsigned char bmp[387368];

void lcd_port_init()
{
	GPICON=0xaaaaaaaa;
	GPJCON=0xaaaaaaaa;
}

void lcd_contral_init()
{
	//[3]Select (mux) control for LCD bypass Normal mode     ;	Select LCD I/F pin configure 01 = RGB I/F style
	MIFPCON=(0<<3);
	SPCON=0x01;
	
	//初始化使用的时钟HCLK到LCD，配置HSYNC和VSYNC位倒转（因为和三星的默认使用不同）
	VIDCON0=(14<<6)|(1<<4)|(1<<1)|(1<<0);
	VIDCON1=(1<<6)|(1<<5);
	
	//配置VBPD,VFPD,VSPW,HBPD,HFPD,HSPW,LINEVAL,HOZVAL
	VIDTCON0=(VBPD<<16)|(VFPD<<8)|(VSPW<<0);
	VIDTCON1=(HBPD<<16)|(HFPD<<8)|(HSPW<<0);
	VIDTCON2=(LINEVAL<<11)|(HOZVAL<<0);
	
	//[16]Half-Word swap enable   [2]Select the BPP	   [0]Enable the video output and the VIDEO control signal
	WINCON0=(0<<17)|(0<<18)|(0<<16)|(0xb<<2)|(1<<0);
	
	//设置屏幕的起始地址（最左上角），终止地址（最左下角）,屏幕大小
	VIDOSD0A=(LeftTopX<<11)|(LeftTopY<<0);
	VIDOSD0B=(RightBotX<<11)|(RightBotY<<0);
	VIDOSD0C=(LINEVAL+1)|(HOZVAL+1);
	
	//初始化写入起始地址和终止地址
	VIDW00ADD0B0=FRAME_BUFFER;
	VIDW00ADD1B0=((FRAME_BUFFER+(HOZVAL+1)*4*(LINEVAL+1)*4));
	
	WPALCON=0b001;	//110
}

void point(int row,int col,int color)
{
	unsigned int red,green,blue;
	unsigned long *point=(unsigned long*)FRAME_BUFFER;
	
	red=(color>>16)&0xff;
	green=(color>>8)&0xff;
	blue=(color>>0)&0xff;
	
	*(point + row*480 + col) = color;
}

void draw_bmp()
{
	int i,j,t=8;
//	unsigned char *p = (unsigned char *)bmp;
	int red,green,blue;
	int color;
	
	for(i=0;i<272;i++)
	{
		for(j=0;j<480;j++)
		{
			red=bmp[t++];
			green=bmp[t++];
			blue=bmp[t++];
			color=(red<<16)|(green<<8)|(blue<<0);
			
			point(i,j,color);
		}
	}
}

void draw_white()
{
	int i,j,t=8;
//	unsigned char *p = (unsigned char *)bmp;
	int red,green,blue;
	int color;
	
	for(i=0;i<272;i++)
	{
		for(j=0;j<480;j++)
		{
			red=0xFF;
			green=0xFF;
			blue=0xFF;
			color=(red<<16)|(green<<8)|(blue<<0);
			
			point(i,j,color);
		}
	}
}

void lcd_init()
{
	lcd_port_init();
	lcd_contral_init();
}


void lcd_test()
{
	int x,y;
	
	draw_bmp();

/*	
	for(y=1;y<470;y++){
		point(110,y,0x9CD3D8);
		point(130,y,0x9CD3D8);
		point(150,y,0x9CD3D8);
	}
*/
/*	
	for(x=0;x<480;x=x+10)
	for(y=0;y<271;y++)
	{
	//	point(x,160,0x7FFF00);
		point(y,x,0xEEEE00);
	}
*/

}