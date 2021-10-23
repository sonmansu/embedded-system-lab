# 임베디드 시스템 설계 및 실험 7주차 결과 보고서
7조 이석우, 손수민, 김춘수, 정지용
 
## 1. 실험 목표
1. Interrupt 방식을 활용한 GPIO 제어 및 UART 통신
2. 라이브러리 함수 사용법 숙지

## 2.실험 이론
### 인터럽트

&nbsp;&nbsp;인터럽트는 하드웨어의 변화를 감지해 외부로부터 전기신호 입력을 CPU가 알아채는 방법이다. 인터럽트가 발생할 때마다 컨트롤러는 현재 명령의 실행을 완료(또는 잠시 멈추고)하고 인터럽트 서비스 루틴(ISR) 또는 인터럽트 핸들러의 실행을 시작한다. ISR은 프로세서나 컨트롤러에 인터럽트가 발생할 때 무엇을 해야 하는지 알려준다.
</br>&nbsp;&nbsp; 인터럽트는 하드웨어 인터럽트 또는 소프트웨어 인터럽트로 나뉜다. 하드웨어 인터럽트는 하드웨어 인터럽트는 디스크 컨트롤러나 외부 주변 장치와 같은 외부 장치에서 프로세서로 보내는 전자적인 경고 신호이다. 예를 들어, 키보드의 키를 누르거나 마우스를 움직이면 프로세서가 키 입력이나 마우스 위치를 읽도록 하는 경우에 하드웨어 인터럽트를 발생시킨다. 하드웨어 인터럽트를 통상 인터럽트라고 부른다.
</br>&nbsp;&nbsp;소프트웨어 인터럽트는 외부가 아닌 CPU 내부에서 자신이 실행한 명령이나 CPU의 명령 실행에 관련된 모듈이 변화하는 경우 발생한다. 프로그램 실행 중 프로그램 상의 처리 불가능한 오류나 이벤트를 알리기 위한 경우 발생하는데, 이를 트랩(trap) 또는 예외(exception)라 부른다. 예를 들어, 프로세서의 산술 논리 장치가 숫자를 0으로 나누는 명령을 실행하거나, 존재하지 않는 메모리 주소에 접근할 때 소프트웨어 인터럽트가 발생한다.


### 폴링

