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

&nbsp;&nbsp;폴링은 Hardware의 변화를 지속적으로 읽어들여 변화를 알아채는 방법이다. 신호를 판단하기 위해 지속적으로 확인해야 한다 다른 일을 하는 중에 신호를 읽을 수 없다.
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
</br>&nbsp;&nbsp;NVIC는 Nested vectored interrupt controller의 약자로, 중첩된 인터럽트를 관리하는 역할을 한다. 인터럽트는 동시에 여러 개가 발생할 수 있어서, 처리할 때 우선순위를 정할 필요가 있다. 우선순위가 높은 이벤트는 먼저 처리되고 낮은 이벤트는 나중에 처리된다. 우선순위에는 preemption priority와 sub priority가 있는데, preemption priority에 따라 인터럽트가 우선적으로 처리되며, 우선순위가 동일한 경우에는 sub priority에 따라서 인터럽트를 처리한다. 이러한 우선순위에 따른 인터럽트의 총체적 관리를 NVIC가 담당한다. 그러므로 인터럽트를 활용하기 위해 NVIC와 관련된 레지스터 설정이 필요하다. 만약 하나의 인터럽트 처리 도중에 우선순위가 더 높은 인터럽트가 발생한다면 우선순위를 사용하여 새로 발생한 인터럽트를 우선 수행한 뒤에 마저 수행한다. 값이 작을수록 우선순위가 높다. 

## 3. 실습 설명
### Todo 0
``` C
	int mode = 0; 
	int led_idx = 0; 
	char msg[] = "Team07\r\n";
```
&nbsp;&nbsp;실습에 사용할 전역 변수이다. mode 변수는 LED의 물결을 제어하기 위해서 사용된다. led_idx 변수는 LED의 점등 순서를 모듈러 연산을 통해 제어하는 데 필요한 변수이다. 그리고 우리조가 Putty 앱을 통해 출력해야할 문자열을 msg 변수에 선언하였다.

### Todo 1(RCC_Configure)
``` C
void RCC_Configure(void) {// stm32f10x_rcc.h 참고

	GPIO_InitTypeDef GPIO_LED;
        // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'

        /* UART TX/RX port clock enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

        /* JoyStick Up/Down port clock enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);


        /* LED port clock enable */
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
	GPIO_InitTypeDef GPIO_InitStructure;
```

</br>&nbsp;&nbsp;GPIO_InitTypeDef 구조체를 활용한다. GPIO_Pin, GPIO_Speed, GPIO_Mode 변수를 통해 보드를 제어한다.
</br>
``` C
   /* JoyStick up, down pin setting  UP(5), DOWN(2)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU | GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* button pin setting */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU | GPIO_Mode_IPD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* LED pin setting*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
   
    /* UART pin setting */
    //TX 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // PUSH PULL 이었을 때, 통신이 안됨
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU | GPIO_Mode_IPD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
```
</br>&nbsp;&nbsp;조이스틱은 위, 아래로 움직일 때 2번 핀과 5번 핀을 사용한다. 출력은 최대 스피드인 50MHz, GPIO 모드는 인풋 풀업 상태로 설정하였다.
</br>&nbsp;&nbsp;S1 버튼은 PD11번 핀, LED는 PC2, 3, 4, 7번 핀을 사용하고 모드도 각각 GPIO_Mode_IPU | GPIO_Mode_IPD와 GPIO_Mode_Out_PP로 설정하였다(버튼은 입력이고, LED는 출력이다).
</br>&nbsp;&nbsp;UART TX는 PA9번 핀이다. 실험을 하면서 GPIO_Mode 설정의 중요했다. Push Pull로 실험을 했을 때, Putty와 통신이 잘 되지 않았다. 시행착오 끝에 GPIO_Mode를 GPIO_Mode_AF_PP로 설정하였다(Alternate function이므로, GPIO_Mode_AF_PP). UART RX는 PA10번이고 모드는 GPIO_Mode_IPU | GPIO_Mode_IPD로 설정했다.

