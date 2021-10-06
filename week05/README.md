# 임베디드 시스템 설계 및 실험 5주차 결과 보고서
7조 이석우, 손수민, 김춘수, 정지용
 
## 1. 실험 목표
1. 스캐터 파일의 이해 및 플래시 프로그래밍
2. 릴레이 모듈의 이해 및 임베디드 펌웨어를 통한 동작
3. 센싱에서 폴링 방식의 이해

## 2. 실험 이론
### 스캐터 파일

&nbsp;&nbsp;이번 실험에서 우리는 scatter file을 이용하여 scatter loading을 하는 방식을 이용한다. 스캐터 로딩이란 데이터를 원하는 위치에서 읽을 수 있도록 메모리를 따로 분리해서 여러 곳에 올리는 것을 말하며, 스캐터 파일이란 분산 적재로, 실행시킬 바이너리 이미지가 메모리에 로드될 때, 바이너리 이미지의 어떤 영역이 어느 주소에 얼마의 크기만큼 배치되어야 하는지를 작성한 파일이다. 스캐터 파일은 바이너리의 여러 부분을 각각 별개의 메모리 영역에 로드해야 하거나, 자주 사용되거나 빠른 실행을 요구하는 코드영역을 접근시간이 빠른 메모리에 우선 배치하도록 하여 효율적인 section관리가 가능하도록 할 수 있다. 스캐터 파일 구성은 region, output, input section들로 구성되며, region은 output section들로 구성되고, output section들은 input section들로 구성된다. Input section은 ro(code), rw(global data), ZI(zero initialized) 이 세 개의 특성 중 하나의 특성을 가진다. Output section이 어떤 input section으로 구성되는지는 스캐터 파일에 직접 작성되어 있지는 않지만, linker가 임의로 만들어낸다. 스캐터 파일에 input section과 region만 표기해 주면, linker가 input section을 임의로 모아 output section과 region을 생성한다.
  
| <img src="https://user-images.githubusercontent.com/80534651/136148926-8ce85640-7288-43be-abbd-888548195d7a.png" width="500px"> | <img src="https://user-images.githubusercontent.com/80534651/136158891-92dfb518-23fb-422d-9e1f-d33e7484364e.png" width="500px"> | 
|:--:|:--:| 
| 스캐터 파일 | memory가 작성된 스캐터 파일 |

  
 ### 릴레이 모듈

&nbsp;&nbsp;릴레이 모듈은 릴레이를 제어하는 모듈로, 전자기 유도원리를 이용하여 스위치 역할로써 사용가능하다. 릴레이의 제어 신호에 신호를 가하여 출력상태(ON/OFF)를 변경할 수 있다.   
&nbsp;&nbsp;릴레이 모듈의 VCC와 GND에 전원을 연결하고 제어 신호(IN)에는 PC8과 연결했다. COM은 제어 신호(IN)에 따라 NO 또는 NC로 붙는데, NO(Normally Closed)는 평소에 close 돼있고 high 신호를 보내면 닫혀있던 스위치가 열려 흐르던 전류가 멈춘다. NC(Normally Open)는 평소에 open 돼있고 high 신호를 보내면 열려있던 스위치가 닫혀 전류가 흐른다.

| <img src="https://user-images.githubusercontent.com/80534651/136149041-58d5fc9c-b407-498d-a252-b28ed5d87477.png" width="100%"> | 
|:--:| 
| 릴레이 모듈 |

## 3. 실험 실습

### 실험 과정 사진
| <img src="https://user-images.githubusercontent.com/80534651/136159776-9699a3db-ad77-44d4-b6af-ce67afd4d84a.jpg" height="300px"> | <img src="https://user-images.githubusercontent.com/80534651/136149051-ba135553-ee9b-4af5-b39f-f2941d2907f6.png" height="300px"> | <img src="https://user-images.githubusercontent.com/80534651/136149061-4f270c6a-b9bd-49ea-b1a0-71925dc7a354.png" height="300px"> | 
|:--:|:--:|:--:| 
| 보드 전체 구성 | 릴레이 모듈 연결 | 모터 연결 |

### 코드 설명
&nbsp;&nbsp;먼저 init() 함수를 통해 사용할 레지스터들에 대한 기본 설정과 초기화 작업을 수행한다. 릴레이 모듈은 Port C를 사용하고 버튼과 LED는 Port D를 사용하기 때문에 RCC_APB2ENR 레지스터에 0x30을 넣어 포트 C, D 에 clock을 인가했다. 그 다음 PD7 LED를 사용하기 위해 GPIOD_CRL을 reset 해주고 output 모드로 설정해준다. LED를 키고 끄기 위해 GPIOD_BSRR 레지스터를 reset 해준다. 그리고 PD11, PD12 버튼을 사용하기 위해 GPIOD_CRH를 reset 후 input 모드로 설정하고 입력을 저장하기 위해 GPIOD_IDR 레지스터를 reset 한다. 마지막으로 릴레이를 PC8로 제어하기 위해 GPIOC_CRH를 reset 후 output 모드로 설정하고 GPIOC_BSRR을 reset해준다.
  
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
&nbsp;&nbsp;이번 실험에서 사용할 delay는 delay함수를 만들어 사용한다.delay 함수 내부는 ppt의 내용을 참고하여 for문을 이용한 공루프 형태로 구현한다.


  ``` C
  void delay(void) {
    int i=0;
    for(i=0;i<1000000;i++)  {} //empty Loop
  }
  ```

&nbsp;&nbsp;while()문에서는 if-else 조건문으로 버튼 입력에 따라 모터를 작동시키거나 LED를 점멸한다. 버튼 입력 시 GPIOD_IDR 레지스터의 해당 핀 번호로 0이 저장되기 때문에 not(~) 연산 후 1과 AND(&) 연산 하여 버튼 입력을 확인했다. S1 버튼을 누를 경우 GPIOC_BSRR의 8번 bit를 set하여 릴레이에 high 신호를 주어 모터를 작동시킨다. delay() 함수 동안 모터를 작동 시킨 후 GPIOC_BSRR의 24번 bit에 1을 주어 reset 시켜 모터를 멈춘다. S2 버튼을 누를 경우 LED는 GPIOD_BSRR의 7번 bit에 1을 주어 점등하고 delay() 후 23번 bit에 1을 주어 소등한다.


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
  
  
  
## 4. 실험 결과 및 향후 계획
&nbsp;&nbsp;이번 실험에서는 외부 모듈을 보드와 연결하고, 릴레이 모듈을 통해 동작을 제어하는 방법을 공부하였다. 그리고 실험을 통해 풀링 방식의 단점을 체험해볼 수 있었다. 폴링 방식은 delay() 함수가 동작하는 동안엔 버튼을 눌러도 신호를 읽을 수 없어 delay() 함수가 끝난 후 버튼을 눌러줘야만 했다. 반면 interrupt 방식을 이용하면 delay() 함수가 동작하는 동안에 인터럽트가 들어오더라도 인터럽트 핸들러가 신호를 처리할 수 있다.   
&nbsp;&nbsp;실험 도중 어려웠던 점은 아무래도 보드 외부의 모듈을 제어하는 것이라, 부품 자체의 문제를 무시할 수 없었다는 점이다. 특히, 릴레이 모듈의 NC 및 NO 출력의 경우, 제공된 실험자료를 토대로 우리가 기대하던 반응과 반대의 결과가 나와 적잖이 당황하였다. 향후 실험에서도 이러한 부분들을 주의하며 진행해 나가야겠다.
  
     
