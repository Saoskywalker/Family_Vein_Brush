/************************************************************************
Name: UserBaseLib for STM8S IAR
Editor: Aysi
Date: 2017.11.23
************************************************************************/

#ifndef _User_Base_Lib_H
#define _User_Base_Lib_H

#include "stm8s.h"
#include "TM1650.h"

#define BIT_CLEAR(dv,db) (dv &= (~(1<<db)))
#define BIT_SET(dv,db) (dv |= (1<<db))
#define BIT_GET(dv,db) (dv&(1<<db))
#define BIT0_1 0x01
#define BIT1_1 0x02
#define BIT2_1 0x04
#define BIT3_1 0x08
#define BIT4_1 0x10
#define BIT5_1 0x20
#define BIT6_1 0x40
#define BIT7_1 0x80

typedef struct
{
  unsigned char ODR0        : 1;
  unsigned char ODR1        : 1;
  unsigned char ODR2        : 1;
  unsigned char ODR3        : 1;
  unsigned char ODR4        : 1;
  unsigned char ODR5        : 1;
  unsigned char ODR6        : 1;
  unsigned char ODR7        : 1;
}_GPIO_ODR_BIT;

#define GPIOA_OUT ((_GPIO_ODR_BIT *) GPIOA_BaseAddress)
#define GPIOB_OUT ((_GPIO_ODR_BIT *) GPIOB_BaseAddress)
#define GPIOC_OUT ((_GPIO_ODR_BIT *) GPIOC_BaseAddress)
#define GPIOD_OUT ((_GPIO_ODR_BIT *) GPIOD_BaseAddress)

typedef struct
{
  unsigned char IDR0        : 1;
  unsigned char IDR1        : 1;
  unsigned char IDR2        : 1;
  unsigned char IDR3        : 1;
  unsigned char IDR4        : 1;
  unsigned char IDR5        : 1;
  unsigned char IDR6        : 1;
  unsigned char IDR7        : 1;
}_GPIO_IDR_BIT;

#define GPIOA_IN ((_GPIO_IDR_BIT *) GPIOA_BaseAddress+1)
#define GPIOB_IN ((_GPIO_IDR_BIT *) GPIOB_BaseAddress+1)
#define GPIOC_IN ((_GPIO_IDR_BIT *) GPIOC_BaseAddress+1)
#define GPIOD_IN ((_GPIO_IDR_BIT *) GPIOD_BaseAddress+1)

typedef struct
{
 unsigned char ms1        : 1;
 unsigned char ms2        : 1;
 unsigned char ms3        : 1;
 unsigned char ms4        : 1;
 unsigned char ms5        : 1;
 unsigned char ms6        : 1;
 unsigned char error      : 1;
 unsigned char work       : 1;
}_StateBit;
extern _StateBit FlagState;

extern u8 SendBufLen;
extern u8 *SendBuffer;
extern u8 UART1BusyFlag;

void Uart1Init(u32 baud);
volatile void Delay(uint16_t nCount);
void Timer4Init(void);
void UART1SendBuf(u8 *SendBufAddr, u8 SendLen);
void IWDG_Configuration(void);
void Tim2_Time_Upmode_conf(uint8_t TIM2_Prescaler,uint16_t TIM2_Period);
void Tim1_Time_Upmode_conf(uint16_t TIM1_Prescaler,
                           uint16_t TIM1_Period,
                           uint8_t TIM1_RepetitionCounter);
void TIM1_PWM_Init(uint16_t TIM1_Prescaler, uint16_t TIM1_Period, uint16_t pules);
void AD1Init(void);

//App define
#define HEAT_PIN GPIOD_OUT->ODR4
#define LED_CON GPIOA_OUT->ODR3
#define LED_CON_CLOSE 0
#define LED_CON_OPEN 1
#define REC_SEND_485 GPIOD_OUT->ODR3
#define REC_485 0
#define SEND_485 1

//App value
extern u8 EEPROMLocal;
extern u32 EECountTemp;

//App Function
u32 EEPROMRestartDeal();
void EEPROMStorage(u32 *EECount);

//SMG display
#define DIG1 (u8)0X68
#define DIG2 (u8)0X6A
#define DIG3 (u8)0X6C
#define DIG4 (u8)0X6E

#define SMG_One_Display(ch, i) TM1650_Display(ch, i);

//Key get
#define KEY_TEMP 0X47
#define KEY_START 0X4F
#define KEY_BIO 0X57

#define Key_Get() TM1650_Key();

extern u8 TempIntensity;
void BIO1PWM(u8 i, u8 Work);
void HeatPWM(u8 i, u8 Work);

#endif