&nbsp;&nbsp;폴링은 Hardware의 변화를 지속적으로 읽어들여 변화를 알아채는 방법이다. 신호를 판단하기 위해 지속적으로 확인해야 한다 다른 일을 하는 중에 신호를 읽을 수 없다
</br>
![image](https://user-images.githubusercontent.com/62247273/138541484-a9d735b1-5da2-49cc-9eda-499b48413f31.png)
![image](https://user-images.githubusercontent.com/62247273/138541488-7918b1c1-1e6d-4e5a-aa65-14b9ce2f0f71.png)
</br>&nbsp;&nbsp;왼쪽이 폴링 방식이고 오른쪽은 인터럽트 방식의 순서도를 나타낸 것이다.


### EXTI(Extended Interrupt and Event Controller)
 
&nbsp;&nbsp;외부 인터럽트/이벤트 컨트롤러는 인터럽트/이벤트 요청을 생성하는 데 사용되는 20개의 에지 감지기 라인으로 구성된다. 각 라인은 트리거 이벤트(상승 에지, 하강 에지, 둘 다)를 선택하도록 독립적으로 구성될 수 있으며 독립적으로 마스킹(masking)될 수 있다. 
</br>&nbsp;&nbsp;보류 중인 레지스터는 인터럽트 요청의 상태를 유지한다. EXTI는 내부 APB2 클록 주기보다 짧은 펄스 폭을 가진 외부 라인을 감지할 수 있다. 최대 80개의 GPIO를 16개의 외부 인터럽트 라인에 연결할 수 있다.
</br>&nbsp;&nbsp;인터럽트란 코드상에서 인터럽트 핸들러를 실행시키는 것으로 CPU가 관여를 하는 것을 말한다. 이벤트란 주변 하드웨어 장치에서 직접 신호가 전송되어 CPU가 관여하지 않고 처리되는 것을 말한다. 핸들러 코드를 사용하지 않는다.


![image](https://user-images.githubusercontent.com/62247273/138541884-a2ce3cd6-c057-4446-bb3a-e40b80419773.png)
</br>
&nbsp;&nbsp;위 그림과 같이 총 16개의 EXTI 라인이 있으며 7개의 IRQ(인터럽트 요구)로 처리된다. 동일한 EXTI에 연결된 핀들은 동시에 인터럽트 소스로 사용될 수 없다. 예를 들어 PA0와 PB0를 인터럽트 소스로 사용할 수 없다. 5번에서 9번 그리고 10번에서 15번 EXTI는 EXTI9_5_IRQ 와 EXTI15_10_IRQ를 함께 사용하기 때문에 인터럽트 핸들러 내부에서 어떤 인터럽트 소스에서 발생하였는지 구분할 필요가 있다. 

### 인터럽트 벡터 테이블

![image](https://user-images.githubusercontent.com/62247273/138541810-65c59f8d-9b6a-4e6f-8e54-f58094041396.png)

</br>&nbsp;&nbsp;여러 가지 인터럽트에 대해 해당 인터럽트 발생시 처리해야 할 루틴의 주소를 보관하고 있는 테이블을 의미한다. 대부분의 CPU는 인터럽트 벡터 테이블을 가지고 있다.

### NVIC(Nested Vectored Interrupt Controller)

![image](https://user-images.githubusercontent.com/62247273/138541833-11a2323b-5fdd-42f4-b413-cdf057f51934.png)
</br>&nbsp;&nbsp;NVIC는 여러 인터럽트를 관리하는 레지스터이다. IPR레지스터는 총 5가지 PRIORITY GROUP 으로 구성될 수 있으며 구성에 따라 Preemption Priority 와 Sub Priority 의 개수가 변경된다. Preemption Priority 는 ISR 간 선점 동작에 적용된다. Sub Priority 는 Pending 된 ISR 이 수행되는 우선순위만을 결정하며 선점 동작에는 영향을 미치지 않는다. 

## 3.실험 과정
### Todo 1(RCC_Configure)

``` C
void RCC_Configure(void) // stm32f10x_rcc.h 참고
{
	// TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'	
	/* UART TX/RX port clock enable */
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	/* JoyStick Up/Down port clock enable */
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/* LED port clock enable */         t
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	/* USART1 clock enable */
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	/* Alternate Function IO clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}
```

&nbsp;&nbsp;UART를 사용하기 위해서는 Port A, 조이스틱을 사용하기 위해서는 Port C, LED를 사용하기 위해서는 Port D가 필요하다. 그래서 Port A와 Port C, Port D에 클럭 인가를 하였다. USART1과 Alterbate Function IO를 사용하기 위해서 각각 클럭을 인가하였다.

### Todo 2(GPIO_Configure)

``` C
    GPIO_InitTypeDef JoyStick;
    GPIO_InitTypeDef LED;
    GPIO_InitTypeDef USART;
    GPIO_InitTypeDef Button;
    GPIO_InitTypeDef GPIO_InitStructureRX;
    GPIO_InitTypeDef GPIO_InitStructureTX;
```

</br>&nbsp;&nbsp;GPIO_InitTypeDef 구조체를 활용한다. 조이스틱과 LED, USART, Button, UART RX, UART TX를 설정(configure)하기 위해 구조체를 따로따로 선언한다.
</br>
``` C
    /* JoyStick up, down pin setting */
    JoyStick.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5;
    JoyStick.GPIO_Speed = GPIO_Speed_50MHz;
    JoyStick.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &JoyStick);
```
</br>&nbsp;&nbsp;예를 들어서 조이스틱은 위, 아래로 움직일 때 2번 핀과 5번 핀을 사용한다. 출력은 최대 스피드인 50MHz, GPIO 모드는 인풋 풀업 상태로 설정하였다.
</br>&nbsp;&nbsp;LED 등도 이와 비슷한 방식으로 설정하였다.

### Todo 3(EXTI_Configure)
``` C
    EXTI_InitTypeDef JoyStick_down;
    EXTI_InitTypeDef JoyStick_up;
    EXIT_InitTypeDef Button;
```
&nbsp;&nbsp;EXIT_InitTypeDef 구조체를 사용하는데, 조이스틱 down, up과 S1버튼을 사용하므로 총 3개를 선언한다.
</br>
``` C
     //Joystick_Down
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
    JoyStick_down.EXTI_Line = EXTI_Line2;
    JoyStick_down.EXTI_Mode = EXTI_Mode_Interrupt;
    JoyStick_down.EXTI_Trigger = EXTI_Trigger_Falling;
    JoyStick_down.EXTI_LineCmd = ENABLE;
    EXTI_Init(&JoyStick_down);
```
&nbsp;&nbsp;예를 들어 조이스틱 down의 경우에는 C포트를 사용하고 2번 핀을 사용하므로 GPIO_EXTILineConfig함수로 설정을 하였다. 2번 핀을 사용하므로 EXIT_Line2를 EXTI_Line값으로 주었고, 조이스틱을 조종할 때 인터럽트가 발생하도록 구성을 하였다. EXTI_Init함수로 초기화하였다.

</br>&nbsp;&nbsp;조이스틱 up과 버튼도 비슷한 방식으로 설정하였다.

### Todo 4(USART1_Init)
&nbsp;&nbsp;USART1에 대해서는 초기 파일에 다음과 같은 정보가 있었다.
```
       BaudRate: 9600
       WordLength: 8bits
       Parity: None
       StopBits: 1bit
       Hardware Flow Control: None
```
&nbsp;&nbsp;위를 토대로 다음과 같이 코드를 작성하였다.
``` C
	USART_InitTypeDef USART1_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
```
&nbsp;&nbsp;클럭 인가를 아래처럼 준다.
``` C
	// Enable the USART1 peripheral
	USART_Cmd(USART1, ENABLE);
	// TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt
	USART_ITConfig(USART1 , USART_IT_RXNE , ENABLE);
```

### Todo 5(NVIC_Configure)
``` C
    NVIC_InitTypeDef JoyStick_down;
    NVIC_InitTypeDef JoyStick_up;
    NVIC_InitTypeDef Button;
    NVIC_InitTypeDef UART1;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
```
&nbsp;&nbsp;조이스틱 down, up, S1 Button, UART1의 인트럽트를 관리하기 위해 NVIC_InitTypeDef 구조체를 4개를 선언하였다. 그리고 PriorityGroup은 PreemptionPriority를 4개 사용하므로 4개를 담당할 수 있는 그룹2로 하였다.

``` C
    // Joystick Down
    JoyStick_down.NVIC_IRQChannel = EXTI2_IRQn;
    JoyStick_down.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    JoyStick_down.NVIC_IRQChannelSubPriority = 0; // TODO
    JoyStick_down.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&JoyStick_down);
```
&nbsp;&nbsp;조이스틱 down은 PC2번이므로 EXTI2_IRQn으로 하였고 PreemptionPriority는 0, SubPriority도 0으로 하였다. 조이스틱 UP은 위의 코드와 비슷하다. 단, PC5번을 쓰므로, EXTI9_5_IRQn을 주었고 PreemptionPrioirty는 1로 주었다.　S1 버튼은 PD11이므로 EXTI15_10_IRQn을 주고 PreemptionPrioirty는 2로 주었다.
</br>&nbsp;&nbsp;단 UART의 경우에는 다음의 코드가 더 필요하다.
``` C
	NVIC_EnableIRQ(USART1_IRQn);
```

### Todo 6(USART1_IRQHandler())
``` C
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
       	      	 	else ua_state = 0;
      		default:
          		ua_state = 0;
        }
        // clear 'Read data register not empty' flag
    	USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
```
&nbsp;&nbsp;int ua_state변수는 전역변수로 0으로 초기화되어있다. 이것이 전역변수인 이유는 ua_state변수가 메인 함수에서 동작 제어를 위해 필요하기 때문이다.
</br>&nbsp;&nbsp;d를 누르면 LED 물결 방향이 위로 가게 해야한다. ua_state가 0이면 물결 방향이 위로 가고, 1이면 물결 방향이 아래로 간다고 하자. 처음에는 물결 방향이 위로 가므로 ua_state = 0인데, d를 누르면 계속 위로 가도록 변화를 주지 않되, 만약 u를 눌러서 ua_state = 1이면 0으로 바꿔주고 break한다.
</br>&nbsp;&nbsp;p를 누르면 물결 방향이 아래로 가야 한다. ua_state = 0이면 1로 바꿔서 방향이 변하게 하고, 1이면 계속 유지하게 한다.
</br>&nbsp;&nbsp;그리고 꼭 마지막에는 입력된 데이터를 지우도록 한다!

### Todo 7(EXTI15_10_IRQHandler 등)
&nbsp;&nbsp;우리는 EXTI2_IRQn, EXTI9_5_IRQn, EXTI15_10_IRQn을 선언했고, 이들을 처리할 핸들러 함수는 EXTI15_10_IRQHandler만 초기 파일에 정의되어 있다. 우선 EXTI15_10_IRQHandler함수를 먼저 살펴보자. Todo 6은 키보드 입력으로 LED 순서를 다루었다면, Todo 7에서는 조이스틱으로 LED를 조종할 수 있도록 하는 것이다.
``` C
	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == Bit_RESET) {
			// TODO implement
            		/* send UART 1 */               
                	char msg[] ="Team07\r\n";
                        	for ( i=0; i<15; i ++) {
                         		SendData(msg[i]);
                      		}
                      	Delay();
		}
        EXTI_ClearITPendingBit(EXTI_Line11);
	}
```
&nbsp;&nbsp;S1 버튼을 눌러서 Putty에 "Team07\r\n"을 출력하게 하는 것은 지난 실험 때 사용한 SendData함수를 사용한다. S1 버튼은 PD11이고 EXTI_Line11과 관련이 있다. 버튼 입력이 있으면 Putty와 연동해서 데이터 통신을 가능하게 한다. 그리고 마지막에는 EXTI_ClearITPendingBit를 한다.

``` C
void EXTI2_IRQHandler(){
   if(EXTI_GetITStatus(EXTI_Line2) != RESET){
      ua_state = 1;
      EXTI_ClearITPendingBit(EXTI_Line2);
   }
}
```
&nbsp;&nbsp;조이스틱 down(PC2)은 LED 순서를 아래로 가게 해야한다. main함수에서 while문이 돌아갈 때, 인터럽트 핸들러에 의해서 ua_state 값을 지정할 수 있다. ua_state가 1이면 LED가 아래로 갈 수 있다. 그렇게 정해주고, 마지막에 EXTI_ClearITPendingBit를 하자.
``` C
void EXTI9_5_IRQHandler(){
   if(EXTI_GetITStatus(EXTI_Line5) != RESET){
      ua_state = 0;
      EXTI_ClearITPendingBit(EXTI_Line5);
   }
}
```
&nbsp;&nbsp;조이스틱 up(PC5)은 LED 순서를 위로 가게 해야한다. ua_state = 0으로 해서 LED 순서를 조정할 수 있도록 한다. 마지막에 EXTI_ClearITPendingBit를 하자.

### Todo 8(LED 점등)
&nbsp;&nbsp;LED 순서는 2, 3, 4, 7번 순서이다. 이에 맞게 LED를 위로 진행시키고, 아래로 진행시킬 수 있다. LED 점등도 구조체 함수를 이용해서 구현할 수 있다. 여기에서는 위로 LED가 진행하는 함수만 제시하도록 한다.
``` C
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
```

### Todo 9(main 함수)
&nbsp;&nbsp;메인 함수에서 우리가 손을 댈 곳은 while문이다. 기기가 작동하면 기본적으로 LED 물결이 윗 방향으로 움직인다. 여기에 우리가 인터럽트를 줄 때, ua_state값을 건드림으로써 LED순서를 조종하려고 한다.
``` C
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
``` 
&nbsp;&nbsp;led_up, led_down 함수 안에 Delay함수가 있어서 따로 넣지는 않았다. 초기 전역 변수 ua_state가 0이므로 기본적으로 case 0이 적용되어 led_up이 된다. 하지만 인터럽트를 이용해서 ua_state값을 1로 만들면 led_down함수가 적용된다.
&nbsp;&nbsp;이와 별개로 S1 버튼을 누르면 EXTI15_10_IRQHandler함수가 실행되어 Putty에 데이터를 전송할 수 있다.

## 4. 실험 결론




