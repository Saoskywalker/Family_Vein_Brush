/************************************************************************
Name: UserBaseLib for N76E003 KEIL
Editor: Aysi
Date: 2019.10.15
************************************************************************/

#include "UserBaseLib.h"
#include "delay.h"

u8 SendBufLen = 0;
u8 *SendBuffer = 0;
u8 UART1BusyFlag = 0;
_StateBit FlagState = {0, 0, 0, 0, 0, 0, 0, 0};



void Tim1_Time_Upmode_conf(uint16_t TIM1_Prescaler,
                           uint16_t TIM1_Period,
                           uint8_t TIM1_RepetitionCounter)
{
  /* Set the Autoreload value */
  TIM1->ARRH = (uint8_t)(TIM1_Period >> 8);
  TIM1->ARRL = (uint8_t)(TIM1_Period);
  
  /* Set the Prescaler value */
  TIM1->PSCRH = (uint8_t)(TIM1_Prescaler >> 8);
  TIM1->PSCRL = (uint8_t)(TIM1_Prescaler);
  
  /* Select the Counter Mode */
  TIM1->CR1 = (uint8_t)((uint8_t)(TIM1->CR1 & (uint8_t)(~(0x60  | 0x10)))
                       | (uint8_t)(0x00));
  
  /* Set the Repetition Counter value */
  TIM1->RCR = TIM1_RepetitionCounter;
  /* Set the ARPE Bit */
  TIM1->CR1 |= 0x80;
  /* Enable the Interrupt Upmode sources */
  TIM1->IER |= (uint8_t)TIM1_IT_UPDATE;
  /* set or Reset the CEN Bit */
  TIM1->CR1 |= 0x01;
}

void TIM1_PWM_Init(uint16_t TIM1_Prescaler, uint16_t TIM1_Period, uint16_t pules)
{
  //timer1 ch3: PC3 ch4: PC4
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_SLOW);
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);

  TIM1_DeInit();
  TIM1_TimeBaseInit(TIM1_Prescaler, TIM1_COUNTERMODE_UP, TIM1_Period, 0x00); 
  TIM1_ARRPreloadConfig(ENABLE);
  TIM1_OC4Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_ENABLE, pules, 
                TIM1_OCPOLARITY_LOW, TIM1_OCIDLESTATE_RESET);
  TIM1_OC3Init(TIM1_OCMODE_PWM2, TIM1_OUTPUTSTATE_DISABLE, TIM1_OUTPUTNSTATE_DISABLE, 
              TIM1_Period+1, TIM1_OCPOLARITY_LOW, TIM1_OCNPOLARITY_HIGH,
               TIM1_OCIDLESTATE_RESET, TIM1_OCNIDLESTATE_SET);
  TIM1_CCxCmd(TIM1_CHANNEL_4, ENABLE);
  TIM1_CCxCmd(TIM1_CHANNEL_3, ENABLE);
  TIM1_OC4PreloadConfig(ENABLE);
  TIM1_OC3PreloadConfig(ENABLE);
  TIM1_CtrlPWMOutputs(ENABLE);
  TIM1_Cmd(ENABLE);
}

void AD1Init(void)
{
  Enable_ADC_AIN4;
  clr_ADCF;
}

void Uart0Init(u32 u32Baudrate) 
{
  P06_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
  P07_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit

  SCON = 0x50;     	//UART0 Mode1,REN=1,TI=1
  TMOD |= 0x20;    	//Timer1 Mode1
  
  set_SMOD;        	//UART0 Double Rate Enable
  set_T1M;
  clr_BRCK;        	//Serial port 0 baud rate clock source = Timer1
 
#ifdef FOSC_160000
    TH1 = 256 - (1000000/u32Baudrate+1);               /*16 MHz */
#endif    	
#ifdef FOSC_166000
    TH1 = 256 - (1037500/u32Baudrate);         		     /*16.6 MHz */
#endif
    set_TR1;
		set_TI;						//For printf function must setting TI = 1
}

