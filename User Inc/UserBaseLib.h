/************************************************************************
Name: UserBaseLib for N76E003 KEIL
Editor: Aysi
Date: 2019.10.15
************************************************************************/

#ifndef _User_Base_Lib_H
#define _User_Base_Lib_H

#include "N76E003_BSP.h"

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
 unsigned char ms1        : 1;
 unsigned char ms2        : 1;
 unsigned char u100       : 1;
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

void Uart0Init(u32 u32Baudrate);
volatile void Delay(uint16_t nCount);
void UART0SendBuf(u8 *SendBufAddr, u8 SendLen);
void IWDG_Configuration(void);
void Tim2_Time_Upmode_conf(uint8_t TIM2_Prescaler,uint16_t TIM2_Period);
void Tim1_Time_Upmode_conf(uint16_t TIM1_Prescaler,
                           uint16_t TIM1_Period,
                           uint8_t TIM1_RepetitionCounter);
void TIM1_PWM_Init(uint16_t TIM1_Prescaler, uint16_t TIM1_Period, uint16_t pules);
void AD1Init(void);

//App define
#define HEAT_PIN GPIOA_OUT->ODR3
#define LED_CON GPIOB_OUT->ODR5
#define CHANNEL1_PIN GPIOD_OUT->ODR4
#define CHANNEL2_PIN GPIOD_OUT->ODR5
#define CHANNEL3_PIN GPIOD_OUT->ODR6
#define BIOS_PIN GPIOA_OUT->ODR1
#define BIOA_PIN GPIOA_OUT->ODR2

#define A_OUT_PIN GPIOB_OUT->ODR4
#define B_OUT_PIN GPIOC_OUT->ODR3
#define C_OUT_PIN GPIOC_OUT->ODR4
#define D_OUT_PIN GPIOC_OUT->ODR5
#define E_OUT_PIN GPIOC_OUT->ODR6
#define F_OUT_PIN GPIOC_OUT->ODR7
#define G_OUT_PIN GPIOD_OUT->ODR1
#define H_OUT_PIN GPIOD_OUT->ODR2

#define A_IN_PIN GPIOB_IN->IDR4
#define B_IN_PIN GPIOC_IN->IDR3
#define C_IN_PIN GPIOC_IN->IDR4
#define D_IN_PIN GPIOC_IN->IDR5
#define E_IN_PIN GPIOC_IN->IDR6
#define F_IN_PIN GPIOC_IN->IDR7
#define G_IN_PIN GPIOD_IN->IDR1
#define H_IN_PIN GPIOD_IN->IDR2

#define LED_CLOSE 1
#define LED_OPEN 0

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

#define SMG_One_Display(ch, i) MCU_DK_Display(ch, i);

//Key get
#define KEY_TEMP 0X47
#define KEY_START 0X4F
#define KEY_BIO 0X57

#define Key_Get() MCU_DK_Key();

extern u8 TempIntensity, BIOIntensity;
void BIO1PWM(u8 i, u8 Work);
void BIO1Power(u8 i, u8 Work);
void HeatPWM(u8 i, u8 Work);
void MCU_DK_Display(u8 ch,u8 dat);
u8 MCU_DK_Key(void);
void MCU_DK_SetLight(u8 i);
void MCU_DK_DisAndKey_Handle(void);

#endif

