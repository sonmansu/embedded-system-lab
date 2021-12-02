// http://poohyhoh.blogspot.com/2010/09/arm-cortex-m3-stm32-power-control-pwr-5.html
// ...

#include "stm32f10x_bkp.h" //?
#include "stm32f10x_rtc.h"

#define CRL_CNF_Set      ((uint16_t)0x0010)      /*!< Configuration Flag Enable Mask */
#define CRL_CNF_Reset    ((uint16_t)0xFFEF)      /*!< Configuration Flag Disable Mask */
#define PRLH_MSB_Mask    ((uint32_t)0x000F0000)  /*!< RTC Prescaler MSB Mask */
#define RTC_LSB_Mask     ((uint32_t)0x0000FFFF)  /*!< RTC LSB Mask */


void NVIC_Configuration(void) {
    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void RTC_IRQHandler(void) {
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
        RTC_ClearITPendingBit(RTC_IT_SEC); /* Clear the RTC Second interrupt */
       // LED_Toggle_Red(); /* Toggle Red */
        TimeDisplay = 1; /* Enable time update */

        RTC_WaitForLastTask(); /* Wait until last write operation on RTC registers has finished */
        /* Reset RTC Counter when Time is 23:59:59 */
        if (RTC_GetCounter() == 0x00015180) {
            RTC_SetCounter(0x0);
            RTC_WaitForLastTask(); /* Wait until last write operation */
        }
    }
}

void RTC_Configuration(void) {
   
   /* RTC clock source configuration */ /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    
    BKP_DeInit(); /* Reset Backup Domain */

    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) { }

    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); /* Select the RTC Clock Source */
    RCC_RTCCLKCmd(ENABLE); /* Enable the RTC Clock */

    RTC_WaitForSynchro(); /* Wait for RTC APB registers synchronisation */
    RTC_WaitForLastTask(); /* Wait until last write operation on RTC registers has finished */

    /* Set the RTC time base to 1s */
    RTC_SetPrescaler(39999); /* RTC period = RTCCLK/RTC_PR = (40 KHz)/(39999+1) */
    RTC_WaitForLastTask(); /* Wait until last write operation on RTC registers has finished *
    
    
    
    RTC_ITConfig(RTC_IT_ALR,ENABLE); /* Enable the RTC Alarm */
    RTC_WaitForLastTask(); /* Wait until last write operation */
}

void RTC_ITConfig(uint16_t RTC_IT, FunctionalState NewState)
{
  assert_param(IS_RTC_IT(RTC_IT)); /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE) {
    RTC->CRH |= RTC_IT;
  } else {
    RTC->CRH &= (uint16_t)~RTC_IT;
  }
}

void RTC_EnterConfigMode(void)
{
  /* Set the CNF flag to enter in the Configuration Mode */
  RTC->CRL |= CRL_CNF_Set;
}

void RTC_ExitConfigMode(void)
{
  /* Reset the CNF flag to exit from the Configuration Mode */
  RTC->CRL &= CRL_CNF_Reset;
}

void RTC_SetPrescaler(uint32_t PrescalerValue)
{
  assert_param(IS_RTC_PRESCALER(PrescalerValue)); /* Check the parameters */
  
  RTC_EnterConfigMode();
  /* Set RTC PRESCALER MSB word */
  RTC->PRLH = (PrescalerValue & PRLH_MSB_Mask) >> 16;
  /* Set RTC PRESCALER LSB word */
  RTC->PRLL = (PrescalerValue & RTC_LSB_Mask);
  RTC_ExitConfigMode();
}

void RTC_WaitForLastTask(void)
{
  /* Loop until RTOFF flag is set */
  while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET) { }
}

void RTC_WaitForSynchro(void)
{
  /* Clear RSF flag */
  RTC->CRL &= (uint16_t)~RTC_FLAG_RSF;
  /* Loop until RSF flag is set */
  while ((RTC->CRL & RTC_FLAG_RSF) == (uint16_t)RESET) { }
}


void EXTI_Configuration(void)
{ 

    /* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line    = EXTI_Line17;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}


//기타 참고자료
/*
STM32 HAL: RTC
https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=eziya76&logNo=221530955958

STM32 내부 RTC + HAL library
https://m.blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=eziya76&logNo=220986126032

STM32I432 RTC alarm example : 우리는 stm32f10x
https://github.com/CalebKang/stm32l432_rtc_alarm_example/blob/master/Core/Src/main.c

stm32f103-RTC_alarm
https://github.com/pioter77/stm32f103-RTC_alarm/blob/master/main.c
*/
