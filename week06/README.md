# 임베디드 시스템 설계 및 실험 6주차 결과 보고서
7조 이석우, 손수민, 김춘수, 정지용
 
## 1. 실험 목표
1. 라이브러리를 활용하여 코드 작성
2. Clock Tree 의 이해 및 사용자 Clock 설정
3. 오실로스코프를 이용한 clock 확인
4. UART 통신의 원리를 배우고 실제 설정 방법 파악

## 2. 실험 이론 
### UART와 USART

&nbsp;&nbsp;UART는 Universal Asynchronous Receiver/Transmitter의 약자로, 마이크로 컨트롤러에서 사용되어 온 통신 방식 중의 하나이다. 속도가 상대적으로 느리고 통신거리가 짧지만, 근거리에서 소량의 데이터를 보낼 때 유용하다. UART는 비동기 통신 방식을 채택하여, 데이터의 원활한 송수신을 위해서 clock을 대체할 수단이 필요하다. Start bit, stop bit, baud rate로 이를 대체한다. USART는 Universal Synchronous and  Asynchronous serial Receiver/Transmitter의 약자로, UART이면서 때로는 clock에 따라 데이터를 보내거나 받을 수 있는 차이점이 있다.
  
### Start bit/Stop bit

&nbsp;&nbsp;UART는 일반적으로 데이터를 전송하지 않을 때 데이터 전송 라인이 고전압 상태를 유지한다. 수신 UART가 고전압에서 저전압으로의 변화를 감지했을 때, baud rate의 주파수에서 데이터 프레임의 비트를 읽기 시작하는데 이 때 쓰이는 것이 start bit이다. stop bit는 전송되는 데이터 패킷의 끝을 신호하기 위해 필요하다.

