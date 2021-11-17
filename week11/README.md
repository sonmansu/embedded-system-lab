# 임베디드 시스템 설계 및 실험 11주차 결과 보고서
7조 이석우, 손수민, 정지용, 김춘수


## 1. 실험 목표

1. Timer 이해 및 실습
1. 서보모터 이해 및 실습

## 2. 배경 지식

### 2.1 실험에 필요한 장치 및 구현기법 설명

#### TFT LCD
&nbsp;&nbsp;초 박막 액정표시장치로 액체와 고체의 중간 특성을 가진 액정의 상태 변화와 편광판의 편광 성질을 이용하여 통과하는 빛의 양을 조절함으로써 정보를 표시한다. RGB 픽셀이 유리판에 코딩 되어 컬러 영상을 구현하는 Color Filter 와 액정을 제어하기 위해 초박형 유리 기판 위에 반도체 막을 형성한 회로인 TFT 기판, Filter와 기판 사이에 주입된 액정과 광원인 Black light unit으로 구성 되어있다.  

&nbsp;&nbsp;TFT LCD는 사용하는 인터페이스에 따라 timing 특성이 바뀐다. 실험에서 사용한 parallel 8080 series interface의 제어신호는 다음과 같다.

- CS : high에서 low일 때, chip이 동작되도록 하는 신호

- D/C : low시에 command를 전송하고, high시에 data를 전송

- WR : high에서 low로 변할 때, data를 display에 write

- RD : high에서 low로 변할 때, data를 display로부터 read


