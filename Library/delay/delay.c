/************************************************
Transplant from ALIENTEK STM32F103, use for STM8S
NOTE: Delect uCOS code
by: Aysi v1.0 2017.11.27
************************************************/

#include "delay.h"
			 
void delay_init()
{
  TIM4_TimeBaseInit(TIM4_PRESCALER_16, 200);//prescaler: 16, reload: 200
	/* Clear TIM4 update flag */
//	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
        TIM4_ClearFlag(TIM4_FLAG_UPDATE);//clear time up flag
	/* Enable update interrupt */
	TIM4_ITConfig(TIM4_IT_UPDATE, DISABLE);//disable int
//	TIM4_Cmd(ENABLE);
}

//Delay 1us, but have a bug: usually delay is 4+nus us, value can not be 0		    								   
void delay_us(u8 nus)
{
//		TIM4_SetAutoreload(nus);//reload value                
//		TIM4_SetCounter(0);
//		TIM4_Cmd(ENABLE);
                TIM4->ARR = nus;//reload value
                TIM4->CNTR = 0;//clear count
                TIM4->CR1 |= 0X01;//start timer4
		while(!TIM4_GetFlagStatus(TIM4_FLAG_UPDATE));//wait time up
//                while(Timer4Flag==0);
//                Timer4Flag = 0;
//		TIM4_Cmd(DISABLE);//Disable Timer4
//                TIM4_ClearFlag(TIM4_FLAG_UPDATE);
                TIM4->CR1 &= ~0x01;//Disable Timer4
                TIM4->SR1 = 0;//clear time up flag
//		TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
//		TIM4_SetCounter(0);//clear count
                TIM4->CNTR = 0;	//clear count
}

//Delay 1ms,note: has a bug(delay isn't accuracy)
void delay_ms(u16 nms)
{	
  u8 Cnt1ms;
  u16 Cntms;	
  
  TIM4->ARR = 200;
  TIM4->CNTR = 0;
  TIM4->CR1 |= 0X01; 
  for(Cntms = 0;Cntms<=nms;Cntms++)
  {
    for(Cnt1ms = 0;Cnt1ms<=5;Cnt1ms++)
    {
      while(!TIM4_GetFlagStatus(TIM4_FLAG_UPDATE));//wait time up
      TIM4->SR1 = 0;//clear time up flag
    }
  }
  TIM4->CR1 &= ~0x01;//Disable Timer4
  TIM4->SR1 = 0;//clear time up flag
  TIM4->CNTR = 0;	//clear count	
    
} 









































