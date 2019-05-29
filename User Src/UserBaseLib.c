/************************************************************************
Name: UserBaseLib for STM8S IAR
Editor: Aysi
Date: 2017.11.23
************************************************************************/

#include "UserBaseLib.h"

u8 SendBufLen = 0;
u8 *SendBuffer = 0;
u8 UART1BusyFlag = 0;
//_TimeBit FlagTime = {0, 0, 0, 0, 0, 0, 0, 0};

//IAR STM8S Function
void Timer4Init(void)//Count 200us
{
  /*TIM4_UpdateDisableConfig(ENABLE);//��������¼�
  TIM4_ARRPreloadConfig(ENABLE);//�Զ���װ
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);//�ж����ã������ж�
  TIM4_SetCounter(0xff);//��������ֵ
  TIM4_SetAutoreload(0xFF);//�������Զ���װ�ĳ�ֵ
  TIM4_PrescalerConfig(TIM4_PRESCALER_128, TIM4_PSCRELOADMODE_UPDATE);//Ԥ��Ƶֵ
  */
  TIM4_TimeBaseInit(TIM4_PRESCALER_16, 200);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(DISABLE);
}  

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

void Uart1Init(u16 baud) 
{
  UART1_DeInit();
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);  // PD3 = RD/DE
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST); // PD5 = TX1
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);      // PD6 = RX1
  UART1_Init(
             baud,
             UART1_WORDLENGTH_8D,
             UART1_STOPBITS_1_5,
             UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TXRX_ENABLE);
  
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  UART1->SR &= ~0xC0;
//  UART1->CR2 |= 1<<6;//enable TX interrupt
  UART1_Cmd(ENABLE);
}

//UART send Buffer
void UART1SendBuf(u8 *SendBufAddr, u8 SendLen)
{
  SendBufLen = SendLen;
  SendBuffer = SendBufAddr;
  UART1BusyFlag = 1;
  UART1->DR = *SendBuffer;//Send one Byte with UART1
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

volatile void Delay(uint16_t nCount)//about 0.5us
{
  uint16_t i = nCount;
  while (nCount != 0)
  {
    nCount--;
    asm("nop");
  }
  while (i != 0)
  {
    i--;
    asm("nop");
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
