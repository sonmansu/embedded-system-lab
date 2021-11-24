# 임베디드 시스템 설계 및 실험 12주차 결과 보고서
7조 이석우, 손수민, 정지용, 김춘수


## 1. 실험 목표
1. DMA 동작 방법의 이해
1. DMA 구현

## 2. 배경 지식

### Direct Memory Access (DMA)

&nbsp;&nbsp;DMA란 주변장치들이 메모리에 직접 접근하여 읽거나 쓸 수 있도록 하는 기능이다. CPU 의 개입 없이 I/O 장치와 기억장치 데이터를 전송할 수 있게 되어, CPU는 Interrupt와 달리 별도의 명령을 실행할 필요가 없다. 메모리 처리 Interrupt의 사이클만큼 성능이 향상된다.


| <img src = "https://user-images.githubusercontent.com/62247273/142562129-9a934f4c-8b0c-4610-8b1e-6688139ffe69.png" width = "300px"> | <img src = "https://user-images.githubusercontent.com/62247273/142562130-809a6792-6013-450f-be57-e18cbd7600c4.png" width = "300px"> |
|:---:|:---:|
| 일반적인 메모리 접근 방식 | DMA 방식 |

&nbsp;&nbsp;첫 번째 그림은 DMA가 작동하지 않을 떄의 흐름을 나타낸 모식도이다. 모든 I/O로의 접근은 CPU를 통해서 수행된다. Data를 전달할 때마다 CPU가 관여한다. 반면, 두 번째 그림은 DMA 방식을 나타낸 것이다. RAM이 I/O 장치로부터 데이터가 필요하면, CPU는 DMA 컨트롤러에게 신호(전송 크기, 주소 등)을 보낸다. DMA 컨트롤러가 RAM 주소로 데이터를 bus를 통해 주고 받으며, 모든 데이터 전송이 종료되면 DMA 컨트롤러가 CPU에 인터럽트 신호를 보낸다.

### DMA Channel
&nbsp;&nbsp;모듈은 DMA 컨트롤러의 DMA 채널을 통해 메모리 Read/Write를 할 수 있다. STM32 보드의 DMA 채널은 총12개(DMA1 채널 7개, DMA2 채널 5개)이다. 한 DMA의 여러 채널 사이 요청은 우선순위에 따르는데 4가지 단계로 구성이 되어 있다(very high, high, medium, low). 주변 기기-메모리, 메모리-주변기기, 주변기기-주변기기 간의 전송이 가능하다.

| <img src = "https://user-images.githubusercontent.com/62247273/142719658-db49c6cb-d781-4f4b-9b2f-ac75a54be077.png" width = "300px"> | <img src = "https://user-images.githubusercontent.com/62247273/142719677-537cd11a-01af-4e9b-9003-25a81be4395b.png" width = "300px" > |
|:---:|:---:|
| DMA1 채널 | DMA2 채널 |

### DMA Mode
&nbsp;&nbsp;DMA Mode는 두 가지가 있는데 첫 번째로 Normal Mode가 있다. DMA 컨트롤러는 데이터를 전송할 때마다 NDT값을 감소시킨다. NDT는 DMA를 통해 전송할 데이터의 총 용량을 의미하며 레지스터의 값이 0이 되면 데이터 전송 중단한다. 데이터 전송을 받고 싶을 때 마다 새롭게 요청이 필요하다.
</br>&nbsp;&nbsp;두 번째는 Circular Mode로서 주기적인 값의 전송(업데이트)이 필요할 때 사용하는 모드이다. NDT값이 0이 될 경우 설정한 데이터 최대 크기로 재설정된다.

