/*****************************************************************************
Name: Heat&BIO
version: v1.0 Released
Editor: Aysi
Describe: 
History: 
2019.5.28: Created
2019.6.1; OK
2019.10.14: cannel tm1650, use mcu replace
2019.10.15: implant from STM8S003 to N76E003
2019.10.19: add battery function
*****************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
// #define DEBUG
#include "UserBaseLib.h"
#include "delay.h"
#include "TM1650.h"
//#include <stdio.h>
// #include "UART_Frame.h"

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* //BIO1 PWM
u8 BIOIntensity = 0;
const u16 BIO1IntensityTable[] = {0, 36, 38, 40, 42, 44,
																	46, 48, 50};
const u16 BIO1ModPeriod[] = {11, 330, 400};
const u16 BIO1ModCompare[] = {3, 2, 240};	
void BIO1PWM(u8 i, u8 Work)
{
	static u16 BIO1TimeCnt = 0;
	static u8 BIO1ModRenew = 0;
	
	if(Work)
	{
		if(++BIO1TimeCnt>=BIO1ModPeriod[BIO1ModRenew])
		{
			BIO1TimeCnt = 0;
			BIO1ModRenew = i;
		}
		if((BIO1TimeCnt<BIO1ModCompare[BIO1ModRenew]))
			BIOS_PIN = 0; //open
		else
			BIOS_PIN = 1; //close
	}
	else
	{
		BIO1TimeCnt = 0;
		BIOS_PIN = 1; //close
	}
}

void BIO1Power(u8 i, u8 Work)
{
  static u16 BIO1TimeCnt = 0;
	static u8 BIO1ModRenew = 0;
	
	if(Work)
	{
		if(++BIO1TimeCnt>=75)
		{
			BIO1TimeCnt = 0;
			BIO1ModRenew = i;
		}
		if((BIO1TimeCnt<BIO1IntensityTable[BIO1ModRenew]))
			BIOA_PIN = 1;
		else
			BIOA_PIN = 0;
	}
	else
	{
		BIO1TimeCnt = 0;
		BIOA_PIN = 0;
	}
} */

/*Bee function*/
const u16 BeeModPeriod[] = {110, 410, 1000, 1010, 3010};
const u16 BeeModCompare[] = {100, 400, 500, 1000, 3000};
u8 BeeMod = 0, BeeTime = 0;
void BeeFunction(void)
{
	static u16 BeeTimeCnt = 0;

	if (BeeTime > 0)
	{
		if (++BeeTimeCnt >= BeeModPeriod[BeeMod])
		{
			BeeTimeCnt = 0;
			BeeTime--;
		}
		else
		{
			if (BeeTimeCnt >= BeeModCompare[BeeMod])
				SOUND_PIN = 0;
			else
				SOUND_PIN = 1;
		}
	}
	else
	{
		BeeTimeCnt = 0;
		SOUND_PIN = 0;
	}
}

u16 WorkTime = 0;
void WorkTimeDeal(void)
{
	if (FlagState.s1)
	{
			FlagState.s1 = 0;
			if (WorkTime > 0)
			{
				WorkTime--;
			}
			else
			{
				INLINE_MUSIC_STOP();
				WorkTime = 900;
				FlagState.work = 0;
			}
	}
}

//Heat
u8 TempIntensity = 1;
const u16 HeatIntensityTable[] = {0, 1, 2, 3, 4,
																	5, 6, 7, 8};
/* void HeatPWM(u8 i, u8 Work)
{
	static u16 HeatTimeCnt = 0;
	static u8 HeatModRenew = 0;
	
	if(Work)
	{
		if(++HeatTimeCnt>=8)
		{
			HeatTimeCnt = 0;
			HeatModRenew = i;
		}
		if((HeatTimeCnt<HeatIntensityTable[HeatModRenew]))
			HEAT_PIN = 1;	//ON
		else
			HEAT_PIN = 0;	//OFF
	}
	else
	{
		HeatTimeCnt = 0;
		HEAT_PIN = 0;	//ON
	}
} */

//Temperature Process
const u16 TemperatureTable[] = {0, 1505, 1490, 1480, 1470, 1460,
                                1450, 1440, 1430, 1420, 1410,
                                1400, 1390, 1380, 1370, 1360,
                                1350, 1340, 1330, 1320, 1310, 
                                1296, 1196, 1096, 996, 796, 
                                596};
u16 Temperature1 = 0;
static u8 Ntc1ErrorFlag = 0;
void TemperatureProcess1(void)
{
	static u16 i = 0;
	
	//NTC ERROR
	if(Temperature1>=4000||Temperature1<=100)
	{
		if(++i>=800)
		{
			i = 800;
			if(Ntc1ErrorFlag==0)
			{
				Ntc1ErrorFlag = 1;
        INLINE_MUSIC_ERROR();
				LAMP2_PIN = 0;
			}			
		}
	}
	else
	{
		if (i>=25)
			i -= 25;
		else
			i = 0;
		if(i==0)
			Ntc1ErrorFlag = 0;

		if(Temperature1>=TemperatureTable[TempIntensity]+16)
			LAMP2_PIN = 1;
		if(Temperature1<=TemperatureTable[TempIntensity])
			LAMP2_PIN = 0;
	}
}

