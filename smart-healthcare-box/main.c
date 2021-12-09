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
void USART12_Init(void);
void NVIC_Configure(void);
void ADC_Configure(void);
void delay(void);
void turn_rgbled(int led_idx);
void sendStringUsart(USART_TypeDef* USARTx, char* msg);
void buzzerOn(void);
void buzzerOff(void);
void TIM2_Configure(void) ;
void TIM2_IRQHandler();
int readDistance(uint16_t GPIO_PIN_TRIG, uint16_t GPIO_PIN_ECHO);
void delayTime(uint32_t delayTime);
void getDistance();
/* 핀매핑
- 자석: PE0
- RGB LED: PB12,13,14 (R,G,B순서대로 ), 공통단자: GND
- S1버튼: PD11 (내부적으로 연결)
- 부저(Piezo): PB0

[이진] [오후 7:57] TRIG PE2핀  vfㅍㄹ
[이진] [오후 7:57] ECHO PE3핀 ㅊㄹ

*/
uint32_t usTime = 0;
//RGB LED 변수
#define RED 0
#define GREEN 1
#define BLUE 2

int flagBuzzer = 1; //c에서는 boolean type이 없음

unsigned led_array[3] = {
  GPIO_Pin_12,    GPIO_Pin_13,    GPIO_Pin_14 //R,G,B 순서
};

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
  //Piezo
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  //임시 /* JoyStick Up/RIGHT/Down port clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  //타이머 clock 인가 time clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void GPIO_Configure(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  //마그네틱
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;// | GPIO_Mode_IPU;
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
  
  //@임시    /* JoyStick up, down pin setting  UP(5), RIGHT(4) DOWN(2)*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU | GPIO_Mode_IPD;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  // 초음파 센서 6개
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12; // TRIG
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 초음파 발사
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13; // Echo
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // input pull down 초음파 받기
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  
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

void EXTI_Configure(void) // stm32f10x_gpio.h 참고
{
  EXTI_InitTypeDef EXTI_InitStructure;
  // TODO: Select the GPIO pin (Joystick, button) used as EXTI Line using function 'GPIO_EXTILineConfig'
  // TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'
  
  /* s1 Button */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* 자석 E0 */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; /
  /* 이거하면 자석 붙였을때 뗄떼 , 둘다 IT문 안이 무조건 호출됨.. RESET으로 하나 SET으로 하나 마찬가지였음 그건 왜지
  
FALLING으로 하면 붙였을때 뗄데 둘다 '리셋됨'은 호출되는데
IF문 안은 == SET 으로해야지만 ;안'이 호출됨. 이게 정상작동 */
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
      //이거하면 
      
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  // NOTE: do not select the UART GPIO pin used as EXTI Line here
}

void TIM2_IRQHandler() {
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
    //printf("%d",usTime);
    usTime++; // 1us마다 Interrupt가 걸리도록 설정해두었으니 usTime을 측정하는 변수
    
  }
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

void USART12_Init(void){
  // USART 1
  USART_InitTypeDef USART1_InitStructure;
  
  // Enable the USART1 peripheral
  USART_Cmd(USART1, ENABLE);
  USART1_InitStructure.USART_BaudRate = 9600;
  USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART1_InitStructure.USART_StopBits = USART_StopBits_1;
  USART1_InitStructure.USART_Parity = USART_Parity_No;
  USART1_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART1_InitStructure);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  
  // USART 2
  USART_InitTypeDef USART2_InitStructure;
  USART_Cmd(USART2, ENABLE);
  USART2_InitStructure.USART_BaudRate = 9600;
  USART2_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART2_InitStructure.USART_StopBits = USART_StopBits_1;
  USART2_InitStructure.USART_Parity = USART_Parity_No;
  USART2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART2, &USART2_InitStructure);
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
  
  // User S1 Button
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // 자석 BE0
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // TODO
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    // TIMER2
  NVIC_EnableIRQ(TIM2_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; // 우선순위가 가장 높다.
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void TIM2_Configure(void) {
  
    // 짧은 시간 많은 감지가 필요하므로 1us를 만든다고 가정
  TIM_TimeBaseInitTypeDef TIM_InitStructure;
  TIM_InitStructure.TIM_Prescaler = 72;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStructure.TIM_Period = 1;
  TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM2, &TIM_InitStructure);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
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
//휴대폰으로부터 메세지 수신 시 호출
void USART2_IRQHandler() {
  uint16_t word;
  if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){
    // the most recent received data by the USART1 peripheral
    word = USART_ReceiveData(USART2);
    
    if (word == '0') {//약복용 완료됐다는 것, 보드의 부저를 꺼야함  
      turn_rgbled(RED);
      flagBuzzer = 0; //부저끔
    }
    else if (word == '1') //알람 시간 입력됨 
      turn_rgbled(BLUE);
    else if (word == '2') //타이머 지속 시간   입력됨
      turn_rgbled(GREEN);
    
    USART_SendData(USART1, word); //푸티에 출력
    // clear 'Read data register not empty' flag
    USART_ClearITPendingBit(USART2,USART_IT_RXNE);
  }
}
/* sendStringUsart: char 배열 타입의 문자열 출력
인자1) USART1: PUTTY로 출력
USART2: 휴대폰으로 출력
인자2) char 배열 문자열  */
void sendStringUsart(USART_TypeDef* USARTx, char* msg) { 
  char *tmp = &msg[0];
  while (*tmp != '\0') {
    USART_SendData(USARTx, (uint16_t)*tmp);
    /* Wait till TC is set */
    while ((USARTx->SR & USART_SR_TC) == 0);
    tmp++;
  }
}
void EXTI15_10_IRQHandler(void) { 
  if (EXTI_GetITStatus(EXTI_Line11) != RESET) { //// s1버튼이 눌리면 메뉴 출력
    if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == Bit_RESET) {          
      sendStringUsart(USART2, msg_menu); //폰에 메뉴판 출력 
    }
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
}
void EXTI0_IRQHandler(void) {
  if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
    printf("EXTI0_IRQHandler0 리셋됨\n"); //붙였다 뗄떼마다 호출됨
    if (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0) == Bit_RESET) {     //자석 B0가  리셋되면 (=뚜껑열리면)
      printf("EXTI0_IRQHandler1 if문 안\n");
      flagBuzzer = 0; //부저끔
    }
    printf("EXTI0_IRQHandler2 if문 밖\n");
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
  printf("====================\n");
  
}

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
      delay();
      GPIO_ResetBits(GPIOB,GPIO_Pin_0);
      delay();
    }
    else //약통 열면 break
      break;
  }
}
void buzzerOn() { //위의 alert 보고 자석 인터럽트 방식으로 부저 울리도록 수정. 
  printf("부저 플래그: %d\n", flagBuzzer);
  
  while(flagBuzzer) { 
    printf("부저울림\n");
    GPIO_SetBits(GPIOB,GPIO_Pin_0);
    delay();
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);
    delay();
  } 
  //0을 전송하거나 뚜껑 열리면 부저 꺼짐
  printf("부저끔\n"); 
  
  flagBuzzer = 1; // 부저 플래그 다시 복원 
}

