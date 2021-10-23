//Embedded System Design and Lab Thursday team7 

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

#include "misc.h"

int sw_state = 0;
int ua_state = 0;
/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void EXTI_Configure(void);
void USART1_Init(void);
void NVIC_Configure(void);

void EXTI15_10_IRQHandler(void);

void Delay(void);

void sendDataUART1(uint16_t data);

void led_up();
void led_down();
//---------------------------------------------------------------------------------------------------

void RCC_Configure(void) // stm32f10x_rcc.h 참고
{
	// TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
	
	/* UART TX/RX port clock enable */
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	/* JoyStick Up/Down port clock enable */
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/* LED port clock enable */
         
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	/* USART1 clock enable */
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/* Alternate Function IO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure(void) // stm32f10x_gpio.h 참고
{
    GPIO_InitTypeDef JoyStick;
    GPIO_InitTypeDef LED;
    GPIO_InitTypeDef USART;
    GPIO_InitTypeDef Button;   //다 따로 만들면? https://pkr7098.tistory.com/120
    GPIO_InitTypeDef GPIO_InitStructureRX;
    GPIO_InitTypeDef GPIO_InitStructureTX;
	// TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
	
    /* JoyStick up, down pin setting */
    JoyStick.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5;
    JoyStick.GPIO_Speed = GPIO_Speed_50MHz;
    JoyStick.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &JoyStick);
    
    /* button pin setting */
    Button.GPIO_Pin = GPIO_Pin_11;
    Button.GPIO_Speed = GPIO_Speed_50MHz;
    Button.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &Button);
    
    /* LED pin setting*/
    LED.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
    LED.GPIO_Speed = GPIO_Speed_50MHz;
    LED.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &LED);
	
    /* UART pin setting */
    //TX
    GPIO_InitStructureTX.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructureTX.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructureTX.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructureTX);
    //RX
    GPIO_InitStructureRX.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructureRX.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructureRX.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructureRX);
	
}

void EXTI_Configure(void){ // stm32f10x_gpio.h 참down
  
    EXTI_InitTypeDef JoyStick_down;
     EXTI_InitTypeDef JoyStick_up;
     EXIT_InitTypeDef Button;

	// TODO: Select the GPIO pin (Joystick, button) used as EXTI Line using function 'GPIO_EXTILineConfig'
	// TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'
    
     //Joystick_Down
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
    JoyStick_down.EXTI_Line = EXTI_Line2;
    JoyStick_down.EXTI_Mode = EXTI_Mode_Interrupt;
    JoyStick_down.EXTI_Trigger = EXTI_Trigger_Falling;
    JoyStick_down.EXTI_LineCmd = ENABLE;
    EXTI_Init(&JoyStick_down);

    /* Joystick Up */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
    JoyStick_up.EXTI_Line = EXTI_Line5;
    JoyStick_up.EXTI_Mode = EXTI_Mode_Interrupt;
    JoyStick_up.EXTI_Trigger = EXTI_Trigger_Falling;
    JoyStick_up.EXTI_LineCmd = ENABLE;
    EXTI_Init(&JoyStick_up);
    
    /* Button */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
    Button.EXTI_Line = EXTI_Line11;
    Button.EXTI_Mode = EXTI_Mode_Interrupt;
    Button.EXTI_Trigger = EXTI_Trigger_Falling;
    Button.EXTI_LineCmd = ENABLE;
    EXTI_Init(&Button);
    
    // NOTE: do not select the UART GPIO pin used as EXTI Line here
}

void USART1_Init(void) // stm32f10x_usart.h 참고
{
	USART_InitTypeDef USART1_InitStructure;

	// Enable the USART1 peripheral
	USART_Cmd(USART1, ENABLE);
	
	// TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
	USART_InitStructure.USART_BaudRate = 9600;
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        USART_InitStructure.USART_Parity = USART_Parity_No;
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        /* 
       BaudRate: 9600
       WordLength: 8bits
       Parity: None
       StopBits: 1bit
       Hardware Flow Control: None
     */
	
	// TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
	USART_ITConfig(USART1 , USART_IT_RXNE , ENABLE);
}

