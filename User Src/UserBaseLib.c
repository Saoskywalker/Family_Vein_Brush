/************************************************************************
Name: UserBaseLib for N76E003 KEIL
Editor: Aysi
Date: 2019.10.15
************************************************************************/

#include "UserBaseLib.h"
#include "delay.h"

void PWM_Init(uint16_t Period, uint16_t pules)
{
  /**********************************************************************
	PWM frequency = Fpwm/((PWMPH,PWMPL) + 1) <Fpwm = Fsys/PWM_CLOCK_DIV> 
								= (16MHz/8)/(0x7CF + 1)
								= 1KHz (1ms)
***********************************************************************/

  PWM3_P04_OUTPUT_ENABLE; //pin use for pwm
  PWM_IMDEPENDENT_MODE; //pwm work mode
  PWM_CLOCK_DIV_8;
  PWMPH = (uint8_t)(Period>>8); //period
  PWMPL = (uint8_t)Period;
  PWM3H = (uint8_t)(pules>>8); //pules
  PWM3L = (uint8_t)pules;
  set_LOAD; //load period and pules values
  set_PWMRUN; //run pwm
  P04_PushPull_Mode; //set pin output mode
}

void AD1Init(void)
{
  Enable_ADC_AIN4; //p0.5
  clr_ADCF;
}

u16 AD1Sample(u8 channel)
{
  u16 advalue = 0, adtemp = 0; 
  u8 averageCnt = 0;
  
  //change Fose frequency to 4MHz, in order to reduce AD sample, raise steady
  //CKDIV = 0x02;

  if (channel == 4)
  {
    Enable_ADC_AIN4;
  }
  else if (channel == 0)
  {
    Enable_ADC_AIN0;
  }
  else if (channel == 1)
  {
    Enable_ADC_AIN1;
  }
  else
  {
    return 0;
  }

  advalue = 0;
  for (averageCnt = 0; averageCnt < 8; averageCnt++)
  {
    set_ADCS; //start adc convert
    while (ADCF == 0)
      ;       //check EOC flag
    clr_ADCF; //clear EOC flag
    adtemp = 0;
    adtemp = ADCRH;
    adtemp = (adtemp << 4) + ADCRL;
    advalue += adtemp;
  }
  advalue = advalue >> 3; // x/8

  Disable_ADC; //close ADC
  //CKDIV = 0x00; //recovery frequency
  
  return advalue;
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
    set_TR1;          //timer1 run
		set_TI;						//For printf function must setting TI = 1
    //clr_TI;
    clr_RI;

    //set_ES;           //enable UART interrupt //note:tx function must fix if use 
}

void Uart1Init(u32 u32Baudrate)
{
  P02_Quasi_Mode; //Setting UART pin as Quasi mode for transmit
  P16_Quasi_Mode; //Setting UART pin as Quasi mode for transmit

  //use timer3 as Baudrate generator
  SCON_1 = 0x50; //UART1 Mode1,REN_1=1,TI_1=1
  T3CON = 0x08;  //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1), UART1 in MODE 1
  clr_BRCK;

#ifdef FOSC_160000
  RH3 = HIBYTE(65536 - (1000000 / u32Baudrate) - 1); /*16 MHz */
  RL3 = LOBYTE(65536 - (1000000 / u32Baudrate) - 1); /*16 MHz */
#endif
#ifdef FOSC_166000
  RH3 = HIBYTE(65536 - (1037500 / u32Baudrate)); /*16.6 MHz */
  RL3 = LOBYTE(65536 - (1037500 / u32Baudrate)); /*16.6 MHz */
#endif
  set_TR3; //Trigger Timer3
}

