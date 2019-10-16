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
u8 BIOIntensity = 0;
const u16 BIO1IntensityTable[] = {0, 21, 22, 23, 24, 25,
																	26, 27, 28};
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
		if(++BIO1TimeCnt>=77)
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
}

//Heat
u8 TempIntensity = 0;
const u16 HeatIntensityTable[] = {0, 5, 7, 9, 12,
																	15, 20, 25, 32};
void HeatPWM(u8 i, u8 Work)
{
	static u16 HeatTimeCnt = 0;
	static u8 HeatModRenew = 0;
	
	if(Work)
	{
		if(++HeatTimeCnt>=40)
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

/*MCU LED&KEY IO use together function model*/
u8 MCU_DK_KeyData = 0, MCU_DK_DisLight = 30; //light relate to display period
struct {
 u8 dat1;
 u8 dat2;
 u8 dat3;
}DisStruct;

void MCU_DK_Display(u8 ch,u8 dat) //set SMG display
{
  if(ch==DIG1)
    DisStruct.dat1 = dat;
  else if(ch==DIG2)
    DisStruct.dat2 = dat;
  else if(ch==DIG3)
    DisStruct.dat3 = dat;
}

u8 MCU_DK_Key(void)	  //key scan
{
  return MCU_DK_KeyData;
}

void MCU_DK_SetLight(u8 i) //back light
{
  MCU_DK_DisLight = i;
}

void MCU_DK_DisAndKey_Handle(void) //note: put at timer funtion 
{
  static u8 step = 1, cnt = 0, ii = 0;
  static u8 keyCnt1 = 0, keyCnt2 = 0, keyCnt3 = 0, keyCnt4 = 0;

  if(++cnt>=50) //period 50Hz
  {
    cnt = 0;
    if(++step>4)
      step = 1;
    if(step>=4) //A~H become input
    {
      keyCnt1 = 0; //clear cnt
      keyCnt2 = 0;
      keyCnt3 = 0;
      GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT); //A
      GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT); //B
      GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); //C
      GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); //D
      GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT); //E
      GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT); //F
      GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT); //G
      GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT); //H
      //close channel
      CHANNEL1_PIN = 1;
      CHANNEL2_PIN = 1;
      CHANNEL3_PIN = 1;
    }
    else //A~H become output
    {
      GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_OD_HIZ_SLOW); //A
      GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_SLOW); //B
      GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_SLOW); //C
      GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW); //D
      GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_SLOW); //E
      GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_SLOW); //F
      GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW); //G
      GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_SLOW); //H

      //close channel
      CHANNEL1_PIN = 1;
      CHANNEL2_PIN = 1;
      CHANNEL3_PIN = 1;

      //renew display data
    switch (step)
    {
    case 1: 
      ii = DisStruct.dat1;
      break;

    case 2:
      ii = DisStruct.dat2;
      break;

    case 3:
      ii = DisStruct.dat3;
      break;

    default:
      break;
    }
    if (ii & 0X01)
      A_OUT_PIN = LED_OPEN;
    else
      A_OUT_PIN = LED_CLOSE;
    if (ii & 0X02)
      B_OUT_PIN = LED_OPEN;
    else
      B_OUT_PIN = LED_CLOSE;
    if (ii & 0X04)
      C_OUT_PIN = LED_OPEN;
    else
      C_OUT_PIN = LED_CLOSE;
    if (ii & 0X08)
      D_OUT_PIN = LED_OPEN;
    else
      D_OUT_PIN = LED_CLOSE;
    if (ii & 0X10)
      E_OUT_PIN = LED_OPEN;
    else
      E_OUT_PIN = LED_CLOSE;
    if (ii & 0X20)
      F_OUT_PIN = LED_OPEN;
    else
      F_OUT_PIN = LED_CLOSE;
    if (ii & 0X40)
      G_OUT_PIN = LED_OPEN;
    else
      G_OUT_PIN = LED_CLOSE;
    if (ii & 0X80)
      H_OUT_PIN = LED_OPEN;
    else
      H_OUT_PIN = LED_CLOSE;
    }  
  }
    
  if(step==4) //scan key, note key scan time: 1/4 period
  {
    if(F_IN_PIN==0)
    {
      if(++keyCnt1>=45) //sensibility
      {
        keyCnt1 = 45;
        MCU_DK_KeyData = KEY_TEMP;
        return;
      }
    }
    else
    {
      keyCnt1 = 0;
    }
    if(G_IN_PIN==0)
    {
      if(++keyCnt2>=45) //sensibility
      {
        keyCnt2 = 45;
        MCU_DK_KeyData = KEY_START;
        return;
      }
    }
    else
    {
      keyCnt2 = 0;
    }
    if(H_IN_PIN==0)
    {
      if(++keyCnt3>=45) //sensibility
      {
        keyCnt3 = 45;
        MCU_DK_KeyData = KEY_BIO;
        return;
      }
    }
    else
    {
      keyCnt3 = 0;
    }
    MCU_DK_KeyData = 0;
  }
  else if (cnt < MCU_DK_DisLight) //scan display
  {
    switch (step)
    {
    case 1: 
      CHANNEL1_PIN = 0;
      break;

    case 2:
      CHANNEL2_PIN = 0;
      break;

    case 3:
      CHANNEL3_PIN = 0;
      break;

    default:
      break;
    }
  }
  else
  { //close channel
    CHANNEL1_PIN = 1;
    CHANNEL2_PIN = 1;
    CHANNEL3_PIN = 1;
  }
}

