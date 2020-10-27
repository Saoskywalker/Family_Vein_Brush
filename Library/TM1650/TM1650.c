#include "TM1650.h"

/*******************官方例程************************/
#define NOP _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();  //宏定义

/************ START信号*******************************/
static void TM1650_START(void)
{
	SCL_TM1650=1;
	SDA_TM1650=1;
	NOP;
	SDA_TM1650=0;
	NOP;
	SCL_TM1650=0;
}
/******************** STOP信号************************/
static void TM1650_STOP(void)
{
	SDA_TM1650=0;
	NOP;
	SCL_TM1650=1;
	NOP;
	SDA_TM1650=1;
	NOP;
	SCL_TM1650=0;
	SDA_TM1650=0;
}
/****************写1个字节给TM1650********************/
static void write_8bit( unsigned char dat)
{
 	unsigned char i;
	SCL_TM1650=0;
	for(i=0;i<8;i++)
		{
		if(dat&0x80)
		{
			SDA_TM1650=1;
			NOP;
			NOP;
			SCL_TM1650=1;
			NOP;
			NOP;
			NOP;
			NOP;
			NOP;
			SCL_TM1650=0;	 
		}
		else
		{
			SDA_TM1650=0;
			NOP;
			NOP;
			SCL_TM1650=1;
			NOP;
			NOP;
			NOP;
			NOP;
			NOP;
			SCL_TM1650=0;
		}	
			dat<<=1;	 
		}
		SDA_TM1650=1;			//ACK信号
		NOP;
		NOP;
		NOP;
		NOP;
		SCL_TM1650=1;
		NOP;
		NOP;
		NOP;
		NOP;
		NOP;
		SCL_TM1650=0;
		NOP;
		NOP;	 
}

/**********************读8bit**************************/
static unsigned char read_8bit(void)
{
	unsigned char dat,i;
	SDA_TM1650=1;
	dat=0;
	for(i=0;i<8;i++)
	{
	SCL_TM1650=1;                        //时钟上沿
	NOP;
	NOP;
	NOP;
	dat<<=1;
	if(SDA_TM1650)
	 dat++;
	SCL_TM1650=0;
	NOP;
	NOP;
	NOP;
	NOP;
	}
	SDA_TM1650=0;			    //ACK信号
	NOP;
	NOP;
	NOP;
	SCL_TM1650=1;
	NOP;
	NOP;
	NOP;
	NOP;
	SCL_TM1650=0;
	NOP;
	
	return dat ;

} 
/*******************读按键命令************************/
// unsigned char TM1650_Key(void) //替换函数
unsigned char TM1650_read(void)
{
	unsigned char key;
	TM1650_START();
    write_8bit(0x49);//读按键指令	
	key=read_8bit();
	TM1650_STOP();
	return key;
} 
/*****************发送命令信号***********************/
// void TM1650_Display(u8 add,u8 dat) //替换函数
void TM1650_send(unsigned char date1,unsigned char date2)
{
 	TM1650_START();
	write_8bit(date1);
	write_8bit(date2);
	TM1650_STOP();
}
/*****************************************/

static void I2C_Start_TM1650(void)
{          
	SET_SDA_OUT_TM1650();
	SCL_TM1650 = 1;
	SDA_TM1650 = 1;
	TM1650_IIC_Delay_us(5);
	SDA_TM1650 = 0;
}
 
//--------------------------------------------------------------
// Prototype      : void I2C_Stop(void)
// Calls          : Delay_5us()
// Description    : Stop Singnal
//-------------------------------------------------------------- 
static void I2C_Stop_TM1650(void)
{   
	SET_SDA_OUT_TM1650();    
	SCL_TM1650 = 1;  
	SDA_TM1650 = 0;
	TM1650_IIC_Delay_us(10);
	SDA_TM1650 = 1;
}
 
//应答函数
static void IIC_Ack_TM1650(void)
{
    //数据线一直保持为低电平，时钟线出现上升沿即为应答
	SET_SDA_OUT_TM1650();
    SDA_TM1650 = 0;
	SCL_TM1650 = 1;
	TM1650_IIC_Delay_us(5);
    //应答完成后 将时钟线拉低 允许数据修改
    SCL_TM1650 = 0;
	TM1650_IIC_Delay_us(5);
}

