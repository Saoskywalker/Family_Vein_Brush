#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "delay.h"
#include "UserBaseLib.h"
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
 	   		   
//IO��������
#define MPU_SDA_IN()  {GPIOB->DDR&=~GPIO_PIN_5;GPIOB->CR1&=~GPIO_PIN_5;GPIOB->CR2&=~GPIO_PIN_5;}
#define MPU_SDA_OUT() {GPIOB->DDR|=GPIO_PIN_5;GPIOB->CR1|=GPIO_PIN_5;GPIOB->CR2|=GPIO_PIN_5;}
//#define MPU_SDA_IN()  {GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);}
//#define MPU_SDA_OUT() {GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW);}

//IO��������	 
#define MPU_IIC_SCL    GPIOB->ODR 		//SCL    PB4
#define MPU_IIC_SCL_STATUS	GPIO_PIN_4
#define MPU_IIC_SDA    GPIOB->ODR		//SDA	 PB5
#define MPU_IIC_SDA_STATUS	GPIO_PIN_5
//#define MPU_READ_SDA   GPIO_ReadInputPin(GPIOB, GPIO_PIN_5)  		//����SDA  PB5
#define MPU_READ_SDA GPIOB_IN->IDR5

//IIC���в�������
void MPU_IIC_Delay(void);				//MPU IIC��ʱ����
void MPU_IIC_Init(void);                //��ʼ��IIC��IO��				 
void MPU_IIC_Start(void);				//����IIC��ʼ�ź�
void MPU_IIC_Stop(void);	  			//����IICֹͣ�ź�
void MPU_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 MPU_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void MPU_IIC_Ack(void);					//IIC����ACK�ź�
void MPU_IIC_NAck(void);				//IIC������ACK�ź�

void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