void main(void)
{ 
  // u16 advalue = 0, s1 = 0; 
  const u8 DIG1_Dis[] = {BIT7_1, BIT6_1}; 
  const u8 DIG2_Dis[] = {0, 0X01, 0X03, 0X07, 0X0F, 0X1F, 0X3F, 0X7F, 0XFF}; 
  const u8 DIG3_Dis[] = {0, 0X01, 0X03, 0X07, 0X0F, 0X1F, 0X3F, 0X7F, 0XFF}; 
  u8 TaskNumber = 1, KeyValue = 0;
  u8 KeyUp = 1;
  // u8 *EEAddr = (u8 *)0x00004000;

  //Init clock, CPU and Master clock is 16MHz
  CLK_HSICmd(ENABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  //io init
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW); //HEAT
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_OD_LOW_SLOW); //LED
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_SLOW); //Channel 1
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW); //Channel 2
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_OUT_PP_HIGH_SLOW); //Channel 3
  GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW); //BIO S
  GPIO_Init(GPIOA, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW); //BIO A

  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT); //A
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT); //B
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT); //C
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_IN_PU_NO_IT); //D
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT); //E
  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT); //F
  GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_IN_PU_NO_IT); //G
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT); //H

  // Uart1Init(115200);
// Tim1_Time_Upmode_conf(16, 1000, 0);
  //Timer4Init();       //Count 200us
  Tim2_Time_Upmode_conf(4, 100);        
  enableInterrupts();//Open main interrupt

  delay_init();
  // TIM1_PWM_Init(0, 8000, 0);
  AD1Init();
  // TM1650_Init(0, 1);

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

  // usart1_send_char(0X01); //version number
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
          SMG_One_Display(DIG3, DIG1_Dis[FlagState.work]);
          SMG_One_Display(DIG1, DIG2_Dis[BIOIntensity]);
          SMG_One_Display(DIG2, DIG3_Dis[TempIntensity]);
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
          // if (++s1 >= 100) //1s
          // {
          //   s1 = 0;
          //   for (u8 ioi = 0; ioi < 8; ioi++)
          //   {
          //     ADC1->CR1 |= ADC1_CR1_ADON; //start adc convert
          //     while (!(ADC1->CSR & 0X80))
          //       ;                     //check EOC flag
          //     ADC1->CSR &= ~(1 << 7); //clear EOC flag
          //     advalue += ADC1_GetConversionValue();
          //   }
          //   advalue = advalue >> 3;
          //   usart1_send_char(advalue >> 8);
          //   usart1_send_char((u8)advalue);
          //   advalue = 0;
          // }

          if(FlagState.work)
          {
            LED_CON = 1; 
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
            LED_CON = 0;
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