### Todo 3(EXTI_Configure)
``` C
    EXTI_InitTypeDef EXTI_InitStructure;
     /* Joystick Down */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
```
&nbsp;&nbsp;예를 들어 조이스틱 down의 경우에는 C포트를 사용하고 2번 핀을 사용하므로 GPIO_EXTILineConfig함수의 매개변수로 (GPIO_PortSourceGPIOC, GPIO_PinSource2)를 주었다.. 2번 핀을 사용하므로 EXIT_Line2를 EXTI_Line값으로 주었고, 조이스틱을 조종할 때 인터럽트가 발생하도록 구성을 하였다. 조이스틱의 경우 입력이 없으면 1이고 들어오면 0이 되므로 trigger유형은 falling에 해당한다. 마지막으로 EXTI_Init함수로 초기화하였다.

``` C
    /* Joystick Up */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);
    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Button */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
```    
</br>&nbsp;&nbsp;조이스틱 up과 S1 버튼도 같은 방식으로 설정을 하면 된다. 조이스틱 up은 PC5q번, S1 버튼은 PD11이므로 각각 EXTI라인을 5번과 11번에 설정하면 된다. 
.
### Todo 4(USART1_Init)
&nbsp;&nbsp;USART1에 대해서는 초기 파일에 다음과 같은 정보가 있었다.
```
       BaudRate: 9600
       WordLength: 8bits
       Parity: None
       StopBits: 1bit
       Hardware Flow Control: None
```
&nbsp;&nbsp;BaudRate는 9600, 워드길이는 8비트이고 tx,rx를 모두 이용할 것이기 때문에 mode설정, 플로우 컨트롤은 disable(none)이다. 패리티를 이용하지 않기 때문에 Parity는 None로 설정하고, 스톱비트는 1로 주었다. 위를 토대로 다음과 같이 코드를 작성하였다.
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
	/* TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and 
	the argument value 'Receive Data register not empty interrupt */
	USART_ITConfig(USART1 , USART_IT_RXNE , ENABLE);
```

### Todo 5(NVIC_Configure)
``` C
	NVIC_InitTypeDef NVIC_InitStructure;
	// TODO: fill the arg you want
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
```
&nbsp;&nbsp;조이스틱 down, up, S1 Button, UART1의 인트럽트를 관리하기 위해 NVIC_InitTypeDef 구조체를 선언하였다. 
``` C
	NVIC_Init(&NVIC_InitStructure);
        // Joystick Down
        NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // TODO
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        // Joystick Up
        NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // TODO
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        // User S1 Button
        NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        // UART1
        // 'NVIC_EnableIRQ' is only required for USART setting
        NVIC_EnableIRQ(USART1_IRQn);
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
```
&nbsp;&nbsp;NVIC_IRQChannel을 설정하여 어떤 exit line의 인터럽트를 확인할지를 정할 수 있다. preemptionPriority와 subPriority를 정하는데, 조이스틱 간에는 동시에 입력이 이루어 지지 않기 때문에 서로 인터럽트가 겹칠 우려가 없다. 그래서 우선순위를 동일하게 작성하였다. UART 통신 관련 코드에 대해서는 최고 우선순위인 0을 설정했다.

### Todo 6(USART1_IRQHandler())
``` C
	uint16_t word;
        if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
        // the most recent received data by the USART1 peripheral
        	word = USART_ReceiveData(USART1);
        	// TODO implement, 키보드 입력 'd' 또는 'u'에 따라 동작
        	if (word == 'u') {
                // A 동작
                	mode = 0;
        	} else if (word == 'd') {
                // B 동작
                	mode = 1;
        	}
        // clear 'Read data register not empty' flag
        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
```
&nbsp;&nbsp;mode 변수는 전역변수로 0으로 초기화되어있다. mode = 0이면 물결 방향이 1->2->3->4 위로 가도록(미션지의 A동작), mode = 1이면 물결 방향이 4->3->2->1 아래로 가도록(미션지의 B동작)을 하도록 한다. 키보드로 u를 누르면 A동작을 하고, d를 누르면 B동작을 하도록 mode에 대입되는 값을 달리 설정하였다.


### Todo 7(EXTI15_10_IRQHandler 등)
&nbsp;&nbsp;우리는 EXTI2_IRQn, EXTI9_5_IRQn, EXTI15_10_IRQn을 선언했고, 이들을 처리할 핸들러 함수는 EXTI15_10_IRQHandler만 초기 파일에 정의되어 있다. 우선 EXTI15_10_IRQHandler함수를 먼저 살펴보자. Todo 6은 키보드 입력으로 LED 순서를 다루었다면, Todo 7에서는 조이스틱으로 LED를 조종할 수 있도록 하는 것이다.
``` C
	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
        	if (GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11) == Bit_RESET) {
                	mode = 1;
                	// TODO implement
                 	/* send UART 1 */
                        char *tmp = &msg[0];
                	while (*tmp != '\0') {
                		sendDataUART1(*tmp);
                		tmp++;
              		}
      		}
        EXTI_ClearITPendingBit(EXTI_Line11);
	}