void delayTime(uint32_t delayTime){
  uint32_t prev_time = usTime;
  while(1)
  {
    if(usTime - prev_time > delayTime) break;
  }
}

void delay(void) {
  int i;
  for (i = 0; i < 2000000; i++) {}
}
//TIM을 1us로 맞추는 걸로 수정
// E2, E3
int readDistance(uint16_t GPIO_PIN_TRIG, uint16_t GPIO_PIN_ECHO){
    uint32_t prev = 0;
    GPIO_SetBits(GPIOE, GPIO_PIN_TRIG);
    GPIO_ResetBits(GPIOE, GPIO_PIN_ECHO);
    delayTime(10);
    GPIO_ResetBits(GPIOE, GPIO_PIN_TRIG);
    //uint8_t val = GPIO_ReadInputDataBit(GPIOE, GPIO_PIN_ECHO);

    /* 버스트 발생 직후 에코는 HIGH 레벨을 가진다.
    따라서 버스트가 발생했는지 알기 위해 while문을 통해
    에코가 LOW 레벨(RESET)을 가질 때(버스트 발생 X)는 반복문에 머물게 하고 
    에코가 HIGH 레벨(SET)을 가질 때(버스트 발생)는 반복문을 탈출한다.*/  
    while(GPIO_ReadInputDataBit(GPIOE, GPIO_PIN_ECHO) == RESET);
    
    // 반복문을 탈출한 이후엔 시간 측정을 위해 prev 변수에 현재 시각을 저장한다.
    prev = usTime; 
    
    /* 에코에 버스트가 다시 들어오면 에코는 LOW 레벨을 가진다.
    따라서 에코가 HIGH 레벨(SET)일 동안은 아직 버스트가 돌아 오지 않은 거니까
    반복문에 머물게 하고 에코가 LOW 레벨을 가졌을 땐 버스트가 들어왔다는
    의미니까 반복문을 탈출해 거리를 계산한다.*/ 
    while(GPIO_ReadInputDataBit(GPIOE, GPIO_PIN_ECHO) != RESET);

    // 거리는 (버스트 왕복거리) / 2 / 0.034cm/us 로 구해진다.
    int distance = (usTime - prev)*34/1000;

    return distance;

}

void getDistance(){
   uint32_t v = readDistance(GPIO_Pin_2,GPIO_Pin_3);
      if(v>=0 && v <= 4) {
      turn_rgbled(GREEN);
    printf("Sensor: %d\n",v);
      delay();

      }
   else if(v> 4 && v <8){
     turn_rgbled(BLUE); 
  printf("Sensor: %d\n",v);
   delay();
      
     }
      else{
   turn_rgbled(RED); 
       printf("Sensor: %d\n",v);
       delay();
      }
}

int main(void) {
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  USART12_Init();
  EXTI_Configure();
  // ADC_Configure();
  NVIC_Configure();
  Tim_Configure();
  TIM2_Configure();
  
  
//  while (1) {
//    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == Bit_RESET)  {//약먹을시간되면  (임시로 조이스틱up시)
//      //      alert();
//      sendStringUsart(USART2, msg_medicine_time); //폰에 약먹으라고 메세지 전송
//      buzzerOn();     
//    }
//    
//    
//    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_RESET)  //제한시간 초과시  (임시로 조이스틱down시)
//      sendStringUsart(USART2, msg_medicine_fail); //약 복용안했다고 메세지 전송
//  }
  while(1){
     getDistance;

  }
  return 0;
}