//UART send Buffer
void UART0SendBuf(u8 *SendBufAddr, u8 SendLen)
{
  SendBufLen = SendLen;
  SendBuffer = SendBufAddr;
  UART1BusyFlag = 1;
//Send one Byte with UART0
  Send_Data_To_UART0(*SendBuffer);
}

//Config 16bit Timer2: f=f_master/2^[3:0]
void Tim2_Time_Upmode_conf(uint8_t TIM2_Prescaler,uint16_t TIM2_Period)
{
    TIM2->PSCR = (uint8_t)(TIM2_Prescaler);
    /* Set the Autoreload value */
    TIM2->ARRH = (uint8_t)(TIM2_Period >> 8);
    TIM2->ARRL = (uint8_t)(TIM2_Period);
    /* Set or reset the UG Bit */
    TIM2->EGR = (uint8_t)TIM2_PSCRELOADMODE_IMMEDIATE; 
    /*!< Auto-Reload Preload Enable mask. */
    TIM2->CR1 |= 0x80;
    /* Enable the Interrupt sources */
    TIM2->IER |= (uint8_t)TIM2_IT_UPDATE;
    /*!< Counter Enable mask. */
    TIM2->CR1 |= 0x01;
}

volatile void Delay(uint16_t nCount) 
  uint16_t i = nCount;
  while (nCount != 0)
  {
    nCount--;
    nop
  }
  while (i != 0)
  {
    i--;
    nop
  }
}

//open IWDG: 1.02s
void IWDG_Configuration(void)
{
  /* Enable the IWDG*/
  IWDG->KR = IWDG_KEY_ENABLE;
  /* Enable the access to the IWDG registers*/
  IWDG->KR = (uint8_t)IWDG_WriteAccess_Enable; /* Write Access */
  /* Fixe IWDG Reset period */
  IWDG->PR = (uint8_t)IWDG_Prescaler_256;
  IWDG->RLR = 0xFF;
  /* Refresh IWDG */
  IWDG->KR = IWDG_KEY_REFRESH ;
}

//App define

//App value

u8 EEPROMLocal = 0;
u32 EECountTemp = 0;

//App Function

u32 EEPROMRestartDeal()
{
//    UART1_SendData8(FLASH_ReadByte(0x00004000));
//  FLASH_Unlock(FLASH_MEMTYPE_DATA);
//       FLASH_ProgramByte(pp, testP[iii]);
//       FLASH_Lock(FLASH_MEMTYPE_DATA);
  u16 *EEAddr = (u16 *)0x00004000;

  while(*EEAddr>65000)
  {
    EECountTemp += *EEAddr;
    EEAddr++;
    if(++EEPROMLocal>=63)
    {
      return EECountTemp;
    }
  }
  EECountTemp += *EEAddr;
  return EECountTemp;
}

//Parameter: need to storage value
//Note: ban from redance, can add or clear
void EEPROMStorage(u32 *EECount)
{
  u16 *EEAddr = (u16 *)0x00004000;
  u8 i = 0;
  
  if(EEPROMLocal>=63)
  {
    return;
  }
  else
  {
    EEAddr = EEAddr+EEPROMLocal;
    if(*EEAddr>65000)
    {
      EEAddr++;
      EEPROMLocal++;
      if(EEPROMLocal>=63)
      {
        return;
      }
    }
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
//    FLASH_ProgramByte(pp, testP[iii]);
    if(*EECount)
    {
      *EEAddr = (u16)((*EECount-EECountTemp)+*EEAddr);
      EECountTemp = *EECount;
    }
    else
    {     
      EEAddr = (u16 *)0x00004000;
      for(i = 0;i<=EEPROMLocal;i++)
      {
        *EEAddr = 0;
        EEAddr++;
      }
    }
    FLASH_Lock(FLASH_MEMTYPE_DATA);
  }
}

