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

void Uart0Init(u32 u32Baudrate);
void Uart1Init(u32 u32Baudrate);
volatile void Delay(uint16_t nCount);
u8 ReceiveData_UART0(void);
void SendData_UART0(u8 c);
u8 ReceiveData_UART1(void);
void SendData_UART1 (u8 c);
void UART0SendBuf(u8 *SendBufAddr, u8 SendLen);
void IWDG_Configuration(void);
#define IWDG_Feed set_WDCLR
void Tim2_Time_Upmode_conf(uint16_t TIM2_Period);
void PWM_Init(uint16_t Period, uint16_t pules);
void AD1Init(void);
u16 AD1Sample(u8 channel);

extern u8 BeeMod, BeeTime;
void BeeFunction(void);

//App define
#define OLENOOL_PIN P04
#define SOLENOLDP_PIN P01
#define SOLENOLDS_PIN P03
#define LAMP1_PIN P06
#define LAMP2_PIN P12
#define MOTOR1_PIN P11
#define PUMPL_PIN P10
#define PUMPR_PIN P00
#define CHANNEL1_PIN P07
#define SOUND_PIN P15

#define REC_SEND_485 GPIOD_OUT->ODR3
#define REC_485 0
#define SEND_485 1

//App value
typedef struct
{
 unsigned char ms1        : 1;
 unsigned char ms2        : 1;
 unsigned char u100       : 1;
 unsigned char heat       : 1;
 unsigned char shock      : 1;
 unsigned char s1        : 1;
 unsigned char error      : 1;
 unsigned char work       : 1;
}_StateBit;
extern _StateBit FlagState;

extern u8 SendBufLen;
extern u8 *SendBuffer;
extern u8 UART1BusyFlag;

#define INLINE_MUSIC_CANNEL()	{if(FlagState.work) BeeTime = 0;}
#define INLINE_MUSIC_BUTTON() {if(FlagState.work) {BeeMod = 0; BeeTime = 1;}}
#define INLINE_MUSIC_START() {if(FlagState.work) {BeeMod = 1; BeeTime = 1;}}
#define INLINE_MUSIC_STOP() {if(FlagState.work) {BeeMod = 2; BeeTime = 5;}}
#define INLINE_MUSIC_ERROR() {if(FlagState.work) {BeeMod = 0; BeeTime = 7;}}

// extern u8 EEPROMLocal;
// extern u32 EECountTemp;

//App Function
// u32 EEPROMRestartDeal();
// void EEPROMStorage(u32 *EECount);

//SMG display
#define DIG1 (u8)0X68
#define DIG2 (u8)0X6A
#define DIG3 (u8)0X6C
#define DIG4 (u8)0X6E

#define SMG_One_Display(ch, i) TM1650_Display(ch, i);

//Key get
#define KEY_TEMP_DOWN 0X47
#define KEY_TEMP_UP 0X4F
#define KEY_POWER 0X67
#define KEY_HEAT 0X57
#define KEY_SYRENGTH 0X5F
#define KEY_VIBRATON 0X6F

#define Key_Get() TM1650_Key();

extern u8 TempIntensity, BIOIntensity;
extern u8 TempWork1, TempWork2, TempStage1, TempStage2;
void HeatPWM(u8 i, u8 Work);
void HeatPWM2(u8 i, u8 Work);

#endif
