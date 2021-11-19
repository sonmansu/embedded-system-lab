//7주차
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#include "misc.h"

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void USART1_Init(void);
void NVIC_Configure(void);

void EXTI15_10_IRQHandler(void);

void Delay(void);

void sendDataUART1(uint16_t data);

//---------------------------------------------------------------------------------------------------

int myFlag = 1;

void RCC_Configure(void) // stm32f10x_rcc.h 참고
{
  // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'

  /* UART TX/RX port clock enable */
  RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE );

  /* JoyStick Up/Down port clock enable */
  RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOC, ENABLE );


  /* LED port clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

  /* USART1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  /* Alternate Function IO clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) // stm32f10x_gpio.h 참고
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'

  /* JoyStick up, down pin setting */ // 업: pc5, 다운: pc2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // Input with Pull-Up
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* button pin setting */ //s1버튼 : pd11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // Input with Pull-Up
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* LED pin setting*/ //LED1,2,3,4: PD 2,3,4,5
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* UART pin setting */
  //TX : pa9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //Alternate Function output Push Pull
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //RX : pa10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD | GPIO_Mode_IPU; //Input with Pull-Up / Pull-Down
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void EXTI_Configure(void) // stm32f10x_gpio.h 참고
{
  EXTI_InitTypeDef EXTI_InitStructure;

  // TODO: Select the GPIO pin (Joystick, button) used as EXTI Line using function 'GPIO_EXTILineConfig'
  // TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'

  /* Joystick Down */ //PC2
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Joystick Up */ //PC5
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Button */ //PD11
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  // NOTE: do not select the UART GPIO pin used as EXTI Line here // 왜지?
}

void USART1_Init(void) // stm32f10x_usart.h 참고
{
  USART_InitTypeDef USART1_InitStructure;

  // Enable the USART1 peripheral
  USART_Cmd(USART1, ENABLE);

  // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
  /*
  BaudRate: 9600
  WordLength: 8bits
  Parity: None
  StopBits: 1bit
  Hardware Flow Control: None
  */
  USART1_InitStructure.USART_BaudRate = 9600;
  USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART1_InitStructure.USART_Parity = USART_Parity_No;
  USART1_InitStructure.USART_StopBits = USART_StopBits_1;
  USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART1_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //이거없으면 통신안됨
  USART_Init(USART1, &USART1_InitStructure);


  // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //@@RXNE가뭐지??
}

void NVIC_Configure(void) { // misc.h 참고

  NVIC_InitTypeDef NVIC_InitStructure;

  // TODO: fill the arg you want
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'

  // Joystick Down //PC2
  NVIC_InitStructure.NVIC_IRQChannel =  EXTI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Joystick Up //PC5
  NVIC_InitStructure.NVIC_IRQChannel =  EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //애도 0으로 줄 수 있나?
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  // User S1 Button //PD11
  NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //애도 0으로 줄 수 있나?
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // UART1
  // 'NVIC_EnableIRQ' is only required for USART setting
  NVIC_EnableIRQ(USART1_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // TODO
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void USART1_IRQHandler() {
  uint16_t word;
  if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
    // the most recent received data by the USART1 peripheral
    word = USART_ReceiveData(USART1);
    // TODO implement, 키보드 입력 'd' 또는 'u'에 따라 동작
    if (word == 'u') {
      myFlag = 1; //A동작
    } else if (word == 'd') {
      myFlag = 2; //B동작
    }
    // clear 'Read data register not empty' flag
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
  }
}
char str[] = "TEAM07\r\n";

void EXTI15_10_IRQHandler(void) { // when the button is pressed
  if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
    if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == Bit_RESET) {
      // TODO implement
      /* send UART 1 */
      char *tmp = &str[0];
      while (*tmp != '\0') {
        sendDataUART1(*tmp);
        tmp++;
      }
    }
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
}


// TODO: Create Joystick interrupt handler functions
/* Use "EXTIx_IRQHandler()" */
void EXTI2_IRQHandler(void) { //조이스틱다운 PC2
  if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_RESET) {
      myFlag = 2; //B 동작
    }
    EXTI_ClearITPendingBit(EXTI_Line2);
  }
}
//조이스틱업 pc5
void EXTI9_5_IRQHandler(void) {
  if(EXTI_GetITStatus(EXTI_Line5) != RESET) {
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == Bit_RESET) {
      myFlag = 1; //A동작
    }
    EXTI_ClearITPendingBit(EXTI_Line5);
  }
}



void Delay(void) {
  int i;

  for (i = 0; i < 2000000; i++) {}
}

void sendDataUART1(uint16_t data) {
  USART_SendData(USART1, data);
  /* Wait till TC is set */
  while ((USART1->SR & USART_SR_TC) == 0);
}

int main(void)
{
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  EXTI_Configure();
  USART1_Init();
  NVIC_Configure();

  uint16_t LED_arr[4] = {GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_7};
  int led_idx = 0;

  while (1) {
    // TODO: implement
    led_idx = (led_idx+4) % 4; //+4를 해줘야 음수가 안나옴

    for (int i = 0; i < 4; i ++) {
      if (i == led_idx)  GPIO_SetBits(GPIOD, LED_arr[i]);
      else                  GPIO_ResetBits(GPIOD, LED_arr[i]);
    }

    if (myFlag == 1) { //A동작
      led_idx++;
    } else if (myFlag == 2) { //B동작
      led_idx--;
    }

    // Delay
    Delay();
  }
  return 0;
}
