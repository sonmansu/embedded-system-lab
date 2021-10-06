# 임베디드 시스템 설계 및 실험 5주차 결과 보고서
7조 이석우, 손수민, 김춘수, 정지용
 
## 1. 실험 목표
1. 스케터 파일의 이해 및 플래시 프로그래밍
2. 릴레이 모듈의 이해 및 임베디드 펌웨어를 통한 동작
3. 센싱에서 폴링 방식의 이해

## 2. 실험 원리 및 과정
### 스캐터 파일
 이번 실험에서 우리는 scatter file을 이용하여 scatter loading을 하는 방식을 이용한다. 스캐터 로딩이란 데이터를 원하는 위치에서 읽을 수 있도록 메모리를 따로 분리해서 여러 곳에 올리는 것을 말하며, 스캐터 파일이란 분산 적재로, 실행시킬 바이너리 이미지가 메모리에 로드될 때, 바이너리 이미지의 어떤 영역이 어느 주소에 얼마의 크기만큼 배치되어야 하는지를 작성한 파일이다. 스캐터 파일은 바이너리의 여러 부분을 각각 별개의 메모리 영역에 로드해야 하거나, 자주 사용되거나 빠른 실행을 요구하는 코드영역을 접근시간이 빠른 메모리에 우선 배치하도록 하여 효율적인 section관리가 가능하도록 할 수 있다. 스캐터 파일 구성은 region, output, input section들로 구성되며, region은 output section들로 구성되고, output section들은 input section들로 구성된다. Input section은 ro(code), rw(global data), ZI(zero initialized) 이 세 개의 특성 중 하나의 특성을 가진다. Output section이 어떤 input section으로 구성되는지는 스캐터 파일에 직접 작성되어 있지는 않지만, linker가 임의로 만들어낸다. 스캐터 파일에 input section과 region만 표기해 주면, linker가 input section을 임의로 모아 output section과 region을 생성한다.
  
| <img src="https://user-images.githubusercontent.com/80534651/136148926-8ce85640-7288-43be-abbd-888548195d7a.png" width="500px"> | <img src="https://user-images.githubusercontent.com/80534651/136158891-92dfb518-23fb-422d-9e1f-d33e7484364e.png" width="500px"> | 
|:--:|:--:| 
| 스캐터 파일 | memory가 작성된 스캐터 파일 |

  
 ### 릴레이 모듈
 전자기 유도원리를 이용한 스위치 역할을 하며, 릴레이에 신호를 가하면 출력 상태가 변경된다. 
제어 신호로 신호를 주고, VCC에는 3.3V전원을 인가하며, 접지에는 GND를 연결한다. 그리고 릴레이 모듈의 COM부분은 5V전원이 인가되며, NC, NO는 작동 방식이 다르다. NO는 평소에 open되어 있어 high상태이지만, 전원 인가 시(high신호)에는 close되어 low가 된다. NC는 반대로 평소에 close 되어 low상태이지만, 전원 인가 시(high신호)에는 open되어 high가 된다. 

| <img src="https://user-images.githubusercontent.com/80534651/136149041-58d5fc9c-b407-498d-a252-b28ed5d87477.png" width="100%"> | 
|:--:| 
| 릴레이 모듈 |

### 실험 과정 
| <img src="https://user-images.githubusercontent.com/80534651/136159776-9699a3db-ad77-44d4-b6af-ce67afd4d84a.jpg" height="300px"> | <img src="https://user-images.githubusercontent.com/80534651/136149051-ba135553-ee9b-4af5-b39f-f2941d2907f6.png" height="300px"> | <img src="https://user-images.githubusercontent.com/80534651/136149061-4f270c6a-b9bd-49ea-b1a0-71925dc7a354.png" height="300px"> | 
|:--:|:--:|:--:| 
| 보드 전체 구성 | 릴레이 모듈 연결 | 모터 연결 |

### 코드 구현
  init() 함수에 진입 후, 초기화 과정을 거친다. 우선 이번 실험에서 S1, S2에 대한 초기화를 하기 위해, Reset Value에 해당하는 0x44444444를 입력한 다음, 해당 위치에 8의 값(x00088000)을 주어 input의 configuration을 실시한다. 릴레이 모듈(PC8)도 마찬가지로 reset 및 output configuration을 시행한다.  
  
  ``` C
  void init() {
      RCC_APB2ENR = 0x30; // Port C,D Clock Enable

      // LED PD7
      GPIOD_CRL = 0x44444444; // reset
      GPIOD_CRL = 0x30000000; // led PD 7 output push-pull
      GPIOD_BSRR = 0x00000000; // led bsrr 리셋

      //버튼: PD11(S1), PD12(S2)
      GPIOD_CRH = 0x44444444; // reset
      GPIOD_CRH = 0x00088000; // 버튼 configuration input 
      GPIOD_IDR = 0x00000000; // 버튼 INPUT DATA REGISTER RESET

      //릴레이 모듈: PC8
      GPIOC_CRH = 0x44444444; // reset
      GPIOC_CRH = 0x00000003; // 릴레이 PC8 컨피규어레이션, output  push-pull
      GPIOC_BSRR = 0x00000000; // 릴레이 bsrr 리셋
}
  ```
  while()문을 통하여 해당 명령을 수행하는데. S1 버튼을 누를 경우 pc8이 set되고 모터가 작동되고, 약간의 delay를 거친 후 다시 reset되어 모터가 꺼진다. S2버튼을 누를 경우, 위와 마찬가지로 pd7이 set되어 led가 점등 된 후, delay를 거쳐 led가 꺼진다. 
  ``` C
  while(1){
    if (~GPIOD_IDR & 0x00000800) {            // pd 11번 버튼 눌렀을때 모터 작동

      GPIOC_BSRR = 0x00000100;                // relay on : pc8 set
      delay();
      GPIOC_BSRR = 0x01000000;                // relay off : pc8 reset

    } else if (~GPIOD_IDR & 0x00001000) {   // pd 12번 버튼 눌렀을 때  led 켬

      GPIOD_BSRR = 0x00000080;                // led7 on  :  pd7 bsrr set
      delay();
      GPIOD_BSRR = 0x00800000;                // led7 off :  pd7 bsrr reset

    }
  }
  ```
  
  
  
## 3. 실험 결과 및 향후 계획
  이번 실험을 통해, 외부 모듈을 보드와 연결하고, 직접적인 메모리 참조를 통해 동작을 제어하는 방법을 공부하였다. 이번 실험에서 어려웠던 점은 아무래도 보드 외부의 모듈을 제어하는 것이라, 부품 자체의 문제를 무시할 수 없었다는 점이다. 특히, 릴레이 모듈의 NC 및 NO 출력의 경우, 제공된 실험자료를 토대로 우리가 기대하던 반응과 반대의 결과가 나와 적잖이 당황하였다. 향후 실험에서도 이러한 부분들을 주의하며 진행해 나가야겠다.
  
     