![image](https://user-images.githubusercontent.com/71700530/140936492-ba8b9b7a-ace9-4a05-b1c9-4ef79d0227f4.png)
![image](https://user-images.githubusercontent.com/71700530/140936512-c1b4b481-db97-474c-9b7d-2883c829671e.png)

#### PWM과 서보모터
&nbsp;Pulse Width Modulation 의 줄임말로 전압신호의 Pulse (전압 파형) 를 이용하여 원하는 전압신호 평균값을 출력으로 만들어내는 것이다. 디지털 신호는 0과 1의 값만 가질 수 있다. 이 디지털 신호의 0과 1의 비율을 조절하는 것이 PWM이고 이 비율을 Duty Cycle이라고 표현한다. PWM은 LED의 밝기, 모터 회전속도 제어, 서보모터, 위치제어 등에 쓰인다. 

&nbsp;대부분의 서보모터는 50Hz~1000Hz의 주파수를 요구하고 데이터 시트를 통하여 확인하여 사용해야 한다.

![image](https://user-images.githubusercontent.com/64721658/142144797-499bbd72-877e-4833-897b-2b4830329a8a.png)


#### Timer
&nbsp;&nbsp;타이머는 주기적 시간을 얻을 때 사용하는 디지털 카운터 회로 모듈이며, 타이머 모듈을 사용하여 일정한 주기적 시간 처리를 하는 시스템 요구에 대응한다. 마이크로 프로세서 상에서 구현될 때 클럭을 사용하는 것이 원천적이지만, 주파수가 높아 우선 프리스케일 카운터 회로를 사용하여 주파수를 낮춘 후에 낮아진 주파수로 8, 16비트 등의 카운터 회로를 사용하여 주기를 얻는다. 실험에서 사용하는 보드는 advanced-control timer, general-purpose timer, basic timer, systicktimer, watchdog timer가 있다. 이 중 이번 실험에서는 general-purpose timer(tim2,tim3)를 이용했다. 

#### 각 타이머의 기능 및 특징 

##### 1. Systick timer

&nbsp;&nbsp;Real-time operating system 전용 타이머이지만, standard down counter로 사용할 수도 있는 타이머이다. 24비트 다운 카운터로, 카운터 값이 0이 되면 그 다음 클럭에 reload 레지스터에 프로그래밍 한 reload 값으로 카운터 값이 갱신된다. 카운터 값이 1에서 0으로 바뀔 땐, systick예외 요청이 발생한다. 개발자가 예외 핸들러를 작성하면 이 프로그램은 systick 예외 요청이 발생할 때마다 주기적으로 실행될 수 있다. 

##### 2. IWDG/WWDG Timer-Watching timer

&nbsp;&nbsp;컴퓨터의 오작동을 감지하고 복구하기 위해 사용되는 타이머이다. 정상 작동 중인 컴퓨터는 시간이 경과하거나 초과되는 것을 막기 위해 정기적으로 watchdog timer를 재가동시킨다. Iwdg는 independent watchdog timer의 약자이고, lsi 클럭 기반으로 메인 클럭 고장에도 활성 상태를 유지할 수 있다. 타이밍 정확도 제약이 낮은 프로그램에 적합하며, 카운터가 0이 되면 reset이 된다. Wwdg는 window watchdog timer의 약자이며, 7비트 다운 카운터이고, wwdg의 클럭은 apb1클럭을 프리스케일 해서 정의 가능하다. 비정상적인 어플리케이션 동작 감지를 위해 설정 가능한 time-window 가 있다. Time- window내에서 반응하도록 요구하는 프로그램에 적합하며, 카운터가 0x40보다 작을 경우 또는 카운터가 time-wondow 밖에 reload 됐을 경우 reset이 가능하다. Early wakeup interrupt(ewi)는 카운터가 0x40과 같을 때, ewi 인터럽트가 발생하도록 설정이 가능한 특성이다. 

##### 3. Advanced-control timer(tim1 & tim8)

&nbsp;&nbsp;Prescaler를 이용하여 설정 가능한 16bit auto-reload counter를 포함하고 있다. 입력 신호 펄스 길이 측정 또는 출력 파형 생성 등에 이용이 가능하며, the advanced-control and general-purpose는 자원을 공유하지 않는 독립적인 구조로, 동기화 시키는 것도 가능하다. 

##### 4. General-purpose timer(tim2 to tim5)

&nbsp;&nbsp;Prescaler를 이용해 설정 가능한 16bit auto-reload counter를 포함하고 있다. 입력 신호의 펄스 길이 측정 또는 출력 파형 발생 등 다양한 용도로 사용될 수 있다. 펄스 길이와 파형 주기는 timer prescaler와 the rcc clock controller prescaler를 사용하여 us~ms 단위까지 변조가 가능하다. 타이머들은 완전히 독립적이며, 자원을 공유하지 않지만, 동기화가 가능하다. 

##### 5. Basic timer

&nbsp;&nbsp;주로 DAC trigger generation에 사용된다. Stm32에서는 tim6와 tim7이 basic timer이다. 16bit auto-reload 업카운터이고, 설정 가능한 16bit prescaler를 이용하여 counter clock 주파수를 나눠서 설정이 가능하다. DAC 트리거에 사용되며, 카운터 오버플로우 발생 시 인터럽트/DMA를 생성한다. 

| <img src="https://user-images.githubusercontent.com/64721658/142143425-122ddcaa-464c-4311-9437-e6bdf838b92b.png" width="550px"> |
| :----------------------------------------------------------: |
|                   <그림1> 각 타이머별 특징                   |

| <img src = "https://user-images.githubusercontent.com/64721658/142143591-c5cf1486-3798-45f9-a083-8d146c832906.png" width = "550px"><br /> |
|:--:|
| <그림2> 클럭 설정 관련 트리 그림 |

|<img src="https://user-images.githubusercontent.com/64721658/142143609-a6bcc7e5-db64-44bd-95bf-138117755e68.png" width = "250px"> |
|:--:|
|<그림3> 실험에서 사용될 분주계산식 |

분주 계산시에 prescaler와 period의 값은 0부터 시작하므로, 입력하고자 하는 값에 1을 뺀 후 대입해 준다. 

## 3. 실험 과정

### main.c   

먼저 이번 실험에서 led, lcd, 서보모터 사용에 필요한 부분들을 Configure한다.

``` C
void RCC_Configure(void)
{
      /* LCD */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // port C RCC ENABLE
      /* PWM */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // TIM2
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // Port B
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // TIM3
      /* LED */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); // Port D
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

}

void GPIO_Configure(void)
{

    GPIO_InitTypeDef GPIO_InitStructure3;
    GPIO_InitStructure3.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure3.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure3.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure3); // led 활성화

   /* PWM */
    GPIO_InitTypeDef GPIO_InitStructure2;
    GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure2);

}

void NVIC_Configure(void) {
    
     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
     NVIC_InitTypeDef NVIC_InitStructure;
     /* Enable TIM2 Global Interrupt */
     NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);

}
```
이번 실험에서 새롭게 사용되는 PWM을 2개를 Configure 하였는데 하나는 Led(TIM2) 하나는 서브모터(TIM3)로 2개를 구성하였다.
PWM_Configure1은 1초에 한번씩 작동하도록 설정해주기 위해 (1/72Mhz)*10000*(72M/10000) = 1의 계산식으로 1초로 설정해주었다.
서브모터는 버튼 클릭시에만 작동하도록 설정해주었다.

```C
void PWM_Configure1()
{
   prescale = (uint16_t) (SystemCoreClock / 10000);
   TIM_TimeBaseStructure.TIM_Period = 10000;
   TIM_TimeBaseStructure.TIM_Prescaler = prescale;
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
   TIM_ARRPreloadConfig(TIM2, ENABLE);
   TIM_Cmd(TIM2, ENABLE);
}

void PWM_Configure2()
{
   prescale = (uint16_t) (SystemCoreClock / 1000000);
   TIM_TimeBaseStructure.TIM_Period = 20000;
   TIM_TimeBaseStructure.TIM_Prescaler = prescale;
   TIM_TimeBaseStructure.TIM_ClockDivision = 0;
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = 1500; // us
   TIM_OC3Init(TIM3, &TIM_OCInitStructure);
   TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
   TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
   TIM_ARRPreloadConfig(TIM3, ENABLE);
   TIM_Cmd(TIM3, ENABLE);
}
```
LED 2개를 동시에 제어하기 위해 interrupt에 대한 Handler함수를 선언 및 구현한다.

```C
void TIM2_IRQHandler(void) {
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
      if (ledCount % 2 == 0) {
      GPIO_SetBits(GPIOD, GPIO_Pin_2);
      } else{
      GPIO_ResetBits(GPIOD, GPIO_Pin_2); 
      }

      if(ledCount == 0) {
      GPIO_ResetBits(GPIOD, GPIO_Pin_3);
      } else if(ledCount == 5) {
      GPIO_SetBits(GPIOD, GPIO_Pin_3);
      }

      ledCount++;
      ledCount %= 10;

      TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}
```

처음 LCD 화면에서는 THU_TEAM07과 BTN 버튼 OFF가 출력된다.
LCD 버튼 터치를 인식하기 위해 터치값 좌표를 입력을 받아서 좌표값이 사각형 내부이면 TIM2 interrupt를 활성화시킨다.
터치한 좌표값이 사각형 내부로 인식되면 Off에서 On으로 바뀌면서 LED toggle이 진행되도록 한다.이때, 서브모터도 rotation값이 변경되면서 -90에서 90으로 각이 이동하게된다. 이후 다시 버튼을 터치하게 되면 On에서 Off로 변하면서 LED toggle이 중단되고 서브모터도 90에서 -90으로 움직이게 된다.

```C
int main(void)
{

      SystemInit();
      RCC_Configure();
      GPIO_Configure();
      PWM_Configure1();
      PWM_Configure2();

      NVIC_Configure();

      //-----------------

      LCD_Init(); // LCD 초기화
      Touch_Configuration(); // 터치 설정
      Touch_Adjust(); // 화면 터치 초점 맞추기
      LCD_Clear(WHITE); // LCD 배경 초기화

      //-----------------
      uint16_t rotation[2] = {700, 2300};
      int i = 0;

      uint16_t pos_temp[2]; // x, y좌표를 담을 배열

       LCD_ShowString(40, 10, "THU_Team07", MAGENTA, WHITE); // 팀명 출력
       LCD_ShowString(90, 50, "OFF", RED, WHITE); // 디폴트로 OFF 
       LCD_DrawRectangle(40, 80, 80, 120); // 사각형 출력
       LCD_ShowString(50,90, "BTN", MAGENTA, WHITE); // "Button" 글자 출력
      
      while (1) {
           Touch_GetXY(&pos_temp[0], &pos_temp[1], 1); // 터치 좌표 받아서 배열에 입력
           Convert_Pos(pos_temp[0],pos_temp[1],&pos_temp[0],&pos_temp[1]); // 받은 좌표를 LCD 크기에 맞게 변환

           if ((uint16_t)40 < pos_temp[0] && pos_temp[0] < (uint16_t)80 && \
               (uint16_t)80 < pos_temp[1] && pos_temp[1] < (uint16_t)120) {
              if (flag) {
                 LCD_ShowString(90, 50, "    ", RED, WHITE); 
                 LCD_ShowString(40, 50, "ON", RED, WHITE); 
                 TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // interrupt enable
              } else {
                 LCD_ShowString(90, 50, "OFF", RED, WHITE); 
                 LCD_ShowString(40, 50, "    ", RED, WHITE); 
                 TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); // interrupt enable
              }
              flag++;
              flag %= 2;
           }

           TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
           TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
           TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
           TIM_OCInitStructure.TIM_Pulse = rotation[i]; // us
           TIM_OC3Init(TIM3, &TIM_OCInitStructure);
            i++;
            i %= 2;
            delay();

      }
      return 0;
}
```

## 4. 실험 사진
1. 처음 실행하였을때 팀명, 버튼, OFF가 출력됨을 확인할 수 있다.

<img src="https://user-images.githubusercontent.com/64721658/142150620-4b2e08a6-2539-4c02-b21b-78affb21aecb.jpg" width = "600px">

2. 버튼을 클릭하면 ON으로 출력이 바뀌고 LED toggle이 시작되는것을 알 수 있다. 또한, 서보모터도 90도에서 -90도로 이동한것을 확인 할 수 있다.

   <img src="https://user-images.githubusercontent.com/64721658/142150649-12450c5a-dee7-4495-9d0f-a3ca5befd6a0.jpg" width = "600px">



## 5. 결론

&nbsp;&nbsp;이번 실험으로 타이머를 코드로 구현하여 보드상에서 제어하는 방법을 학습하였다. 처음엔 어떤 타입의 타이머를 써야 하며, 타이머 간의 관계 지정에 대해 미숙하여 시간이 오래 걸렸지만, 강의자료와 레퍼런스를 참고하여 코드를 작성하였다.특히 서보모터와 led toggle을 동시에 구현하려고 하였는데 둘 중 하나가 안되는 에러가 자주 발생하여 해결하는데 오랜 시간이 소요되었다.이번 실험에서 배운 타이머 제어를 활용해 추후 텀 프로젝트에서 유용하게 사용할 것 같다.
