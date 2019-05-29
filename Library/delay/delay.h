/************************************************
Transplant from ALIENTEK STM32F103, use for STM8S
NOTE: Delect uCOS code
by: Aysi v1.0 2017.11.27
************************************************/

#ifndef __DELAY_H
#define __DELAY_H 

//#include "stm8s_it.h"
#include "stm8s.h"

void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u8 nus);

#endif





























