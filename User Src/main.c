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
#include "UserBaseLib.h"
#include "delay.h"
#include "TM1650.h"
//#include <stdio.h>
// #include "UART_Frame.h"

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

//0 1 2 3 4 5 6 7 8 9 - E . NC
const u8 DIG_Dis[] = {0X7D, 0X18, 0XB5, 0XB9, 0XD8, 0XE9, 0XED, 0X38, 0XFD, 0XF9, 0X80, 0XE5, 0X02, 0X00}; 
u8 DIG2_Dis = 0; 

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

u16 WorkTime = 959;
u8 disShift = 0;
void WorkTimeDeal(void)
{
  static u16 WorkTimeDis = 0;
  static u8 secondFlag = 13, cnt1min = 0;

  if (FlagState.s1)
  {
    FlagState.s1 = 0;
    if (WorkTime > 0)
    {
      WorkTime--;
      if(++cnt1min>=60)
      {
        cnt1min = 0;
        INLINE_MUSIC_BUTTON();
      }
    }
    else
    {
      INLINE_MUSIC_STOP();
      WorkTime = 959;
      cnt1min = 0;
      FlagState.work = 0;
      DIG2_Dis = 0;
      SMG_One_Display(DIG3, DIG2_Dis);
      TM1650_Light(0);
    }
    if (++disShift >= 2)
    {
      disShift = 2;
      if (secondFlag == 13)
        secondFlag = 12;
      else
        secondFlag = 13;
      // WorkTimeDis = ((WorkTime + 59) / 60 / 10 << 8) + ((WorkTime + 59) / 60 % 10);
      WorkTimeDis = ((WorkTime) / 60 / 10 << 8) + ((WorkTime) / 60 % 10);
      SMG_One_Display(DIG1, DIG_Dis[WorkTimeDis & 0X00FF] | DIG_Dis[secondFlag]);
      SMG_One_Display(DIG2, DIG_Dis[WorkTimeDis >> 8]);
    }
  }
}

//Heat
u8 TempWork1 = 0, TempWork2 = 0, TempStage1 = 0, TempStage2 = 0;
u8 TempIntensity = 1;
const u16 HeatIntensityTable[] = {0, 3, 5, 8, 4,
																	5, 6, 7, 8};
void HeatPWM(u8 i, u8 Work)
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
			LAMP1_PIN = 1;	//ON
		else
			LAMP1_PIN = 0;	//OFF
	}
	else
	{
		HeatTimeCnt = 0;
		LAMP1_PIN = 0;	//OFF
	}
}

void HeatPWM2(u8 i, u8 Work)
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
			LAMP2_PIN = 1;	//ON
		else
			LAMP2_PIN = 0;	//OFF
	}
	else
	{
		HeatTimeCnt = 0;
		LAMP2_PIN = 0;	//OFF
	}
}

//Temperature Process
const u16 TemperatureTable[] = {0, 1664, 1629, 1594, 1559, 1524,
                                1488, 1448, 1408, 1368, 1328,
                                1290, 1261, 1232, 1203, 1174,
                                1144, 1117, 1090, 1063, 1036, 
                                1008, 980, 952, 924, 896, 
                                868};
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
				TempWork1 = 0;
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
TempStage1 = 3;
		if(Temperature1>=TemperatureTable[TempIntensity]+25)
			TempWork1 = 1;
		if(Temperature1<=TemperatureTable[TempIntensity])
			TempWork1 = 0;
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
				TempWork2 = 0;
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
TempStage2 = 3;
		if(Temperature2>=TemperatureTable[TempIntensity]+25)
			TempWork2 = 1;
		if(Temperature2<=TemperatureTable[TempIntensity])
			TempWork2 = 0;
	}
}

