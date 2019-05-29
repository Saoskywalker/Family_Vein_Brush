#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "delay.h"
#include "UserBaseLib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//MPU6050 IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/17
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 	   		   
//IO方向设置
#define MPU_SDA_IN()  {GPIOB->DDR&=~GPIO_PIN_5;GPIOB->CR1&=~GPIO_PIN_5;GPIOB->CR2&=~GPIO_PIN_5;}
#define MPU_SDA_OUT() {GPIOB->DDR|=GPIO_PIN_5;GPIOB->CR1|=GPIO_PIN_5;GPIOB->CR2|=GPIO_PIN_5;}
//#define MPU_SDA_IN()  {GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);}
//#define MPU_SDA_OUT() {GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_SLOW);}

//IO操作函数	 
#define MPU_IIC_SCL    GPIOB->ODR 		//SCL    PB4
#define MPU_IIC_SCL_STATUS	GPIO_PIN_4
#define MPU_IIC_SDA    GPIOB->ODR		//SDA	 PB5
#define MPU_IIC_SDA_STATUS	GPIO_PIN_5
//#define MPU_READ_SDA   GPIO_ReadInputPin(GPIOB, GPIO_PIN_5)  		//输入SDA  PB5
#define MPU_READ_SDA GPIOB_IN->IDR5

//IIC所有操作函数
void MPU_IIC_Delay(void);				//MPU IIC延时函数
void MPU_IIC_Init(void);                //初始化IIC的IO口				 
void MPU_IIC_Start(void);				//发送IIC开始信号
void MPU_IIC_Stop(void);	  			//发送IIC停止信号
void MPU_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 MPU_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 MPU_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void MPU_IIC_Ack(void);					//IIC发送ACK信号
void MPU_IIC_NAck(void);				//IIC不发送ACK信号

void IMPU_IC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif
















