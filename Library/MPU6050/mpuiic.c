#include "mpuiic.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//MPU6050 IIC���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/17
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
 //MPU IIC ��ʱ����
void MPU_IIC_Delay(void)
{
	delay_us(15);
}

//��ʼ��IIC
void MPU_IIC_Init(void)
{					     
	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW);
}
//����IIC��ʼ�ź�
void MPU_IIC_Start(void)
{
	MPU_SDA_OUT();     //sda�����
	MPU_IIC_SDA |= MPU_IIC_SDA_STATUS;	  	  
	MPU_IIC_SCL |= MPU_IIC_SCL_STATUS;
	MPU_IIC_Delay();
 	MPU_IIC_SDA &= ~MPU_IIC_SDA_STATUS;//START:when CLK is high,DATA change form high to low 
	MPU_IIC_Delay();
	MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void MPU_IIC_Stop(void)
{
	MPU_SDA_OUT();//sda�����
	MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;
	MPU_IIC_SDA &= ~MPU_IIC_SDA_STATUS;//STOP:when CLK is high DATA change form low to high
 	MPU_IIC_Delay();
	MPU_IIC_SCL |= MPU_IIC_SCL_STATUS; 
	MPU_IIC_SDA |= MPU_IIC_SDA_STATUS;//����I2C���߽����ź�
	MPU_IIC_Delay();							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 MPU_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MPU_SDA_IN();      //SDA����Ϊ����  
	MPU_IIC_SDA |= MPU_IIC_SDA_STATUS;
	MPU_IIC_Delay();	   
	MPU_IIC_SCL |= MPU_IIC_SCL_STATUS;
	MPU_IIC_Delay();	 
	while(MPU_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			MPU_IIC_Stop();
			return 1;
		}
	}
	MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void MPU_IIC_Ack(void)
{
	MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;
	MPU_SDA_OUT();
	MPU_IIC_SDA &= ~MPU_IIC_SDA_STATUS;
	MPU_IIC_Delay();
	MPU_IIC_SCL |= MPU_IIC_SCL_STATUS;
	MPU_IIC_Delay();
	MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;
}
//������ACKӦ��		    
void MPU_IIC_NAck(void)
{
	MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;
	MPU_SDA_OUT();
	MPU_IIC_SDA |= MPU_IIC_SDA_STATUS;
	MPU_IIC_Delay();
	MPU_IIC_SCL |= MPU_IIC_SCL_STATUS;
	MPU_IIC_Delay();
	MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void MPU_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	MPU_SDA_OUT(); 	    
    MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
       if ((txd&0x80)>>7)
       {
		   MPU_IIC_SDA |= MPU_IIC_SDA_STATUS;
       }		
	   else
	   {
		   MPU_IIC_SDA &= ~MPU_IIC_SDA_STATUS;
	   }
        txd<<=1; 	  
		    MPU_IIC_SCL |= MPU_IIC_SCL_STATUS;
		    MPU_IIC_Delay(); 
		    MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;	
		    MPU_IIC_Delay();
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 MPU_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MPU_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        MPU_IIC_SCL &= ~MPU_IIC_SCL_STATUS;
        MPU_IIC_Delay();
		MPU_IIC_SCL |= MPU_IIC_SCL_STATUS;
        receive<<=1;
        if(MPU_READ_SDA)receive++;   
		MPU_IIC_Delay(); 
    }					 
    if (!ack)
        MPU_IIC_NAck();//����nACK
    else
        MPU_IIC_Ack(); //����ACK   
    return receive;
}


