### DMA Controller
&nbsp;&nbsp;DMA 컨트롤러는 I/O 장치가 프로세서의 참여를 제한하면서 메모리에 직접 액세스할 수 있도록 하는 하드웨어 장치이다. DMA 컨트롤러는 CPU 및 입출력 장치와 통신하기 위해 인터페이스의 동일한 회로가 필요하다. 
![캡처_2021_11_20_16_19_25_598](https://user-images.githubusercontent.com/62247273/142718240-e5bf6c90-49d7-4021-9f92-2ae5dd5769f7.png)
</br>&nbsp;&nbsp;위 사진은 DMA Block Diagram인데, 이것을 이용해서 사용하고자 하는 DMA의 버스와 채널을 선택할 수 있다.

## 3.실험 과정
&nbsp;&nbsp;이번 실험의 핵심은 인터럽트를 사용하지 말고, DMA를 사용하는 것이다. 그래서 ADC를 설정할 때 우리는 ADC_ITConfig 함수 대신 ADC_DMACmd함수를 사용하였다.
``` C
//ADC init 함수
void ADC_Configure(void) {
  ADC_InitTypeDef ADC_InitStructure;
  // ADC1 Configuration
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1,
                           ADC_SampleTime_239Cycles5);
  ADC_DMACmd(ADC1, ENABLE); //dma enable
  ADC_Cmd(ADC1, ENABLE); // ADC1 enable
  ADC_ResetCalibration(ADC1);

  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
```

&nbsp;&nbsp;다음으로 DMA_Configure함수로 DMA를 초기 설정한다.
``` C
void DMA_Configure(void) {
  DMA_InitTypeDef DMA_InitStructure;
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &ADC1->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &ADC_Value;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;

  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1, ENABLE);
}
```
&nbsp;&nbsp;각각 변수의 의미를 Stm320f10x_dma.h파일을 참고하여 설명하겠다. 
```C 
/*  1. DMA_PeripheralBaseAddr은 DMA 채널의 주변 장치의 기본 주소를 지정하는 변수이다. 
    2. DMA_MemoryBaseAddr은 DMA 채널의 메모리 기본 주소를 지정한다. 
    3. DMA_DIR은 주변 장치가 소스(source)인지 대상(destination)인지 지정하는 변수이다. 
    4. DMA_BufferSize는 지정된 채널의 버퍼 사이즈를 데이터 단위로 지정하는데, 
    데이터 단위는 DMA_PeripheralDataSize에 설정된 구성과 동일하다.
    5. DMA_PeripheralInc는 주변기기의 주소 레지스터가 증가할지 여부를 지정한다.
    6. DMA_MemoryInc는 메모리 주소 레지스터가 증가할지 여부를 지정한다.
    7. DMA_PeripheralDataSize는 주변기기의 데이터 width를 지정한다.
    8. DMA_MemoryDataSize는 메모리의 데이터 width를 지정한다.
    9. DMA_Mode는 DMA 채널의 작동 모드를 지정한다.
    10. DMA_Priority는 DMA 채널에 대한 소프트웨어 우선 순위를 지정한다.
    11. DMA_M2M은 DMA 채널이 memory to memory 전송에 사용되는지 여부를 지정한다.
*/    
```

&nbsp;&nbsp;인터럽트를 사용하지 않기 때문에 별도의 핸들러 함수는 필요하지 않다. DMA 컨트롤러를 통해서 데이터를 주고 받을 때 CPU의 관여가 필요 없다. 이번 실험에서는 조도 센서의 값을 받아와야 한다. 전역변수로 선언한 ADC 값을 저장할 공간을 항상 참조하도록 volatile 키워드를 사용했다.
``` C
  volatile uint32_t ADC_Value[1]; //전역변수로 선언한 adc값
```
다음은 메인 함수이다.
``` C
int main(void)
{
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  DMA_Configure();
  ADC_Configure();
  NVIC_Configure();

  LCD_Init();
  Touch_Configuration();
//  Touch_Adjust();
  int threshold = 700; //평소엔 1200~1500 / 플래시 비추면 200

  LCD_Clear(WHITE);
  int adc=0;

  while (1) {
    if (ADC_Value[0] < threshold) { //플래시로 비췄을 때
      LCD_Clear(GRAY);
      LCD_ShowNum(50, 50, ADC_Value[0], 5, YELLOW, GRAY);
    } else {
      LCD_Clear(WHITE);
      LCD_ShowNum(50, 50, ADC_Value[0], 5, BLACK, WHITE);
    }
  }
  return 0;
}
```
&nbsp;&nbsp;이번 실험은 평상시 TFT-LCD의 배경색이 WHITE, 조도센서에 스마트폰 플래시를 비출 때 TFT-LCD 배경색의 GRAY가 되도록 하는 것이 목표이다. 실험하는 조마다 threshold의 값이 다르겠지만, 7조의 경우에는 평소에는 1200~1500, 플래시를 비추었을 때 200의 조도센서 값이 나왔고, 그 사이의 값인 700을 threshold의 값으로 하였다.

## 4. 실험 사진
| <img src = "https://user-images.githubusercontent.com/62247273/142719298-315b5ab9-1855-498f-87c0-da0cca1b152a.jpg" width = "300px"> | <img src = "https://user-images.githubusercontent.com/62247273/142719306-d35a7b12-9428-4275-9504-d1b819f34553.jpg" width = "300px"> |
|:---:|:---:|
| 플래시를 비추기 전 | 플래시를 비추고 난 후|


## 5. 결론
&nbsp;&nbsp;DMA는 CPU에 인터럽트 신호를 보내지 않고도, 데이터 통신을 가능하게 한다. CPU의 작업 흐름을 방해하지 않으며, 인터럽트를 처리하지 않아도 되는 만큼, 작업 효율은 향상되며 더욱 빨리 프로세스를 진행할 수 있다. 그렇다고 하여 DMA만 사용해야 하는 것은 아니다. 중요한 데이터의 경우에는 CPU를 통과해야 하는 경우도 있을 것이다. DMA 방식이 효과적임에도 인터럽트 방식이 지금까지 사용되고 있는 것은 두 가지 방법 개개의 장/단점이 있기 때문일 것이다. 텀프로젝트를 진행하면서 DMA를 적재적소에 잘 활용하야 하겠다.