void NVIC_Configure(void) { // misc.h 참고

    NVIC_InitTypeDef JoyStick_down;
    NVIC_InitTypeDef JoyStick_up;
    NVIC_InitTypeDef Button;
    NVIC_InitTypeDef UART1;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
	
    // Joystick Down
    JoyStick_down.NVIC_IRQChannel = EXTI2_IRQn;
    JoyStick_down.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    JoyStick_down.NVIC_IRQChannelSubPriority = 0; // TODO
    JoyStick_down.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&JoyStick_down);
    // Joystick Up
    JoyStick_up.NVIC_IRQChannel = EXT9_5_IRQn;
    JoyStick_up.NVIC_IRQChannelPreemptionPriority = 1; // TODO
    JoyStick_up.NVIC_IRQChannelSubPriority = 0; // TODO
    JoyStick_up.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&JoyStick_up);
    // User S1 Button
     Button.NVIC_IRQChannel = EXTI15_10_IRQn;
    Button.NVIC_IRQChannelPreemptionPriority = 2; // TODO
    Button.NVIC_IRQChannelSubPriority = 0; // TODO
    Button.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&Button);
    // UART1
	// 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART1_IRQn);
    UART1.NVIC_IRQChannel = USART1_IRQn;
    UART1.NVIC_IRQChannelPreemptionPriority = 3; // TODO
    UART1.NVIC_IRQChannelSubPriority = 0; // TODO
    UART1.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&UART1);
}

void USART1_IRQHandler() {
	uint16_t word;
    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
    	// the most recent received data by the USART1 peripheral
        word = USART_ReceiveData(USART1);

        // TODO implement, 키보드 입력 'd' 또는 'u'에 따라 동작
        switch(word){
        case 'd':
            if(ua_state == 0) ua_state = 0;
            else ua_state = 0;
            break;
        case 'p':
          if(ua_state ==0) ua_state =1;
          else ua_state = 1;
        default:
          ua_state = 0;
        }
        // clear 'Read data register not empty' flag
    	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}

void EXTI15_10_IRQHandler(void) { // when the button is pressed

	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == Bit_RESET) {
			// TODO implement
            /* send UART 1 */               
                 char msg[] ="Team07\r\n";
                      for ( i=0; i<15; i ++) {
                          SendData(msg[i]);
                      }
                      delay();
		}
        EXTI_ClearITPendingBit(EXTI_Line11);
	}
}


// TODO: Create Joystick interrupt handler functions
/* Use "EXTIx_IRQHandler()" */
void EXTI2_IRQHandler(){
   if(EXTI_GetITStatus(EXTI_Line2) != RESET){
      ua_state = 1;
      EXTI_ClearITPendingBit(EXTI_Line2);
   }
}

void EXTI9_5_IRQHandler(){
   if(EXTI_GetITStatus(EXTI_Line5) != RESET){
      ua_state = 0;
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


  
  // GPIOD_CRL = 0x44444444; // reset
   //GPIOD_CRL = 0x30033300; // led PD 2,3,4,7 output push-pull 


void led_up(void){
  
 
             GPIO_ResetBits(GPIOD, GPIO_Pin_2);
         GPIO_SetBits(GPIOD, GPIO_Pin_7);
         delay();

         GPIO_ResetBits(GPIOD, GPIO_Pin_7);
         GPIO_SetBits(GPIOD, GPIO_Pin_4);
         delay();

         GPIO_ResetBits(GPIOD, GPIO_Pin_4);
         GPIO_SetBits(GPIOD, GPIO_Pin_3);
         delay();

         GPIO_ResetBits(GPIOD, GPIO_Pin_3);
         GPIO_SetBits(GPIOD, GPIO_Pin_2);
         delay();
  
}

void led_down(void){
   GPIO_ResetBits(GPIOD, GPIO_Pin_7);
         GPIO_SetBits(GPIOD, GPIO_Pin_2);
         delay();

         GPIO_ResetBits(GPIOD, GPIO_Pin_2);
         GPIO_SetBits(GPIOD, GPIO_Pin_3);
         delay();

         GPIO_ResetBits(GPIOD, GPIO_Pin_3);
         GPIO_SetBits(GPIOD, GPIO_Pin_4);
         delay();

         GPIO_ResetBits(GPIOD, GPIO_Pin_4);
         GPIO_SetBits(GPIOD, GPIO_Pin_7);
         delay();

}

int main(void)
{

    SystemInit();

    RCC_Configure();

    GPIO_Configure();

    EXTI_Configure();

    USART1_Init();

    NVIC_Configure();

    
    while (1) {
    	// TODO: implement   
      switch(ua_state){
        case 0:
          led_up();
          break;
      case 1:
          led_down();
          break;
      }
    	// Delay
    
    }
    return 0;
}
