// https://www.basic4mcu.com/bbs/board.php?bo_table=gesiyo4&wr_id=43&sca=STM32&page=2&device=pc
// 노력하고 있습니다!
include "stm32f10x_rtc.h"


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
        LED_Toggle_Red(); /* Toggle Red */
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
..... ..... ..... ..... ..... .....
    RTC_ITConfig(RTC_IT_ALR,ENABLE); /* Enable the RTC Alarm */
    RTC_WaitForLastTask(); /* Wait until last write operation */
}
