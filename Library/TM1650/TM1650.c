#include "TM1650.h"
 
//--------------------------------------------------------------
// Prototype      : void IIC_Init_TM1650(void)
// Calls          : 
// Description    : 
//--------------------------------------------------------------
void IIC_Init_TM1650(void)
{
   SET_SCL_OUT_TM1650();
   SET_SDA_OUT_TM1650(); 
   SCL_TM1650  = 1;
   SDA_TM1650  = 1;
}

//--------------------------------------------------------------
// Prototype      : void Delay_us(void)
// Description    : 大约延时 z us
//--------------------------------------------------------------
// void TM1650_IIC_Delay_us(u8 z)
// {
//    //u8 i;                   //fcpu 8MHz 时
//    //for (i=50; i>0; i--);
// 	while(z--)
//   {
//     nop();nop();nop();nop();
//   }
// }
//--------------------------------------------------------------
// Prototype      : void I2C_Start(void)
// Calls          : Delay_5us()
// Description    : Start Singnal
//--------------------------------------------------------------
void I2C_Start_TM1650(void)
{
    // SDA 1->0 while SCL High
  	//SCL高电平期间，SDA出现一个下降沿表示起始信号
  	SET_SDA_OUT_TM1650();
    SDA_TM1650 = 1;    	//数据线先保持为高，起始信号要该口的下降沿 
	TM1650_IIC_Delay_us(4);
    SCL_TM1650 = 1;        //时钟线保持为高            
    TM1650_IIC_Delay_us(40);    //有一个大概5us的延时具体以器件而定            
    SDA_TM1650 = 0;        //数据线拉低出现下降沿           
    TM1650_IIC_Delay_us(4);    //延时 一小会，保证可靠的下降沿            
    SCL_TM1650 = 0;        //拉低时钟线，保证接下来数据线允许改变            
}
 
 
//--------------------------------------------------------------
// Prototype      : void I2C_Stop(void)
// Calls          : Delay_5us()
// Description    : Stop Singnal
//-------------------------------------------------------------- 
void I2C_Stop_TM1650(void)
{
    // SDA 0->1 while SCL High
    //SCL高电平期间，SDA产生一个上升沿 表示停止
  	SET_SDA_OUT_TM1650();
	SCL_TM1650 = 0;
	TM1650_IIC_Delay_us(2);
	SDA_TM1650 = 0;		//保证数据线为低电平
	TM1650_IIC_Delay_us(40);
    SCL_TM1650 = 1;		//先保证时钟线为高电平
    TM1650_IIC_Delay_us(10);    //延时 以得到一个可靠的电平信号            
    SDA_TM1650 = 1;        //数据线出现上升沿           
    TM1650_IIC_Delay_us(40);    //延时 保证一个可靠的高电平           
}
 
 
//应答函数
void IIC_Ack_TM1650(void)
{
    //数据线一直保持为低电平，时钟线出现上升沿即为应答
 
	SET_SDA_OUT_TM1650();
	TM1650_IIC_Delay_us(10);
    SDA_TM1650 = 0;
    TM1650_IIC_Delay_us(10);
    SCL_TM1650 = 0;
    TM1650_IIC_Delay_us(40);
	SCL_TM1650 = 1;
	TM1650_IIC_Delay_us(40);
    //应答完成后 将时钟线拉低 允许数据修改
    SCL_TM1650 = 0;
}

//非应答
void IIC_NAck_TM1650(void)
{
    //非应答即相反 与应答区别即为数据线保持高电平即可
	SET_SDA_OUT_TM1650();
	TM1650_IIC_Delay_us(10);
    SDA_TM1650 = 1;
    TM1650_IIC_Delay_us(10);
	SCL_TM1650 = 0;
	TM1650_IIC_Delay_us(40);
    SCL_TM1650 = 1;
    TM1650_IIC_Delay_us(40);
    //最后要将时钟线拉低 允许数据变化
    SCL_TM1650 = 0;
}

//等待应答
uint8_t IIC_Wait_Ack_TM1650(void)//0为有应答，1为无应答
{
    //应答等待计数
    uint8_t ackTime = 0;
    //先将数据线要设置成输入模式本程序未体现，有应答则会出现下降沿
	SCL_TM1650 = 0;
	SET_SDA_OUT_TM1650();
    TM1650_IIC_Delay_us(10);	
	SDA_TM1650 = 1;//
	TM1650_IIC_Delay_us(30);
	SET_SDA_IN_TM1650();//切换为输入模式
	
    //时钟线拉高
    SCL_TM1650 = 1;
    TM1650_IIC_Delay_us(30);
    //等待数据线拉低应答
    while(SDAR_TM1650){
        //如果在该时间内仍未拉低
        ackTime ++;
        if(ackTime > 250)
        {
            //认为非应答 停止信号
            I2C_Stop_TM1650();
            return 1;
        }
    }
    SCL_TM1650 = 0;
    return 0 ;
}
 
void IIC_WrByte_TM1650(uint8_t txd)
{
    //定义一个计数变量
    uint8_t i;
	SET_SDA_OUT_TM1650();
    //将时钟线拉低允许数据改变
//    SCL = 0;
    //按位发送数据
    for(i = 0;i < 8; i ++)
    {
	  	TM1650_IIC_Delay_us(2);
        if((txd&0x80)>>7) //0x80  1000 0000
			SDA_TM1650=1;
		else
			SDA_TM1650=0;
        txd<<=1; 	  
		TM1650_IIC_Delay_us(20);   
		SCL_TM1650=1;
		TM1650_IIC_Delay_us(20);  
		SCL_TM1650=0;	
		TM1650_IIC_Delay_us(20); 
    }
}
 
u8 TM1650_Key(void)	  //按键扫描
{
	u8 i;
	u8 rekey;
	I2C_Start_TM1650();
	IIC_WrByte_TM1650(0x49);//读按键命令
	IIC_Ack_TM1650();
	//DIO_H;
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
	IIC_Ack_TM1650();
	IIC_WrByte_TM1650(dat);
	IIC_Ack_TM1650();
	I2C_Stop_TM1650();
}

void TM1650_Init(void)  //init tm1650
{
	IIC_Init_TM1650();
	delay_ms(50);			//需要延时一小段时间，否则开显示会无响应
	TM1650_Display(0x48,0x31);//初始化为5级灰度，开显示	
}
 
 
 
 
 
 