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
</br>&nbsp;nbsp;인터럽트란 코드상에서 인터럽트 핸들러를 실행시키는 것으로 CPU가 관여를 하는 것을 말한다. 이벤트란 주변 하드웨어 장치에서 직접 신호가 전송되어 CPU가 관여하지 않고 처리되는 것을 말한다. 핸들러 코드를 사용하지 않는다.


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
### Todo 1

```
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
```