u16 Pressure = 0;
static u8 PressureErrorFlag = 0;
static const u16 PressureTable[4][2] = {{0, 0}, {4, 2}, {8, 4}, {12, 12}};
void PressureProcess(u8 mode)
{
  static u16 cnt = 0;
  static u16 i = 0;
  static u8 step = 1;

  if (mode)
  {
    if (Pressure >= 4000 || Pressure <= 100)
    {
      if (++i >= 400)
      {
        i = 400;
        if (PressureErrorFlag == 0)
        {
          PressureErrorFlag = 1;
          INLINE_MUSIC_ERROR();
          PUMPL_PIN = 0;
          PUMPR_PIN = 0;
          SOLENOLDS_PIN = 0;
          SOLENOLDP_PIN = 0;
          OLENOOL_PIN = 0;
          cnt = 0;
          step = 1;
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
    }

    if (PressureErrorFlag == 0)
    {
      switch (step)
      {
      case 1:
      {
        OLENOOL_PIN = 1;
        SOLENOLDS_PIN = 1;
        SOLENOLDP_PIN = 1;
        if (Pressure >= 1925 || cnt >= 5)
        {
          if(++cnt>=PressureTable[mode][0]+5)
          {
            PUMPL_PIN = 0;
            PUMPR_PIN = 0;
            if (cnt >= PressureTable[mode][0]+5+10)
            {
              cnt = 0;
              step++;
            }
          }
        }
        else
        {
          PUMPL_PIN = 1;
          PUMPR_PIN = 1;
        }
        break;
      }
      case 2:
      {
        OLENOOL_PIN = 0;
        SOLENOLDS_PIN = 1;
        SOLENOLDP_PIN = 1;
        if (Pressure >= 1925 || cnt >= 5)
        {
          if(++cnt>=PressureTable[mode][1]+5)
          {
            PUMPL_PIN = 0;
            PUMPR_PIN = 0;
            if (cnt >= PressureTable[mode][1]+5+28)
            {
              cnt = 0;
              step++;
            }
          }
        }
        else
        {
          PUMPL_PIN = 1;
          PUMPR_PIN = 1;
        }
        break;
      }
      case 3:
      {
        SOLENOLDS_PIN = 0;
        SOLENOLDP_PIN = 1;
        if (++cnt >= 12)
        {
          cnt = 0;
          step++;
        }
        break;
      }
      case 4:
      {
        SOLENOLDS_PIN = 1;
        SOLENOLDP_PIN = 0;
        if (++cnt >= 12)
        {
          cnt = 0;
          step = 1;
        }
        break;
      }
      default:
      {
        step = 1;
        break;
      }
      }
    }
  }
  else
  {
    PUMPL_PIN = 0;
    PUMPR_PIN = 0;
    SOLENOLDS_PIN = 0;
    SOLENOLDP_PIN = 0;
    OLENOOL_PIN = 0;
    cnt = 0;
    step = 1;
  }
}

void shockRun()
{
  static u16 cnt = 0;
  static u8 step = 1;
  if (FlagState.shock)
  {
    switch (step)
    {
    case 1:
    {
      if (++cnt >= 700)
      {
        cnt = 0;
        step++;
      }
      else
        MOTOR1_PIN = 1;
      break;
    }
    case 2:
    {

      if (++cnt >= 200)
      {
        cnt = 0;
        step++;
      }
      else
        MOTOR1_PIN = 0;
      break;
    }
    case 3:
    {

      if (++cnt >= 200)
      {
        cnt = 0;
        step++;
      }
      else
        MOTOR1_PIN = 1;
      break;
    }
    case 4:
    {

      if (++cnt >= 200)
      {
        cnt = 0;
        step++;
      }
      else
        MOTOR1_PIN = 0;
      break;
    }
    case 5:
    {

      if (++cnt >= 200)
      {
        cnt = 0;
        step++;
      }
      else
        MOTOR1_PIN = 1;
      break;
    }
    case 6:
    {

      if (++cnt >= 200)
      {
        cnt = 0;
        step = 1;
      }
      else
        MOTOR1_PIN = 0;
      break;
    }
    default:
    {
      cnt = 0;
      step = 1;
      break;
    }
    }
  }
  else
  {
    step = 1;
    cnt = 0;
    MOTOR1_PIN = 0;
  }
}

#define DisTemp() {SMG_One_Display(DIG1, DIG_Dis[(TempIntensity + 29) % 10]); \
                  SMG_One_Display(DIG2, DIG_Dis[(TempIntensity + 29) / 10]); \
                  disShift = 0;}

void main(void)
{ 
  u8 DIG3_Dis_Temp = 0;
  u8 TaskNumber = 1, KeyValue = 0;
  u8 KeyUp = 0, KeyT = 0;
  u8 mode = 0;
  u8 cnt1s = 0;
  u16 KeyCnt = 0;

  //io init 
  //note: after power up, N76E003 IO is only input(float) mode, P0,P1,P3 is 1 P2 only input mode
  clr_P2S_0; //POWER KEY, HIZ mode

  P00_PushPull_Mode; PUMPR_PIN = 0; 
  P01_PushPull_Mode; SOLENOLDP_PIN = 0; 
  P03_PushPull_Mode; SOLENOLDS_PIN = 0; 
  P04_PushPull_Mode; OLENOOL_PIN = 0; 
  P10_PushPull_Mode; PUMPL_PIN = 0;
  P11_PushPull_Mode; MOTOR1_PIN = 0;
  P12_PushPull_Mode; LAMP2_PIN = 0;
  P15_PushPull_Mode; SOUND_PIN = 0;
  P06_PushPull_Mode; LAMP1_PIN = 0; 
  P07_PushPull_Mode; CHANNEL1_PIN = 0;

  #ifndef DEBUG
  IWDG_Configuration(); //Open IWDG
  #endif

  Uart1Init(115200);
  Tim2_Time_Upmode_conf(TIMER_DIV4_VALUE_100us);  //100us      
  set_EA;//Open main interrupt

  // delay_init();
  // PWM_Init(0x7CF, 0);
  AD1Init();
  TM1650_Init(0, 0);

  FlagState.work = 0;

  while (1)
  {
#ifndef DEBUG
    IWDG_Feed; //Clear IWDG cnt
#endif

    if (FlagState.ms2)
    {
      FlagState.ms2 = 0;
      switch (TaskNumber)
      {
        case 1: //KEY GET AND DISPLAY
        {
          KeyValue = Key_Get();
          TaskNumber++;
          break;
        }
        case 2: //UART T&R process
        {
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
          if ((KeyValue == KEY_POWER) && KeyUp)
          {
            KeyUp = 0;
            
            if (FlagState.work)
            {
              INLINE_MUSIC_BUTTON();
              FlagState.work = 0;
              DIG2_Dis = 0;
              TM1650_Light(0);
            }
            else
            {
              FlagState.work = 1;
              INLINE_MUSIC_BUTTON();
              BIT_SET(DIG2_Dis, 0);
              DisTemp();
              TM1650_Light(8);
            }
            SMG_One_Display(DIG3, DIG2_Dis);
          }
          if (FlagState.work)
          {
            if (KeyValue == KEY_TEMP_DOWN)
            {
              if(++KeyCnt>=100)
              {
                KeyCnt = 70;
                if (TempIntensity > 1)
                  TempIntensity--;
                DisTemp();
              }
            }
            else if (KeyValue == KEY_TEMP_UP)
            {
              if (++KeyCnt >= 100)
              {
                KeyCnt = 70;
                if (TempIntensity < 26)
                  TempIntensity++;
                DisTemp();
              }
            }
            else
            {
              KeyCnt = 0;
            }
            if ((KeyValue == KEY_TEMP_DOWN) && KeyUp)
            {
              KeyUp = 0;
              INLINE_MUSIC_BUTTON();
              if (TempIntensity > 1)
              {
                TempIntensity--;
              }
              DisTemp();
            }
            else if ((KeyValue == KEY_TEMP_UP) && KeyUp)
            {
              KeyUp = 0;
              INLINE_MUSIC_BUTTON();
              if (TempIntensity < 26)
              {
                TempIntensity++;
              }
              DisTemp();
            }
            else if ((KeyValue == KEY_HEAT) && KeyUp)
            {
              KeyUp = 0;
              INLINE_MUSIC_BUTTON();  
              if (FlagState.heat)
              {
                FlagState.heat = 0;
                BIT_CLEAR(DIG2_Dis, 3);
              }
              else
              {
                FlagState.heat = 1;
                BIT_SET(DIG2_Dis, 3);
              }
              SMG_One_Display(DIG3, DIG2_Dis);
            }
            else if ((KeyValue == KEY_VIBRATON) && KeyUp)
            {
              KeyUp = 0;
              if (mode)
              {
                if (FlagState.shock)
                {
                  FlagState.shock = 0;
                  BIT_CLEAR(DIG2_Dis, 4);
                }
                else
                {
                  FlagState.shock = 1;
                  BIT_SET(DIG2_Dis, 4);
                }
                INLINE_MUSIC_BUTTON();
                SMG_One_Display(DIG3, DIG2_Dis);
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
                BIT_CLEAR(DIG2_Dis, 4);
                DisTemp();
              }
              BIT_CLEAR(DIG2_Dis, 5); //MODE
              BIT_CLEAR(DIG2_Dis, 2);
              BIT_CLEAR(DIG2_Dis, 1);
              if (mode == 3)
                BIT_SET(DIG2_Dis, 1);
              else if (mode == 2)
                BIT_SET(DIG2_Dis, 2);
              else if (mode == 1)
                BIT_SET(DIG2_Dis, 5);
              SMG_One_Display(DIG3, DIG2_Dis);
            }
          }
          TaskNumber++;
          break;
        }
        case 4: //AD sample
        {
          if (++cnt1s >= 50)
          {
            cnt1s = 0;
            if (FlagState.work)
            {
              Pressure = AD1Sample(0);
              Temperature2 = AD1Sample(1);
              Temperature1 = AD1Sample(4);
              SendData_UART1(Pressure >> 8);
              SendData_UART1((u8)Pressure);
              SendData_UART1(Temperature1 >> 8);
              SendData_UART1((u8)Temperature1);
              SendData_UART1(Temperature2 >> 8);
              SendData_UART1((u8)Temperature2);
              if (Ntc1ErrorFlag)
              {
                SMG_One_Display(DIG1, DIG_Dis[1]);
                SMG_One_Display(DIG2, DIG_Dis[11]);
              }
              else if (Ntc2ErrorFlag)
              {
                SMG_One_Display(DIG1, DIG_Dis[2]);
                SMG_One_Display(DIG2, DIG_Dis[11]);
              }
              else if (PressureErrorFlag)
              {
                SMG_One_Display(DIG1, DIG_Dis[3]);
                SMG_One_Display(DIG2, DIG_Dis[11]);
              }
            }
            PressureProcess(mode);
          }
          TaskNumber++;
          break;
        }
        case 5: //work process
        { 
          if(FlagState.work)
          {
            if (mode)
            {
              shockRun();
              WorkTimeDeal();
            }
            else
            {
              MOTOR1_PIN = 0;
            }
            if(FlagState.heat)
            {
              TemperatureProcess1();
              TemperatureProcess2();
            }
            else
            {
              TempWork1 = 0;
              TempWork2 = 0;
            }
          }
          else
          {
            DIG2_Dis = 0;
            SMG_One_Display(DIG3, DIG2_Dis);
            WorkTime = 959;
            TempIntensity = 1;
            mode = 0;
            FlagState.shock = 0;
            FlagState.heat = 0;
            MOTOR1_PIN = 0;
            TempWork1 = 0;
            TempWork2 = 0;
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