### Baud rate
&nbsp;&nbsp;Baud rate는 1초당 데이터가 얼마만큼 변조되는지를 비율로 나타낸 것을 말한다. 비동기식 통신의 경우, 송신부와 수신부는 같은 속도로 데이터를 보내거나 받아야 하는데, 정확한 데이터 송/수신을 위해서 통신속도를 사용자가 지정하는 것을 baud rate 세팅이라고 한다. 사용자는 공식을 이용하여 baud rate를 계산하여 세팅할 수 있다.
![화면 캡처 2021-10-11 003334](https://user-images.githubusercontent.com/64721658/136702777-3943caf5-fba5-40ea-a442-38783cf7c419.png)

### HSE/HIS CLOCK
&nbsp;&nbsp;CLOCK은 주기적인 전기적 펄스로, 정해진 CLOCK에 따라 모든 동작이 발생한다. HSE, HIS 두가지 CLOCK이 발생하는데, HSE의 경우 보드 외부에서 발생하는 CLOCK으로 실험에서 사용하는 보드는 25MHZ로 설정되어 있다. HIS의 경우 보드 내부 CLOCK으로 기본 8MHZ이다. 시스템 내부의
CLOCK은 HSI, HSE, PLL중 하나이다. PLL은 위상 동기 회로이며, 입력 신호와 출력신호를 이용해 출력신호를 제어하는 시스템을 말한다.PLL은 HIS와 HSE CLOCK을 곱하거나 나누는 연산으로 원하는 주파수 값을 생성할 수 있다. 이번 실험에서 맡아야 할 첫 번째 파트이다.

### Clock Tree
&nbsp;&nbsp;기기의 clock을 어떻게 조정하고 다루는 지를 알기 위해서는 시스템이 제공하는 clock tree를 참고하여 확인할 수 있다. Stm32는 기본적으로 내장 clock이 있다. 필요에 따라 외부의 clock을 사용할 수 있도록 제작되었다.OSC_IN이라는 외부 clock 입력부에 외부 clock을 연결하고 clock tree를 설정해 주면 외부 clock을 이용할 수 있다. MCO는 microcontroller clock output의 약자로, 컨트롤러의 특정 clock을 곧바로 출력해주어 사용자가 clock을 확인할 수 있도록 한다. mco출력 부 바로 앞에 mux가 있어 어떤 clock을 출력을 할 것인지를 mco비트를 통하여 선택할 수 있다. Clock의 주파수를 사용자가 조정하기 위해서는 곱하기와 나누기 연산을 이용하여야 한다.

![AS2737](https://user-images.githubusercontent.com/64721658/136702979-331eb405-07ee-4cba-b495-50025ba84d4c.png)

곱하기, 나누기 연산을 진행할 수 있는 부분은 prediv2, pll2mul, pll3mul,pllmul이 있다. 제일 먼저 클럭은 OSC_IN으로 들어온다. 이를
PREDIV1SCR MUX에서 HSE 클럭을 이용할 지, OSC CLOCK을 사용할 지 결정할 수 있다. OSC클럭을 사용하기로 결정하면, PREDIV1SCR MUX에
서 CLOCK을 내보내기 전에 PREDIV2, PRE2MUL에서 클럭값을 배분할 수 있다. 배분된 클럭은 PREDIV1SCR로 그리고 PLLSCR MUX에서 다시 HIS
와 해당 주파수 중 하나를 선택하게 된다. 이렇게 나온 최종값이 PLLCLK 가 된다. 그 후 PLLMUL에서 CLOCK배분을 하고, 최종적으로 SW MUX에
서 HSE,HSI,PLLCLK 중 하나를 선택하게 된다. 이 값이 SYSCLK 시스템 클럭이 된다. MCO MUX 에서 SYSCLK를 선택하고, MCO를 PROBE하여 오
실로스코프로 확인하면 시스템 클럭을 관찰할 수 있다.

### RS-232C
&nbsp;&nbsp;이번 실험에서 추가적으로 사용할 보드 포트인 RS-232C 포트는 양방향 통신이 가능하고, TX,RX쌍이 있다. 즉, 한방향으로 송신, 수신이 모두 가능하다. 직렬 통신이라고도 하며, 신호 저항성이 매우 높다. 비동기 통신이므로 통신속도가 기기간에 다르게 설정되어 있으면 통신이 불가능하다.그래서 이번 실험에서는 BAUD RATE를 조절하여 통신속도를 일치시키는 방법을 배운다.

## 3. 실험 과정

### Putty설정
실험결과를 확인하기 위해서는 컴퓨터에 putty가 설치되어 있어야 하며, PC 장치 관리자에서 보드와 연결된 Serial Port 확인하고 baud rate는 
이번 실험에서 28800으로 사용한다.

### 사용한 레지스터 설명

#### control register 1(USART_CR1)

&nbsp;&nbsp;control register 1에서 M은 word length이고, PCE, PS는 오류 검출을 위한 parity bit를 enable, select 하는 역할을 한다. 이번 실험에서는 parity control disabled이므로 사용되지 않았다. TE, RE는 각각 transmitter enable, receiver enable이며, 0은 disable, 1은 enable을 가리킨다.

#### control register 2(USART_CR2)

&nbsp;&nbsp;Control register 2의 clken는 clock enable이고, cpol은 clock polarity, cpha는 clock phase이다. Cr2는 stop bit를 초기화하며, clock사용 설정에 이용된다.

#### control register 3(USART_CR3)

&nbsp;&nbsp;Control register 3의 ctse는 cts enable을 뜻하며, RTSE는 RTS enable을 뜻한다. Cr1, cr2로 clock 세팅이 완료되면 초당 전송되는 baud rate를 조절할 때 이용되는 레지스터이다.

#### Baud rate register(USART_BRR)

&nbsp;&nbsp;Baud rate와 PCLK2 값을 이용하여 계산된 USARTDIV을 정수, 실수 부분으로 나눈 다음, DIV_Mantissa, DIV_Fraction에 각각 16진수로
변환하여 적용한 값을 전송하는데 필요한 레지스터로, data register인 ustar_dr을 적용하여 msb 12bit 에 정수부, lsb 12bit에 소수부의 값을 적용하여 전송하는 역할을 한다.

## 4. 코드 설명

### TODO-1 : SYSCLK를 52MHZ로 설정
<img src="https://s3.us-west-2.amazonaws.com/secure.notion-static.com/13396cc3-caf5-484c-a5d0-9853375a3dc4/Untitled.png?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAT73L2G45O3KS52Y5%2F20211011%2Fus-west-2%2Fs3%2Faws4_request&X-Amz-Date=20211011T132016Z&X-Amz-Expires=86400&X-Amz-Signature=0fc51d109a04630cff2b3b2b304c0f3988d944840865af74344a76fbb7d0dfda&X-Amz-SignedHeaders=host&response-content-disposition=filename%20%3D%22Untitled.png%22" width="300">
&nbsp;&nbsp;HSE OSC에서 생성한 25MHz 클럭을 52MHz로 바꾸기 위해선 아래와 같은 과정을 거치면 된다.   

```
1. PREDIV2에서 /5 (RCC_CFGR2 레지스터)
2. PLL2MUL에서 *13 (RCC_CFGR2 레지스터)
3. PREDIV1SRC 에서 PPL2 클락 선택 (RCC_CFGR2레지스터)
4. PREDIV1에서 /5 (RCC_CFGR2 레지스터) 
5. PLLSRC에서 PREDIV1으로 부터 나온 클락 선택 (RCC_CFGR)
6. PLLMUL에서 *4 (RCC_CFGR 레지스터)
7. SW에서 PLL 선택 (RCC_CFGR)
```

레퍼런스에 따르면 PREDIV2, PLL2MUL, PREDIV1SRC, PREDIV1은 RCC_CFGR2 레지스터를 통하여 설정하고 PLLSRC, PLLMUL은 RCC_CFGR 레지스터를 통해 설정한다. 요구 조건에 따라 수정한 코드는 다음과 같다.
``` C
/* Configure PLLs ------------------------------------------------------*/
RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL); // 사용을 위한 초기화
RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLSRC_PREDIV1 | RCC_CFGR_PLLMULL4); // *4 

RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL | RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC); // 사용을 위한 초기화
RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL13 | RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5); // /5, *13, /5 
//@End of TODO - 1
```

### TODO-2 : MCO 포트를 시스템 클럭 출력으로 설정
``` C
//@TODO - 2 Set the MCO port for system clock output
RCC->CFGR &= ~(uint32_t)RCC_CFGR_MCO;
RCC->CFGR |= (uint32_t)RCC_CFGR_MCO_SYSCLK;
//@End of TODO - 2
```
&nbsp;&nbsp;RCC_CFGR 레지스터의 MCO 에 0100 값을 주어 시스템 클락(SYSCLK)을 선택한다. 선택된 시스템 클럭은 오실로스코프로 확인이 가능하다. 

  
### TODO-3 : RCC 설정

```C
/* GPIO RCC Enable  */
/* UART Tx, Rx, MCO port */
RCC->APB2ENR |= (uint32_t)((RCC_APB2ENR_IOPAEN) | (RCC_APB2ENR_AFIOEN));
```
&nbsp;&nbsp;MCO는 PA8, USART1_TX는 PA9, USART1_RX는 PA10를 사용하고 MCO와 UART Tx는 Alternative function으로 사용할 것이기 때문에 IOPAEN과 AFIOEN을 set해준다.

```C
/* USART RCC Enable */
RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
```
그리고 USART1을 이용하여 통신하기 때문에 USART1EN을 set 한다.   

```C
/* User S1 Button RCC Enable */
RCC->APB2ENR |= (uint32_t)(RCC_APB2ENR_IOPDEN);
```
마지막으로 S1 Button은 PD11을 사용하기 때문에 IOPDEN을 set해준다.

### TODO-4 : USART SETTING

  ``` C
  void PortConfiguration(void) {
  //@TODO - 4 GPIO Configuration
  /* Reset(Clear) Port A CRH - MCO, USART1 TX,RX*/
  GPIOA->CRH &= ~(
                  (GPIO_CRH_CNF8 | GPIO_CRH_MODE8) |
                    (GPIO_CRH_CNF9 | GPIO_CRH_MODE9) |
                      (GPIO_CRH_CNF10 | GPIO_CRH_MODE10)
                        );
  /* MCO Pin Configuration */
  GPIOA->CRH |= (uint32_t)(GPIO_CRH_CNF8_1 | GPIO_CRH_MODE8);
  /* USART Pin Configuration */
  GPIOA->CRH |= (uint32_t)((GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9) | (GPIO_CRH_CNF10_1));

  /* Reset(Clear) Port D CRH - User S1 Button */
  GPIOD->CRH &= ~(uint32_t)((GPIO_CRH_MODE) | (GPIO_CRH_CNF));
  /* User S1 Button Configuration */
  GPIOD->CRH |= (uint32_t)(GPIO_CRH_CNF11_1);
  }
  ```
  
&nbsp;&nbsp;이번 실험에서는 MCO 및 UART 사용을 위한 PORT/PIN 의GPIO 설정을 위해 PA8, PA9, PA10을 사용해야 한다. 따라서 GPIOA->CRH를 0000으로 초기화를 한다. MCO핀은 PA8이 관여를 하고 이를 Alternate function output Push-pull이 되도록 설정했다. USART TX는 PA9핀이고 Alternate function output Push-pull, USART RX는 PA10핀이고 Input with pull-up / pull-down으로 최기화가 필요한데, 이를 위에서 USART Pin Configuration부분에서 처리하였다.
&nbsp;&nbsp;Putty에 신호를 보내기 위해서 S1 버튼을 누르는데 GPIO D포트의 11번 핀을 사용한다. 우리가 지금까지 해왔듯, 코드를 0000으로 초기화하고 CNF를 1000으로 설정함으로써 입력값을 전달할 수 있도록 하였다.

### TODO-6 : 워드비트 8자리로 설정

  ``` C
  /*---------------------------- USART CR1 Configuration -----------------------*/
  /* Clear M, PCE, PS, TE and RE bits */
  USART1->CR1 &= ~(uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE);
  /* Configure the USART Word Length, Parity and mode ----------------------- */
  /* Set the M bits according to USART_WordLength value */
  //@TODO - 6: WordLength : 8bit
  ```
  
&nbsp;&nbsp;USART_CR1 레퍼런스를 참고하면, WORDLENGTH에 관한 비트 M의 비트값 설정을 보면 비트값이 0일 때, WORD_LENGTH가 8BIT임을 알 수 있다. USARTINIT()함수의 초기화부분(맨 첫줄)에서 USART_CR1_M을 이용하여 비트를 0으로 초기화 하였으므로, 따로 코드를 추가하지는 않았다.

### TODO-7 : parity:none(disable)

  ``` C
  /* Set PCE and PS bits according to USART_Parity value */
  //@TODO - 7: Parity : None
  ```
  
&nbsp;&nbsp;USART_CR1의 레퍼런스를 확인해 보면 비트값이 0일 때, PARITY기능을 disable한다고 나와 있다. uartinit()함수 초기화 부분에서 usart_cr1_pce를 이용한 초기화를 진행하였으므로, 따로 코드를 추가하지는 않았다.

### TODO-8 : enable tx and rx

  ``` C
  //@TODO - 8: Enable Tx and Rx
  USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
  ```
  
&nbsp;&nbsp;레퍼런스 27.6.4를 참고하여 연산을 진행하였다.

### TODO-9 : stop bit 1로 초기화

  ``` C  
  /*---------------------------- USART CR2 Configuration -----------------------*/
  /* Clear STOP[13:12] bits */
  USART1->CR2 &= ~(uint32_t)(USART_CR2_STOP);
  /* Configure the USART Stop Bits, Clock, CPOL, CPHA and LastBit ------------*/
  USART1->CR2 &= ~(uint32_t)(USART_CR2_CPHA | USART_CR2_CPOL | USART_CR2_CLKEN);
  /* Set STOP[13:12] bits according to USART_StopBits value */
  //@TODO - 9: Stop bit : 1bit
  ```
  
&nbsp;&nbsp;USART1->CR2 &= (uint32_t) ~(USART_CR2_STOP);
 코드로 이미 stop bit를 초기화 한 상태이다. 레퍼런스 27.6.5를 보면, usart_cr2가 나오는데, stop bit가 1이기 위해서는 비트의 값이 0이어야 한다. 초기화 해놓은 상태와 같은 상태이기 때문에 추가적인 코드작성이 필요하지 않다.

### TODO-10 : CTS, RTS disable

  ``` C  
  /*---------------------------- USART CR3 Configuration -----------------------*/
  /* Clear CTSE and RTSE bits */
  USART1->CR3 &= ~(uint32_t)(USART_CR3_CTSE | USART_CR3_RTSE);
  /* Configure the USART HFC -------------------------------------------------*/
  /* Set CTSE and RTSE bits according to USART_HardwareFlowControl value */
  //@TODO - 10: CTS, RTS : disable
  ```
&nbsp;&nbsp;
  
### TODO-11 : Calculate & configure BRR

  ``` C
  /*---------------------------- USART BRR Configuration -----------------------*/
  /* Configure the USART Baud Rate -------------------------------------------*/
  /* Determine the integer part */
  /* Determine the fractional part */
  //@TODO - 11: Calculate & configure BRR
  USART1->BRR &= ~(uint32_t)(USART_BRR_DIV_Fraction|USART_BRR_DIV_Mantissa);
  USART1->BRR |= 0x70E;
  ```
&nbsp;&nbsp;USART1->BRR &= ~(uint32_t)(USART_BRR_DIV_Fraction|USART_BRR_DIV_Mantissa);을 이용하여 BRR을 configure해준후 (레퍼런스 27.6.3참고) 미션지에서 제공된 PCLK2 (26MHz)와 Baud Rate(14400)을 이용하여 USARTDIV를 구한다.여기서 USARTDIV는 26,000,000 / (16 * 14400) 으로 112.847 값이 나왔고 이 값을 USART1_BRR로부터 derive하기 위하여 DIV_Fraction과 DIV_Mantissa로 나누어 계산한다.DIV_Fraction은 소수점아래 값 0.847 * 16 = 13.552 와 가까운 정수값 14 즉, E 이고 DIV_Mantissa는 정수값 112의 16진수값 70이다.따라서, USART_BRR에 넣어야하는 값은 0x70E가 된다.
### TODO-12 : Enable USART (UE)

  ``` C
  //@TODO - 12: Enable USART (UE)
  USART1->CR1 |= (uint32_t)USART_CR1_UE;
  ```
&nbsp;&nbsp;27.6.4 레퍼런스 참고하여 라이브러리에서 정의된 UART를 Enable해주는 USART_CR1_UE를 사용한다.

### TODO-13 : 버튼 입력 시 message 설정

  ``` C
  int main() {
    int i;
    char msg[] = " Hello Team07\r\n";

    SysInit();
    SetSysClock();
    RCC_Enable();
    PortConfiguration();
    UartInit();

    // if you need, init pin values here


    while (1) {
      //@TODO - 13: Send the message when button is pressed
      if (~GPIOD->IDR & (0x00000800)) {
        for ( i=0; i<15; i ++) {
          SendData(msg[i]);
        }
        delay();
      }
    }
  }
  ```
&nbsp;&nbsp;if문에서 버튼 입력 시 GPIOD_IDR 레지스터의 해당 핀 번호로 0이 저장되기 때문에 not(~) 연산 후 1과 AND(&) 연산 하여 버튼 입력을 확인했고 입력이 확인이 되면 SendData함수를 이용하여 우리가 보낼 msg를 for문을 이용하여 전송한다.

## 5. 실험 결론
| <img src="https://user-images.githubusercontent.com/64721658/136804175-bec06e96-45c3-4b55-b7b9-86b02c194979.png" width ="500px"> | <img src="https://user-images.githubusercontent.com/64721658/136802328-ea76780e-f9c4-4ea7-a37b-985b056e7c8e.jpg" width="600px"> |
|:--:|:--:| 
| Putty | 오실로스코프 |

&nbsp;&nbsp;위 사진과 같이 작성한 main.c파일에서 전송한 Hello Team07 메시지를 putty에서 나오는 것을 확인하였다.또한 오실로스코프를 이용하여 MCO를 통해 나오는 System Clock 수치를 확인하였다.

&nbsp;&nbsp;실험과정에서 baud rate계산 시 실수를 하여 값을 잘못 입력하였지만, 다시 계산하여 결과값을 도출해 냈다. 이번 실험으로 단거리 비동기 통신인 uart방식으로 컴퓨터와 보드간의 통신과정 및 결과를 확인할 수 있었고, 근거리에서의 uart통신의 신속성과 정확성을 알 수 있었다. 처음으로 헤더파일 참조를 사용하여서 불편하기도 했지만, 조금 더 써보면 코드의 작업속도를 증진시킬 수 있는 방법이 될 거 같다는 것을 알 수도 있을 것 같았다.