//Config 16bit
void Tim2_Time_Upmode_conf(uint16_t TIM2_Period)
{
  TIMER2_DIV_4; //x*4/16000000 = ys
  TIMER2_Auto_Reload_Delay_Mode;

  RCMP2L = TIM2_Period;
  RCMP2H = TIM2_Period>>8;
  TL2 = 0;
  TH2 = 0;

  set_ET2;                // Enable Timer2 interrupt
  set_TR2;                // Timer2 run
}

volatile void Delay(uint16_t nCount) 
{
  //uint16_t i = nCount;

  while (nCount != 0)
  {
    nCount--;
    nop
  }
  // while (i != 0)
  // {
  //   i--;
  //   nop
  // }
}

/*************************************
 * WDG CONFIG for N76E003
 **note: N76E003 need enable WDG on config
****************************************/

/***********************************************************************
	WDT CONFIG enable 
	warning : this macro is only when ICP not enable CONFIG WDT function
	copy this marco code to you code to enable WDT reset.
************************************************************************/
static void Check_WDT_Reset_Config(void)
{
		set_IAPEN;
		IAPAL = 0x04;
		IAPAH = 0x00;	
		IAPFD = 0xFF;
		IAPCN = 0xC0;						//Read config command
		set_IAPGO;  
		if ((IAPFD&0xF0)==0xF0)
		{
				IAPFD = 0x0F;
				IAPCN = 0xE1;
        set_IAPGO;                         						//trigger IAP
				while((CHPCON&SET_BIT6)==SET_BIT6);          //check IAPFF (CHPCON.6)
				clr_CFUEN;
				clr_IAPEN;
				EA = 0;
				TA = 0xAA;
				TA = 0x55;
				CHPCON &= 0xFD;
				TA = 0xAA;
				TA = 0x55;
				CHPCON |= 0x80;
		}
		clr_IAPEN;
}
void IWDG_Configuration(void)
{
  //----------------------------------------------------------------------------------------------
  // WDT Init !!! ENABLE CONFIG WDT FIRST !!!
  // Warning:
  // Always check CONFIG WDT enable first, CONFIG not enable, SFR can't enable WDT reset
  // Please call Enable_WDT_Reset_Config() function to enable CONFIG WDT reset
  //----------------------------------------------------------------------------------------------
  Check_WDT_Reset_Config();

  TA = 0xAA;
  TA = 0x55;
  WDCON |= 0x07; //Setting WDT prescale, IWDG: 1.638s 
  set_WDCLR;     //Clear WDT timer
  while ((WDCON | ~SET_BIT6) == 0xFF);   //confirm WDT clear is ok before into power down mode
  //	EA = 1;
  set_WDTR; //WDT run
}

/*********************************************************
 N76E003 APROM program DATAFLASH as EEPROM way
 Dataflash use APROM area, please ALWAYS care the address of you code
 * ****************************************************/

/*************************************************************
 * Read_APROM_BYTE
 * *********************************************************/
unsigned char Read_APROM_BYTE(unsigned int code *u16_addr)
{
	unsigned char rdata;
	rdata = *u16_addr>>8;
	return rdata;
}

/********************************************************************
write_DATAFLASH_BYTE :
user can copy all this subroutine into project, then call this function in main.
***********************************************************************/	
volatile unsigned char xdata page_buffer[128];	
void Write_DATAFLASH_BYTE(unsigned int u16EPAddr,unsigned char u8EPData)
{
	unsigned char looptmp=0;
	unsigned int u16_addrl_r;
	unsigned int RAMtmp;
	
//Check page start address
	u16_addrl_r=(u16EPAddr/128)*128;
//Save APROM data to XRAM0
	for(looptmp=0;looptmp<0x80;looptmp++)
	{
		RAMtmp = Read_APROM_BYTE((unsigned int code *)(u16_addrl_r+looptmp));
		page_buffer[looptmp]=RAMtmp;
	}
// Modify customer data in XRAM
		page_buffer[u16EPAddr&0x7f] = u8EPData;
	
//Erase APROM DATAFLASH page
		IAPAL = u16_addrl_r&0xff;
		IAPAH = (u16_addrl_r>>8)&0xff;
		IAPFD = 0xFF;
	  set_IAPEN; 
		set_APUEN;
    IAPCN = 0x22; 		
 		set_IAPGO; 
		
//Save changed RAM data to APROM DATAFLASH
		set_IAPEN; 
		set_APUEN;
	  IAPCN = 0x21;
		for(looptmp=0;looptmp<0x80;looptmp++)
		{
			IAPAL = (u16_addrl_r&0xff)+looptmp;
      IAPAH = (u16_addrl_r>>8)&0xff;
			IAPFD = page_buffer[looptmp];
			set_IAPGO;			
		}
		clr_APUEN;
		clr_IAPEN;
}	

