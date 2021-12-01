//텀프 main.c
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configure(void);
void ADC_Configure(void);
void Delay(void);

/* 핀매핑
    자석: PE0
    RGB LED: PB12,13,14 (R,G,B순서대로 ), 공통단자: GND
*/

//RGB LED 변수
#define RED 0
#define GREEN 1
#define BLUE 2

//---------------------------------------------------------------------------------------------------

void RCC_Configure(void) {
// TODO: Enable the APB2 peripheral clock using the function
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE); // ADC1, port C RCC ENABLE

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); // 자석

    /* Alternate Function IO clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
//마그네틱
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD | GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* led3개 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//@ Set Pin Mode Input Pull-up/down UP
//@ Set Pin Speed Max : 50MHz
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//@ Set Pin Mode General Output Push-Pull
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
//@ Set Pin Mode Input Pull-up/down UP
//@ Set Pin Speed Max : 50MHz
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//@ Set Pin Mode General Output Push-Pull
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//@ Set Pin Mode Input Pull-up/down UP
//@ Set Pin Speed Max : 50MHz
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//@ Set Pin Mode General Output Push-Pull
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
////ADC init 함수
//void ADC_Configure(void) {
// ADC_InitTypeDef ADC_InitStructure;
// // ADC1 Configuration
// ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
// ADC_InitStructure.ADC_ScanConvMode = ENABLE;
// ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
// ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
// ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
// ADC_InitStructure.ADC_NbrOfChannel = 1;
// ADC_Init(ADC1, &ADC_InitStructure);
// ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1,
// ADC_SampleTime_239Cycles5);
// ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); // interrupt enable
// ADC_Cmd(ADC1, ENABLE); // ADC1 enable
// ADC_ResetCalibration(ADC1);
//
// while(ADC_GetResetCalibrationStatus(ADC1));
// ADC_StartCalibration(ADC1);
// while(ADC_GetCalibrationStatus(ADC1));
// ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//}
//
//void NVIC_Configure(void) {
// NVIC_InitTypeDef NVIC_InitStructure;
// // TODO: fill the arg you want
// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
// // ADC1
// //@ NVIC Line ADC1
// NVIC_EnableIRQ(ADC1_2_IRQn);
// NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// NVIC_Init(&NVIC_InitStructure);
//}
//// ADC는 인터럽트 베이스이므로 핸들러 작성 필요
//void ADC1_2_IRQHandler() {
// if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET){
// lightSenser = ADC_GetConversionValue(ADC1);
// ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
// }
//}

unsigned led_array[3] = {
//R,G,B 순서
    GPIO_Pin_12,
    GPIO_Pin_13,
    GPIO_Pin_14
};

void turn_rgbled(int led_idx) {
  for (int i = 0; i < 3; i++) {
    if (i == led_idx)
      GPIO_SetBits(GPIOB, led_array[i]); //애만 킴
    else
      GPIO_ResetBits(GPIOB, led_array[i]); //끔
  }
}
void turn_off(void) {
    GPIO_ResetBits(GPIOB, led_array[0]);
    GPIO_ResetBits(GPIOB, led_array[1]);
    GPIO_ResetBits(GPIOB, led_array[2]);
}


int main(void) {
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
// ADC_Configure();
// NVIC_Configure();

    while (1) {
        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == Bit_SET) { //자석붙었을 때
            turn_rgbled(GREEN);
        } else //안붙었을때
            turn_rgbled(RED);
    }
    return 0;
}
