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
// Description    : ��Լ��ʱ z us
//--------------------------------------------------------------
// void TM1650_IIC_Delay_us(u8 z)
// {
//    //u8 i;                   //fcpu 8MHz ʱ
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
  	//SCL�ߵ�ƽ�ڼ䣬SDA����һ���½��ر�ʾ��ʼ�ź�
  	SET_SDA_OUT_TM1650();
    SDA_TM1650 = 1;    	//�������ȱ���Ϊ�ߣ���ʼ�ź�Ҫ�ÿڵ��½��� 
	TM1650_IIC_Delay_us(4);
    SCL_TM1650 = 1;        //ʱ���߱���Ϊ��            
    TM1650_IIC_Delay_us(40);    //��һ�����5us����ʱ��������������            
    SDA_TM1650 = 0;        //���������ͳ����½���           
    TM1650_IIC_Delay_us(4);    //��ʱ һС�ᣬ��֤�ɿ����½���            
    SCL_TM1650 = 0;        //����ʱ���ߣ���֤����������������ı�            
}
 
 
//--------------------------------------------------------------
// Prototype      : void I2C_Stop(void)
// Calls          : Delay_5us()
// Description    : Stop Singnal
//-------------------------------------------------------------- 
void I2C_Stop_TM1650(void)
{
    // SDA 0->1 while SCL High
    //SCL�ߵ�ƽ�ڼ䣬SDA����һ�������� ��ʾֹͣ
  	SET_SDA_OUT_TM1650();
	SCL_TM1650 = 0;
	TM1650_IIC_Delay_us(2);
	SDA_TM1650 = 0;		//��֤������Ϊ�͵�ƽ
	TM1650_IIC_Delay_us(40);
    SCL_TM1650 = 1;		//�ȱ�֤ʱ����Ϊ�ߵ�ƽ
    TM1650_IIC_Delay_us(10);    //��ʱ �Եõ�һ���ɿ��ĵ�ƽ�ź�            
    SDA_TM1650 = 1;        //�����߳���������           
    TM1650_IIC_Delay_us(40);    //��ʱ ��֤һ���ɿ��ĸߵ�ƽ           
}
 
 
//Ӧ����
void IIC_Ack_TM1650(void)
{
    //������һֱ����Ϊ�͵�ƽ��ʱ���߳��������ؼ�ΪӦ��
 
	SET_SDA_OUT_TM1650();
	TM1650_IIC_Delay_us(10);
    SDA_TM1650 = 0;
    TM1650_IIC_Delay_us(10);
    SCL_TM1650 = 0;
    TM1650_IIC_Delay_us(40);
	SCL_TM1650 = 1;
	TM1650_IIC_Delay_us(40);
    //Ӧ����ɺ� ��ʱ�������� ���������޸�
    SCL_TM1650 = 0;
}

//��Ӧ��
void IIC_NAck_TM1650(void)
{
    //��Ӧ���෴ ��Ӧ������Ϊ�����߱��ָߵ�ƽ����
	SET_SDA_OUT_TM1650();
	TM1650_IIC_Delay_us(10);
    SDA_TM1650 = 1;
    TM1650_IIC_Delay_us(10);
	SCL_TM1650 = 0;
	TM1650_IIC_Delay_us(40);
    SCL_TM1650 = 1;
    TM1650_IIC_Delay_us(40);
    //���Ҫ��ʱ�������� �������ݱ仯
    SCL_TM1650 = 0;
}

//�ȴ�Ӧ��
uint8_t IIC_Wait_Ack_TM1650(void)//0Ϊ��Ӧ��1Ϊ��Ӧ��
{
    //Ӧ��ȴ�����
    uint8_t ackTime = 0;
    //�Ƚ�������Ҫ���ó�����ģʽ������δ���֣���Ӧ���������½���
	SCL_TM1650 = 0;
	SET_SDA_OUT_TM1650();
    TM1650_IIC_Delay_us(10);	
	SDA_TM1650 = 1;//
	TM1650_IIC_Delay_us(30);
	SET_SDA_IN_TM1650();//�л�Ϊ����ģʽ
	
    //ʱ��������
    SCL_TM1650 = 1;
    TM1650_IIC_Delay_us(30);
    //�ȴ�����������Ӧ��
    while(SDAR_TM1650){
        //����ڸ�ʱ������δ����
        ackTime ++;
        if(ackTime > 250)
        {
            //��Ϊ��Ӧ�� ֹͣ�ź�
            I2C_Stop_TM1650();
            return 1;
        }
    }
    SCL_TM1650 = 0;
    return 0 ;
}
 
void IIC_WrByte_TM1650(uint8_t txd)
{
    //����һ����������
    uint8_t i;
	SET_SDA_OUT_TM1650();
    //��ʱ���������������ݸı�
//    SCL = 0;
    //��λ��������
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
 
u8 TM1650_Key(void)	  //����ɨ��
{
	u8 i;
	u8 rekey;
	I2C_Start_TM1650();
	IIC_WrByte_TM1650(0x49);//����������
	IIC_Ack_TM1650();
	//DIO_H;
	SET_SDA_IN_TM1650();//�л�Ϊ����ģʽ
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
 
void TM1650_Display(u8 add,u8 dat) //�������ʾ
{
	//д�Դ����Ӹߵ�ַ��ʼд
	I2C_Start_TM1650();
	IIC_WrByte_TM1650(add); //��һ���Դ��ַ
	IIC_Ack_TM1650();
	IIC_WrByte_TM1650(dat);
	IIC_Ack_TM1650();
	I2C_Stop_TM1650();
}

void TM1650_Init(void)  //init tm1650
{
	IIC_Init_TM1650();
	delay_ms(50);			//��Ҫ��ʱһС��ʱ�䣬������ʾ������Ӧ
	TM1650_Display(0x48,0x31);//��ʼ��Ϊ5���Ҷȣ�����ʾ	
}
 
 
 
 
 
 