/********************** App part **************************/
//App define

//App value
u8 SendBufLen = 0;
u8 *SendBuffer = 0;
u8 UART1BusyFlag = 0;
_StateBit FlagState = {0, 0, 0, 0, 0, 0, 0, 0};

//App Function
// u8 EEPROMLocal = 0;
// u32 EECountTemp = 0;
// u32 EEPROMRestartDeal()
// {
// //    UART1_SendData8(FLASH_ReadByte(0x00004000));
// //  FLASH_Unlock(FLASH_MEMTYPE_DATA);
// //       FLASH_ProgramByte(pp, testP[iii]);
// //       FLASH_Lock(FLASH_MEMTYPE_DATA);
//   u16 *EEAddr = (u16 *)0x00004000;

//   while(*EEAddr>65000)
//   {
//     EECountTemp += *EEAddr;
//     EEAddr++;
//     if(++EEPROMLocal>=63)
//     {
//       return EECountTemp;
//     }
//   }
//   EECountTemp += *EEAddr;
//   return EECountTemp;
// }

// //Parameter: need to storage value
// //Note: ban from redance, can add or clear
// void EEPROMStorage(u32 *EECount)
// {
//   u16 *EEAddr = (u16 *)0x00004000;
//   u8 i = 0;
  
//   if(EEPROMLocal>=63)
//   {
//     return;
//   }
//   else
//   {
//     EEAddr = EEAddr+EEPROMLocal;
//     if(*EEAddr>65000)
//     {
//       EEAddr++;
//       EEPROMLocal++;
//       if(EEPROMLocal>=63)
//       {
//         return;
//       }
//     }
//     FLASH_Unlock(FLASH_MEMTYPE_DATA);
// //    FLASH_ProgramByte(pp, testP[iii]);
//     if(*EECount)
//     {
//       *EEAddr = (u16)((*EECount-EECountTemp)+*EEAddr);
//       EECountTemp = *EECount;
//     }
//     else
//     {     
//       EEAddr = (u16 *)0x00004000;
//       for(i = 0;i<=EEPROMLocal;i++)
//       {
//         *EEAddr = 0;
//         EEAddr++;
//       }
//     }
//     FLASH_Lock(FLASH_MEMTYPE_DATA);
//   }
// }

u8 ReceiveData_UART0(void)
{
  u8 c;
  while (!RI);
  c = SBUF;
  RI = 0;
  return (c);
}

void SendData_UART0(u8 c)
{
  TI = 0;
  SBUF = c;
  while (TI == 0);
}

u8 ReceiveData_UART1(void)
{
    u8 c;
    
    while (!RI_1);
    c = SBUF_1;
    RI_1 = 0;
    return (c);
}

void SendData_UART1 (u8 c)
{
    TI_1 = 0;
    SBUF_1 = c;
    while(TI_1==0);
}

//UART send Buffer
void UART0SendBuf(u8 *SendBufAddr, u8 SendLen)
{
  SendBufLen = SendLen;
  SendBuffer = SendBufAddr;
  UART1BusyFlag = 1;
//Send one Byte with UART0
  // Send_Data_To_UART0(*SendBuffer);
}