u16 Temperature2 = 0;
static u8 Ntc2ErrorFlag = 0;
void TemperatureProcess2(void)
{
	static u16 i = 0;
	
	//NTC ERROR
	if(Temperature2>=4000||Temperature2<=100)
	{
		if(++i>=800)
		{
			i = 800;
			if(Ntc2ErrorFlag==0)
			{
				Ntc2ErrorFlag = 1;
        INLINE_MUSIC_ERROR();
				LAMP1_PIN = 0;
			}			
		}
	}
	else
	{
		if (i>=25)
			i -= 25;
		else
			i = 0;
		if(i==0)
			Ntc2ErrorFlag = 0;

		if(Temperature2>=TemperatureTable[TempIntensity]+16)
			LAMP1_PIN = 1;
		if(Temperature2<=TemperatureTable[TempIntensity])
			LAMP1_PIN = 0;
	}
}

u16 Pressure = 0;
static u8 PressureErrorFlag = 0;
void PressureProcess(void)
{
  static u16 cnt = 0;
  static u16 i = 0;

  if (Pressure >= 4000 || Pressure <= 100)
  {
    if (++i >= 800)
    {
      i = 800;
      if (PressureErrorFlag == 0)
      {
        PressureErrorFlag = 1;
        INLINE_MUSIC_ERROR();
        PUMPL_PIN = 0;
        PUMPR_PIN = 0;
        SOLENOLDS_PIN = 0;
        SOLENOLDP_PIN = 0;
      }
    }
  }
  else
  {
    if (i >= 25)
      i -= 25;
    else
      i = 0;
    if (i == 0)
      PressureErrorFlag = 0;

    if (Pressure >= 1813)
    {
      PUMPL_PIN = 0;
      PUMPR_PIN = 0;
      SOLENOLDS_PIN = 0;
      SOLENOLDP_PIN = 0;
      cnt = 0;
    }
    else if (Pressure <= 592)
    {
      if (++cnt >= 300)
      {
        cnt = 300;
        PUMPL_PIN = 1;
        PUMPR_PIN = 1;
        SOLENOLDS_PIN = 1;
        SOLENOLDP_PIN = 1;
      }
    }
  }
}

