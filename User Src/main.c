/*****************************************************************************
Name: Heat&BIO
version: v1.0 Released
Editor: Aysi
Describe: 
History: 
2019.5.28: Created
*****************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#define DEBUG
#include "UserBaseLib.h"
#include "delay.h"
#include "UART_Frame.h"

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


void main(void)
{  
  //Init clock, CPU and Master clock is 16MHz
  CLK_HSICmd(ENABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  //io init
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_OUT_PP_LOW_SLOW);
   // GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_IN_PU_NO_IT);
  
  // GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);

  Uart1Init(115200);
// Tim1_Time_Upmode_conf(16, 1000, 0);
  //Timer4Init();       //Count 200us
  Tim2_Time_Upmode_conf(4, 1000);        //f: 1us, 1ms Int
  enableInterrupts();//Open main interrupt

  delay_init();
  TIM1_PWM_Init(0, 8000, 0);
  
#ifndef DEBUG
  IWDG_Configuration(); //Open IWDG
#endif
  
  while (1)
  {
#ifndef DEBUG
    IWDG->KR = 0xAA;  //Clear IWDG cnt
#endif

#ifdef DEBUG     
  //  usart1_send_char(0xaa);
    //GPIOA_OUT->ODR3 = ~ GPIOA_OUT->ODR3;
#endif 

    delay_ms(100);
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