//非应答
// static void IIC_NAck_TM1650(void)
// {
//     //非应答即相反 与应答区别即为数据线保持高电平即可
// 	SET_SDA_OUT_TM1650();
// 	TM1650_IIC_Delay_us(10);
//     SDA_TM1650 = 1;
//     TM1650_IIC_Delay_us(10);
// 	SCL_TM1650 = 0;
// 	TM1650_IIC_Delay_us(40);
//     SCL_TM1650 = 1;
//     TM1650_IIC_Delay_us(40);
//     //最后要将时钟线拉低 允许数据变化
//     SCL_TM1650 = 0;
// }

//等待应答
static uint8_t IIC_Wait_Ack_TM1650(void)//0为有应答，1为无应答
{
	u8 timeout = 1;

	SET_SDA_IN_TM1650();
    SCL_TM1650 = 1;
    TM1650_IIC_Delay_us(5);
    SCL_TM1650 = 0;
	while((SDAR_TM1650)&&(timeout<=100))
	{
		timeout++;
	}
    TM1650_IIC_Delay_us(5);
    SCL_TM1650 = 0;
	return 0;
}
 
static void IIC_WrByte_TM1650(uint8_t txd)
{
	u8 i;

	SET_SDA_OUT_TM1650();
	SCL_TM1650 = 0;
	TM1650_IIC_Delay_us(1);
	for(i=0;i<8;i++)
	{
		if(txd&0x80)
			SDA_TM1650 = 1;
		else
			SDA_TM1650 = 0;
		
		txd = txd<<1;
		SCL_TM1650 = 0;
		TM1650_IIC_Delay_us(5);
		SCL_TM1650 = 1;
		TM1650_IIC_Delay_us(5);
		SCL_TM1650 = 0;
	}
	TM1650_IIC_Delay_us(5);
}

//--------------------------------------------------------------
// Prototype      : void IIC_Init_TM1650(void)
// Calls          : 
// Description    : 
//--------------------------------------------------------------
void IIC_Init_TM1650(void)
{
	P13_Quasi_Mode; // Quasi-Bidirectional MODE
	P14_Quasi_Mode;
	SET_SCL_OUT_TM1650();
	SET_SDA_OUT_TM1650();
	SCL_TM1650 = 1;
	SDA_TM1650 = 1;
}

u8 TM1650_Key(void)	  //按键扫描
{
	u8 i = 0;
	u8 rekey = 0;
	I2C_Start_TM1650();
	IIC_WrByte_TM1650(0x49);//读按键命令
	IIC_Wait_Ack_TM1650();
	SET_SDA_IN_TM1650();//切换为输入模式
	for(i=0;i<8;i++)
	{
	   SCL_TM1650=1;
	   rekey = rekey<<1;
	   
	   if(SDAR_TM1650)
	   {
	   	rekey++;
	   } 
	   TM1650_IIC_Delay_us(5);
	   SCL_TM1650=0;
	   TM1650_IIC_Delay_us(5);	
	}
	IIC_Ack_TM1650();
	I2C_Stop_TM1650();
	return(rekey);
}
 
void TM1650_Display(u8 add,u8 dat) //数码管显示
{
	//写显存必须从高地址开始写
	I2C_Start_TM1650();
	IIC_WrByte_TM1650(add); //第一个显存地址
	IIC_Wait_Ack_TM1650();
	IIC_WrByte_TM1650(dat);
	IIC_Wait_Ack_TM1650();
	I2C_Stop_TM1650();
}

//light: 0~8
void TM1650_Light(u8 light)
{
	if(light==0)
		TM1650_Display(0x48, 0); //close display
	else if(light<=8)
		TM1650_Display(0x48, light<<4|1); //adjust light
}

//clear display
void TM1650_Clear(void)
{
	TM1650_Display(0X68, 0);	
	TM1650_Display(0X6A, 0);	
	TM1650_Display(0X6C, 0);	
	TM1650_Display(0X6E, 0);
}

//light: light level: value:0=8 value:1~7=1~7; dis: dis 1 or close 0
u8 TM1650_Init(u8 light, u8 dis)  //init tm1650
{
	IIC_Init_TM1650();
	delay_ms(50);			//需要延时一小段时间，否则开显示会无响应
	TM1650_Display(0x48, light<<4|dis); //系统设置, 注意按键检测需开启显示
	TM1650_Clear();	
	if(TM1650_Key()==0XFF)	//check device
		return 1;	//error
	else
		return 0;	//success
}
 