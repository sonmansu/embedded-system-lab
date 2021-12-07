//텀프 main.c
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "stm32f10x_tim.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void USART1_Init(void);
void NVIC_Configure(void);
void ADC_Configure(void);
void Delay(void);

/* 핀매핑
  - 자석: PE0
  - RGB LED: PB12,13,14 (R,G,B순서대로 ), 공통단자: GND
  - S1버튼: PD11 (내부적으로 연결)
  - 부저(Piezo): PB0
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

    //블루투스 통신
    /* UART TX/RX port clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* USART1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /* USART2 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // S1버튼
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    //Piezo
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

void GPIO_Configure(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    //마그네틱
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD | GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* led 3개; R,G,B 핀 순대로  */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 블투 통신 */
    /* UART1 pin setting */
    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //RX
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // *Floating*?
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART2 */
    //TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //RX
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // *Floating*?
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // S1 버튼
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    //PIEZO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void Tim_Configure(void)
{
  TIM_TimeBaseInitTypeDef tim;
  tim.TIM_Period = 10000;
  tim.TIM_Prescaler = 7200;
  tim.TIM_ClockDivision = TIM_CKD_DIV1;
  tim.TIM_CounterMode = TIM_CounterMode_Up;
  tim.TIM_RepetitionCounter = 0x0000;
  //  PIEZO
  TIM_TimeBaseInit(TIM1, &tim);
  TIM_Cmd(TIM1, ENABLE);
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
}
void USART1_Init(void){
    // USART 1
    USART_InitTypeDef USART1_InitStructure;

    // Enable the USART1 peripheral
    USART_Cmd(USART1, ENABLE);
    // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
    USART1_InitStructure.USART_BaudRate = 9600;
    USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART1_InitStructure.USART_StopBits = USART_StopBits_1;
    USART1_InitStructure.USART_Parity = USART_Parity_No;
    USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART1_InitStructure);
    // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // USART 2
    USART_InitTypeDef USART2_InitStructure;
    // Enable the USART2 peripheral
    USART_Cmd(USART2, ENABLE);
    // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
    USART2_InitStructure.USART_BaudRate = 9600;
    USART2_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART2_InitStructure.USART_StopBits = USART_StopBits_1;
    USART2_InitStructure.USART_Parity = USART_Parity_No;
    USART2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART2_InitStructure);
    // TODO: Enable the USART2 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
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
void NVIC_Configure(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
    // UART1
    NVIC_EnableIRQ(USART1_IRQn); // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // UART2
    NVIC_EnableIRQ(USART2_IRQn); // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
void USART1_IRQHandler() {
    uint16_t word;
    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
    	// the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);
        USART_SendData(USART2, word);
        // clear 'Read data register not empty' flag
    	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}
void USART2_IRQHandler() {
    uint16_t word;
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){
    	// the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART2);
        USART_SendData(USART1, word);
        // clear 'Read data register not empty' flag
    	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}
char msg_menu[] = "메뉴를 입력 하세요\r \n- 약 먹을 시간 설정: 1\r\n- 타이머 시간 설정: 2\r\n";

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
void alert(){
  //일정 시간이 되었을때 piezo alert
  while(1){
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == Bit_SET){//약통이 닫혀있을때 alert
      GPIO_SetBits(GPIOB,GPIO_Pin_0);
      Delay();
      GPIO_ResetBits(GPIOB,GPIO_Pin_0);
      Delay();
    }
    else //약통 열면 break
      break;
  }
}
void Delay(void) {
    int i;
    for (i = 0; i < 2000000; i++) {}
}
int main(void) {
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    USART1_Init();
// ADC_Configure();
    NVIC_Configure();
    Tim_Configure();
    
//    while (1) {
//        if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == Bit_SET) { //자석붙었을 때
//            turn_rgbled(GREEN);
//        } else //안붙었을때
//            turn_rgbled(RED);
//    }
    alert();
    return 0;
}
