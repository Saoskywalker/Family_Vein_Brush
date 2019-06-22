/*****************************************************************************
Name: Heat&BIO
version: v1.0 Released
Editor: Aysi
Describe: 
History: 
2019.5.28: Created
2019.6.1; OK
*****************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
// #define DEBUG
#include "UserBaseLib.h"
#include "delay.h"
#include "UART_Frame.h"
#include "TM1650.h"

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

//BIO1 PWM
const u16 BIO1IntensityTable[] = {8001, 7200, 7000, 6200, 6000,
																	5000, 4000, 3000, 2000};
const u16 BIO1ModPeriod[] = {11, 250, 400};
const u16 BIO1ModCompare[] = {3, 140, 240};	
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
			TIM1_SetCompare4(4000);	//PWM_BIO
		else
			TIM1_SetCompare4(0);	//PWM_BIO
	}
	else
	{
		BIO1TimeCnt = 0;
		TIM1_SetCompare4(0);	//PWM_BIO
	}
}

//Heat
u8 TempIntensity = 0;
const u16 HeatIntensityTable[] = {0, 100, 200, 300, 400,
																	500, 600, 700, 800};
void HeatPWM(u8 i, u8 Work)
{
	static u16 HeatTimeCnt = 0;
	static u8 HeatModRenew = 0;
	
	if(Work)
	{
		if(++HeatTimeCnt>=1000)
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
}

void main(void)
{  
  const u8 DIG1_Dis[] = {0, BIT0_1}; 
  const u8 DIG2_Dis[] = {0, 0X01, 0X03, 0X07, 0X0F, 0X1F, 0X3F, 0X7F, 0XFF}; 
  const u8 DIG3_Dis[] = {0, 0X01, 0X03, 0X07, 0X0F, 0X1F, 0X3F, 0X7F, 0XFF}; 
  u8 TaskNumber = 1, KeyValue = 0, BIOIntensity = 0;
  u8 KeyUp = 1;
  // u8 *EEAddr = (u8 *)0x00004000;

  //Init clock, CPU and Master clock is 16MHz
  CLK_HSICmd(ENABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  //io init
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);

  Uart1Init(115200);
// Tim1_Time_Upmode_conf(16, 1000, 0);
  //Timer4Init();       //Count 200us
  Tim2_Time_Upmode_conf(4, 1000);        //f: 1us, 2ms Int
  enableInterrupts();//Open main interrupt

  delay_init();
  TIM1_PWM_Init(0, 8000, 0);
  AD1Init();
  TM1650_Init(3, 1);

  // BIOIntensity = *EEAddr;
  // TempIntensity = *(EEAddr+1);
  // if((BIOIntensity>8)||(BIOIntensity>8))
  // {
  //   FLASH_Unlock(FLASH_MEMTYPE_DATA);
  //   BIOIntensity = 0;
  //   TempIntensity = 0;
  //   *EEAddr = BIOIntensity;
  //   EEAddr++;
  //   *EEAddr = TempIntensity;
  //   EEAddr--;
  //   FLASH_Lock(FLASH_MEMTYPE_DATA);
  // }

  usart1_send_char(0X01); //version number
 // ADC1_StartConversion();

#ifndef DEBUG
  IWDG_Configuration(); //Open IWDG
#endif
  
  while (1)
  {
#ifndef DEBUG
    IWDG->KR = 0xAA;  //Clear IWDG cnt
#endif

#ifdef DEBUG
// usart1_send_char(TM1650_Key());
// TM1650_Display(0x6c, 0xFF);
// delay_ms(500);
// TM1650_Display(0x6c, 0x03);
// delay_ms(500);
  //  usart1_send_char(0xaa);
    //GPIOA_OUT->ODR3 = ~ GPIOA_OUT->ODR3;
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
          SMG_One_Display(DIG1, DIG1_Dis[FlagState.work]);
          SMG_One_Display(DIG2, DIG2_Dis[BIOIntensity]);
          SMG_One_Display(DIG3, DIG3_Dis[TempIntensity]);
          KeyValue = Key_Get();
          TaskNumber++;
          break;
        }
        case 3: //key process
        {       
          if(KeyValue!=KEY_START&&KeyValue!=KEY_BIO&&KeyValue!=KEY_TEMP)
          {
            KeyUp = 1;
          }   
          if((KeyValue==KEY_START)&&KeyUp)
          {
            KeyUp = 0;
            if(FlagState.work)
            {
              FlagState.work = 0;
            }
            else
            {
              FlagState.work = 1;
            }          
          }
          else if((KeyValue==KEY_BIO)&&KeyUp)
          {           
            KeyUp = 0;
            if(BIOIntensity==8)
            {
              BIOIntensity = 0;
            }
            else
            {
              BIOIntensity++;
            }     
          }
          else if((KeyValue==KEY_TEMP)&&KeyUp)
          {
            KeyUp = 0;
            if(TempIntensity==8)
            {
              TempIntensity = 0;
            }
            else
            {
              TempIntensity++;
            }   
          }     
          TaskNumber++;    
          break;
        }
        case 4: //work process
        {
          if(FlagState.work)
          {
            TIM1_SetCompare3(BIO1IntensityTable[BIOIntensity]);   
            LED_CON = LED_CON_OPEN; 
            // if((*EEAddr!=BIOIntensity)||(*(EEAddr+1)!=TempIntensity))
            // {
            //   FLASH_Unlock(FLASH_MEMTYPE_DATA);
            //   *EEAddr = BIOIntensity;
            //   EEAddr++;
            //   *EEAddr = TempIntensity;
            //   EEAddr--;
            //   FLASH_Lock(FLASH_MEMTYPE_DATA);
            // }
          }
          else
          {
            TIM1_SetCompare3(BIO1IntensityTable[0]);
            LED_CON = LED_CON_CLOSE;
          }     
          TaskNumber++;
          break;
        }
        case 5: //error process
        {
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

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