```
&nbsp;&nbsp;S1 버튼을 눌러서 Putty에 "Team07\r\n"을 출력하게 하는 것은 지난 실험 때 사용한 SendData함수를 사용한다. S1 버튼은 PD11이고 EXTI_Line11과 관련이 있다. 버튼 입력이 있으면 Putty와 연동해서 데이터 통신을 가능하게 한다. 그리고 마지막에는 EXTI_ClearITPendingBit를 한다.

``` C
void EXTI2_IRQHandler(void) {
  	// up
  	if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
      		if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_RESET) {
               		mode = 0;
   		}
       		EXTI_ClearITPendingBit(EXTI_Line2);
   		}
	}
```
&nbsp;&nbsp;조이스틱 down(PC2)은 LED 순서를 아래로 가게 해야한다. main함수에서 while문이 돌아갈 때, 인터럽트 핸들러에 의해서 mode 값을 지정할 수 있다. mode가 1이면 LED가 아래로 갈 수 있다. 그렇게 정해주고, 마지막에 EXTI_ClearITPendingBit를 하자.
``` C
	void EXTI9_5_IRQHandler(void) {
  	// down
  		if (EXTI_GetITStatus(EXTI_Line5) != RESET) {
      			if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == Bit_RESET) {
                  		mode = 1;
      			}
        		EXTI_ClearITPendingBit(EXTI_Line5);
   		}
	}
```
&nbsp;&nbsp;조이스틱 up(PC5)은 LED 순서를 위로 가게 해야한다. mode = 0으로 해서 LED 순서를 조정할 수 있도록 한다. </br>마지막에 EXTI_ClearITPendingBit를 하자.


### Todo 8(main 함수)
``` C
	unsigned led_array[4] = {
        	GPIO_Pin_2,
      		GPIO_Pin_3,
      		GPIO_Pin_4,
      		GPIO_Pin_7,
    	};
```
&nbsp;&nbsp;위 배열은 각각 LED의 PD2, 3, 5, 7번 핀과 대응한다.

``` C
	while (1) {
        // TODO: implement 
        	int index = led_idx % 4;
        	for (int i = 0; i < 4; ++i) {
          		if (i == index) {
                		GPIO_SetBits(GPIOD, led_array[i]);
          		} else {
                		GPIO_ResetBits(GPIOD, led_array[i]);
                	}
        	}
        
       		// Delay
       		Delay();
       		if (mode == 0) {
       			led_idx++;
       		} else {
        		led_idx--;
      		}
    	}
``` 
&nbsp;&nbsp;led_idx변수를 4 모듈러 연산을 해서 결정된 index값을 통해 점등할 LED와 소등할 LED를 정한다. 그리고 모드 상태에 따라 led_idx를 증가시키거나 감소시킴으로써 LED 점등 순서를 위로 하거나 아래로 할 수 있다.

## 5. 실험 결론
&nbsp;&nbsp;이번 실험에서 지난 실험 미션에서 다루었던 putty 통신과 LED 점등, 조이스틱, 버튼 조작을 모두 활용하였다. 지금까지 이론으로서만 다루었던 인터럽트를 실제로 구현해봄으로써 임베디드 시스템에 더욱 이해할 수 있게 되었다. 인터럽트를 발생시키고, 이를 EXTI에 전달하여 NVIC를 통해 우선순위를 활용하고, 최종적으로 핸들러 함수를 통해 인터럽트를 처리하는 순서를 알게되었다.
이번 미션의 핵심은 인터럽트였기에 앞으로도 인터럽트를 잘 처리할 수 있도록 능력을 배양해야겠다.

