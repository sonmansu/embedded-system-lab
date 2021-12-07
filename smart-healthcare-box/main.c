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
void USART1_Init(void);
void NVIC_Configure(void);
void ADC_Configure(void);
void Delay(void);
void turn_rgbled(int led_idx);
void sendStringUsart(USART_TypeDef* USARTx, char* msg);

/* 핀매핑
  - 자석: PE0
  - RGB LED: PB12,13,14 (R,G,B순서대로 ), 공통단자: GND
  - S1버튼: PD11 (내부적으로 연결)
*/

//RGB LED 변수
#define RED 0
#define GREEN 1
#define BLUE 2

char msg_menu[] = "\r\n============MENU============\r\n"
"1 => ALARM TIME\r\n"
"2 => TIMER DURATION\r\n"
"===========================\r\n"
"Select: ";
char msg_medicine_time[] = "It's time to take medicine. send '0' to finish the alarm\r\n";
char msg_medicine_fail[] = "You didn't take any medicine.\r\n";
//한글안됨..
//"1 => 메뉴를 입력 하세요\r \n- 약 먹을 시간 설정: 1\r\n- 타이머 시간 설정: 2\r\n";
//char msg_medicine_time[] = "약 복용 시간입니다. '0'을 전송하여 부저를 끌 수 있습니다.";
//char msg_medicine_fail[] = "약을 복용하지 않으셨습니다.";

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

     //임시 /* JoyStick Up/RIGHT/Down port clock enable */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
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

    //@임시    /* JoyStick up, down pin setting  UP(5), RIGHT(4) DOWN(2)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU | GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void EXTI_Configure(void) // stm32f10x_gpio.h 참고
{
    EXTI_InitTypeDef EXTI_InitStructure;
    // TODO: Select the GPIO pin (Joystick, button) used as EXTI Line using function 'GPIO_EXTILineConfig'
    // TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'

    /* Button */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // NOTE: do not select the UART GPIO pin used as EXTI Line here
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

void NVIC_Configure(void) {
    NVIC_InitTypeDef NVIC_InitStructure;

    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

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

    // User S1 Button
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler() {
    uint16_t word;
    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
       // the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);
        USART_SendData(USART2, word);

//        //제한시간이 지나면
//        sendStringUsart(USART2, msg_medicine_fail); //휴대폰으로 전송

        // clear 'Read data register not empty' flag
       USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}
void USART2_IRQHandler() {
    uint16_t word;
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){
       // the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART2);

        if (word == '0') //약복용 완료됐다는 것, 보드의 부저를 꺼야함
           turn_rgbled(RED);
        else if (word == '1') //알람 시간 입력됨
           turn_rgbled(BLUE);
        else if (word == '2') //타이머 지속 시간   입력됨
           turn_rgbled(GREEN);

        USART_SendData(USART1, word); //푸티에 출력
        // clear 'Read data register not empty' flag
       USART_ClearITPendingBit(USART2,USART_IT_RXNE);
    }
}
/* char[] 타입의 문자열 출력
매개변수 USART1: PUTTY로 출력
             USART2: 휴대폰으로 출력 */

void sendStringUsart(USART_TypeDef* USARTx, char* msg) {
  char *tmp = &msg[0];
  while (*tmp != '\0') {
      USART_SendData(USARTx, (uint16_t)*tmp);
       /* Wait till TC is set */
      while ((USARTx->SR & USART_SR_TC) == 0);
      tmp++;
  }
}

void EXTI15_10_IRQHandler(void) { // s1버튼이 눌리면 메뉴 출력

   if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
      if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == Bit_RESET) {
        sendStringUsart(USART2, msg_menu); //폰에 메뉴판 출력
      }
        EXTI_ClearITPendingBit(EXTI_Line11);
   }
}

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
void Delay(void) {
    int i;
    for (i = 0; i < 2000000; i++) {}
}
int main(void) {
    SystemInit();
    RCC_Configure();
    GPIO_Configure();
    USART1_Init();
    EXTI_Configure();
// ADC_Configure();
    NVIC_Configure();

    turn_rgbled(GREEN);

    while (1) {

        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == Bit_RESET)  {//약먹을시간되면  (임시로 조이스틱up시)
          sendStringUsart(USART2, msg_medicine_time); //폰에 약먹으라고 메세지 전송
        }

         if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_RESET)  //제한시간 초과시  (임시로 조이스틱down시)
          sendStringUsart(USART2, msg_medicine_fail); //약 복용안했다고 메세지 전송
    }
    return 0;
}