void main(void)
{ 
  u8 DIG3_Dis_Temp = 0;
  u8 DIG2_Dis; 
  const u8 DIG_Dis[] = {0X7D, 0X18, 0XB5, 0XB9, 0XD8, 0XE9, 0XED, 0X38, 0XFD, 0XF9, 0X80, 0X02}; 
  u8 TaskNumber = 1, KeyValue = 0;
  u8 KeyUp = 0, KeyT = 0;
  u8 mode = 0;
  
  //io init 
  //note: after power up, N76E003 IO is only input(float) mode, P0,P1,P3 is 1 P2 only input mode
  clr_P2S_0; //POWER KEY, HIZ mode

  P00_PushPull_Mode; LAMP2_PIN = 0; 
  P01_PushPull_Mode; MOTOR1_PIN = 0; 
  P03_PushPull_Mode; PUMPL_PIN = 0; 
  P04_PushPull_Mode; PUMPR_PIN = 0; 
  P10_PushPull_Mode; LAMP1_PIN = 0;
  P11_PushPull_Mode; SOLENOLDS_PIN = 0;
  P12_PushPull_Mode; SOLENOLDP_PIN = 0;
  P15_PushPull_Mode; SOUND_PIN = 0;
  // P06_PushPull_Mode; OLENOOL_PIN = 0; 
  // P07_PushPull_Mode; CHANNEL1_PIN = 0;

  #ifndef DEBUG
  IWDG_Configuration(); //Open IWDG
  #endif

  Uart0Init(9600);
  Tim2_Time_Upmode_conf(TIMER_DIV4_VALUE_100us);  //100us      
  set_EA;//Open main interrupt

  // delay_init();
  // PWM_Init(0x7CF, 0);
  AD1Init();
  TM1650_Init(0, 0);

  while (1)
  {
#ifndef DEBUG
    IWDG_Feed;  //Clear IWDG cnt
#endif

    if (FlagState.ms2)
    {
      FlagState.ms2 = 0;
      switch (TaskNumber)
      {
        case 1: //UART T&R process
        {
          TaskNumber++;
          break;
        }
        case 2: //KEY GET AND DISPLAY
        {
          if (FlagState.work) //WORK
            BIT_SET(DIG2_Dis, 0);
          else
            DIG2_Dis = 0;
          if (FlagState.heat) //HEAT
            BIT_SET(DIG2_Dis, 3);
          else
            BIT_CLEAR(DIG2_Dis, 3);
          if (FlagState.shock) //SHOCK
            BIT_SET(DIG2_Dis, 4);
          else
            BIT_CLEAR(DIG2_Dis, 4);
          BIT_CLEAR(DIG2_Dis, 5); //MODE
          BIT_CLEAR(DIG2_Dis, 2);
          BIT_CLEAR(DIG2_Dis, 1);
          if (mode == 3)
            BIT_SET(DIG2_Dis, 5);
          else if (mode == 2)
            BIT_SET(DIG2_Dis, 2);
          else if (mode == 1)
            BIT_SET(DIG2_Dis, 1);

          SMG_One_Display(DIG3, DIG2_Dis);
          SMG_One_Display(DIG1, DIG_Dis[(TempIntensity+29)%10]);
          SMG_One_Display(DIG2, DIG_Dis[(TempIntensity+29)/10]);
          KeyValue = Key_Get();
          TaskNumber++;
          break;
        }
        case 3: //key process
        {
          if (KeyValue != KEY_TEMP_DOWN && KeyValue != KEY_TEMP_UP && 
              KeyValue != KEY_POWER && KeyValue != KEY_HEAT && 
              KeyValue != KEY_SYRENGTH && KeyValue != KEY_VIBRATON)
          {
            KeyUp = 1;
          }
          if ((KeyValue == KEY_TEMP_DOWN) && KeyUp)
          {
            KeyUp = 0;
            INLINE_MUSIC_BUTTON();
            if (TempIntensity > 1)
            {
              TempIntensity--;
            }
          }
          else if ((KeyValue == KEY_TEMP_UP) && KeyUp)
          {
            KeyUp = 0;
            INLINE_MUSIC_BUTTON();
            if (TempIntensity < 26)
            {
              TempIntensity++;
            }
          }
          else if ((KeyValue == KEY_POWER) && KeyUp)
          {
            KeyUp = 0;
            INLINE_MUSIC_BUTTON();
            if (FlagState.work)
            {
              FlagState.work = 0;
              TM1650_Light(0);
            }
            else
            {
              FlagState.work = 1;
              TM1650_Light(8);
            }
          }
          else if ((KeyValue == KEY_HEAT) && KeyUp)
          {
            KeyUp = 0;
            INLINE_MUSIC_BUTTON();
            if (FlagState.heat)
            {
              FlagState.heat = 0;
            }
            else
            {
              FlagState.heat = 1;
            }
          }
          else if ((KeyValue == KEY_VIBRATON) && KeyUp)
          {
            KeyUp = 0;
            INLINE_MUSIC_BUTTON();
            if (mode)
            {
              if (FlagState.shock)
              {
                FlagState.shock = 0;
              }
              else
              {
                FlagState.shock = 1;
              }
            }
          }
          else if ((KeyValue == KEY_SYRENGTH) && KeyUp)
          {
            KeyUp = 0;
            INLINE_MUSIC_BUTTON();
            if (++mode > 3)
            {
              mode = 0;
              FlagState.shock = 0;
            }
          }
          TaskNumber++;
          break;
        }
        case 4: //work process
        { 
          if(FlagState.work)
          {
            if (mode)
            {
              if (FlagState.shock)
                MOTOR1_PIN = 1;
              else
                MOTOR1_PIN = 0;
              PressureProcess();
              // WorkTimeDeal();
            }
            else
            {
              PUMPL_PIN = 0;
              PUMPR_PIN = 0;
              SOLENOLDS_PIN = 0;
              SOLENOLDP_PIN = 0;
              MOTOR1_PIN = 0;
            }
            if(FlagState.heat)
            {
              TemperatureProcess1();
              TemperatureProcess2();
            }
          }
          else
          {
            mode = 0;
            FlagState.shock = 0;
            FlagState.heat = 0;
            LAMP2_PIN = 0;
            MOTOR1_PIN = 0;
            PUMPL_PIN = 0;
            PUMPR_PIN = 0;
            LAMP1_PIN = 0;
            SOLENOLDS_PIN = 0;
            SOLENOLDP_PIN = 0;
          }     
          TaskNumber++;
          break;
        }
        case 5: //AD sample
        {
          if(FlagState.work)
          {
            if (FlagState.s1)
            {
              FlagState.s1 = 0;
            Pressure = AD1Sample(0);
            Temperature1 = AD1Sample(1);
            Temperature2 = AD1Sample(4);
            SendData_UART0(Pressure>>8);
            SendData_UART0((u8)Pressure);
            SendData_UART0(Temperature1>>8);
            SendData_UART0((u8)Temperature1);
            SendData_UART0(Temperature2>>8);
            SendData_UART0((u8)Temperature2);
            }
          }
          TaskNumber = 1;
          break;
        }
        default:
        {
          TaskNumber = 5;
          FlagState.error = 1;
          break;
        }
      }
    }
  } 
}
