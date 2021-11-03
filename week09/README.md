# 임베디드 시스템 설계 및 실험 9주차 결과 보고서
7조 이석우, 손수민, 김춘수, 정지용
 
## 1. 실험 목표
1. 블루투스 통신에 필요한 장치들을 납땜을 통해 연결한다.
2. 블루투스 모듈의 각 핀 별 담당하는 기능을 학습하고, 스마트폰과의 통신에 이용한다.

## 2.배경 지식

### Bluetooth 
&nbsp;&nbsp; Bluetooth(블루투스)란 1994년 스웨덴 에릭슨 기업에서 개발한 디지털 통신 기기를 위한 개인 근거리 무선 통신 기술의 표준으로, 데이터 전송 거리는 10m~100m정도이다. 좁은 대역대에 많은 기기가 물려서 통신에 방해가 될 수 있어 frequency hopping기법을 이용하여 간섭 및 fading을 방지한다. Master/slave 모드로 동작하며, master는 검색 및 연결요청 등을 담당하며, slave는 검색 대기 및 연결 대기를 담당한다.   
&nbsp;&nbsp; Master가 주변 slave를 탐색하여 발견하면, slave는 자신의 정보를 master에게 발신한다. Bluetooth 장치 하나당 7개의 장치가 동시에 접속이 가능하다. 그리고 블루투스 연결을 위해서 master가 생성하는 주파수 hopping에 slave가 동기화 되어 있어야 한다. 블루투스는 단거리 통신, 저전력, 높은 신뢰성, 저가의 무선 통신을 구현한다는 장점이 있다. 

<img src = "https://user-images.githubusercontent.com/80534651/140203046-59496ad3-a594-4f87-b2f4-ccaa3e55df04.png" width = "300px">

### Bluetooth Profile 
&nbsp;&nbsp; 블루투스 프로파일이란 보내는 데이터의 종류를 명확하게 정의하기 위한 블루투스 기본 구조 위에 추가로 정의된 프로토콜이다. 블루투스 무선 기술은 다양한 장비에 장착되어 이용되고 있고, 따라서 더 많은 기기와의 호환성문제를 해결하기 위해 기기 종류 간에 프로토콜을 정의한다. 즉, 기기마다 연결 프로파일이 다르기 때문에 이 기기와 연결이 되려면 블루투스 모듈이 이 프로파일 모두를 지원해야 한다.    
&nbsp;&nbsp; 이번 실험에서 사용할 프로파일은 SPP(Serial Port Profile)이다. 이는 시리얼 통신을 이용하여 데이터를 송수신하는 프로파일로 SPP 사용 시에 두 장치는 RX, TX가 유선으로 연결된 것처럼 동작하게 된다. 

### Identifier
고유 식별자로 SSID와 UUID가 있다.
#### SSID (Service Set Identifier)
&nbsp;&nbsp; SSID는 무선랜을 통해 클라이언트가 접속할 때 각 무선랜을 구별하기 위한 32 바이트 길이의 고유 식별자이고, 와이파이의 경우에는 각 와이파이 네트워크를 구별하기 위해 사용된다.  

#### UUID (Universally Unique Identifier)
&nbsp;&nbsp; UUID는 네트워크 상에서 서로 다른 개체들을 구별하기 위한 128비트 고유 식별자로 블루투스에서는 서비스의 종류를 구분하기 위해 사용된다. 

### Bluetooth module (FB755AC)
<img src = "https://user-images.githubusercontent.com/80534651/140200462-bdc9fbcf-f8c6-4973-85fd-31e17450f223.png" width = "500px">

&nbsp;&nbsp; Firmtech사에서 개발한 모듈로, master와 slave 형태의 주종관계로 구성되어 있다. 하나의 모듈에 최대 7개의 slave(장치)가 동시에 접속이 가능하다. 통신 거리는 100m 이고, 인터페이스는 uart이다. AT 명령어를 지원하여 AT 명령어를 이용하여 FB755AC의 제어가 가능하다. 12개의 핀으로 구성되며 각 핀의 설명은 다음과 같다.

- STATUS PIN   
 블루투스 모듈의 상태를 모니터링 하기 위해 사용한다. 블루투스 연결 대기중이거나 연결 시도, 주변 블루투스 장치 검색에는 LOW, HIGH를 반복한다.    
 
- STREAM CONTROL, STREAM_STATUS, MESSAGE_CONTROL, MESSAGE_STATUS   
 1:N 통신을 할 때 연결하며, 1:1 통신 시에는 필요없다.    
 
- CONFIG SELECT   
 블루투스 모듈 설정 시에 사용한다. HIGH를 입력한 채로 전원을 켜면 설정 모드로 진입한다.    

- CONNECT CHECK/DCD   
 설정된 연결 수 만큼 MASTER 연결 시 LOW, 하나라도 해지되면 HIGH로 바뀐다. 

### AT 명령어
&nbsp;&nbsp; 블루투스를 포함하여 여러 모뎀 모듈을 제어하는 데 쓰이는 명령어이다. AT명령어의 AT는 attention의 앞 두글자를 따서 만든 것이고, AT_COMMAND와 같이 적어 이용한다. COMMAND부분에 원하는 명령어를 적으면 된다. 이번 실험에서는 블루투스 모듈이 모바일 디바이스(스마트폰)과 연결 및 검색할 수 있도록 하기 위해 AT+BTSCAN 을 이용한다. 

## 3. 실험 과정
&nbsp;&nbsp; 이번 주 실험은 블루투스 모듈을 사용하여 폰과 보드 그리고, 컴퓨터의 통신을
진행하는 것이다. 세부 실험 내용은 다음과 같다.
#### 3-1) 만능 기판 납땜
보드와 블루투스 모듈을 연결하는 회로를 구성하여 납땜한다. 납땜 과정은 다음과 같다.
1. 보드를 기판에 장착하기 위한 보드 장착부를 납땜한다.
2. 블루투스 모듈의 VCC, GND를 만능 기판의 3V3, GND와 연결한다.
3. 블루투스 모듈의 RX, TX를 보드의 PA2, PA3과 연결한다.
4. 블루투스 모듈의 CONFIG SELECT은 나중에 점프선으로 3V3을 입력해주기 위해 헤더핀과 연결한다.
5. 블루투스 모듈의 STATUS핀과 CONNECT CHECK핀은 연결 상태를 확인하기 위해 LED와 연결한다.

| <img src="https://user-images.githubusercontent.com/80534651/140210403-515fc158-1446-4e04-a202-1aa13b0db604.jpg" width="500px"> | <img src="https://user-images.githubusercontent.com/80534651/140210462-8aac1ab9-56d5-4720-9a5e-e56472f390cb.jpg" width="500px"> | 
|:--:|:--:| 
| 기판 앞면 | 기판 뒷면 |

#### 3-2) PC의 putty 프로그램과 Bluetooth 모듈 간 통신이 가능하도록 펌웨어 작성
#### 3-3) 블루투스 모듈 설정
블루투스 모듈의 CONFIG SELECT 핀에 3.3v를 준 상태에서 보드를 키면 putty에 블루투스 모듈 설정 메뉴가 뜬다.
여기서 device name, Pincode (블루투스 연결 비밀번호), Connection mode 4 salve, Uart Config (9600, 8, n, 1) 을 설정한다.
<img src = "https://user-images.githubusercontent.com/80534651/140230739-7db2388c-7d25-4c99-b608-d456cc2dcf84.png" width = "500px">

설정 후 CONFIG SELECT에 3.3v 입력을 해제하고 보드 전원을 껐다 켜면 AT 명령어 대기 모드로 돌입한다. 이 상태에서 "AT+BTSCAN" 명령어를 입력하여 연결 대기에 들어가면 스마트폰으로 블루투스 연결을 한다. 

<img src = "https://user-images.githubusercontent.com/80534651/140231239-88350fa3-39f2-427e-b804-a030f56b0cdd.png" width = "200px">

#### 3-4) Serial Bluetooth Terminal 어플리케이션을 이용하여 PC의 putty와 통신
putty에 입력한 것이 bluetooth 모듈을 통해 스마트폰의 터미널에 출력되는 것과, 스마트폰의 터미널에 입력한 것이 putty에 출력되는 것을 확인한다.

<img src = "https://user-images.githubusercontent.com/80534651/140231580-12a95983-bef6-4578-933b-799e993016df.jpg" width = "300px">

 
## 4. 코드 설명
&nbsp;&nbsp; 이번 실험에서는 블루투스 통신을 위하여, 지난 주 코드에서 Usart2 부분을
추가적으로 작성해 주었다. Usart1_irqhandler함수에서는 usart1 인터럽트를
통한 동작에 관하여 작성하였고, 데이터 입력 시 usart1을 통하여 보드로 전
송하고, 보드에서 다시 usart2를 통해 휴대용 단말기로 전송하기 때문에 전
주에 비해 추가적인 작성이 필요하였다. Usart2_irqhandler함수에서는 usart2
인터럽트 발생시 작동하는 내용을 작성하였고, 데이터가 입력시 보드를 통해
usart1으로 전송하도록 하였다.

## 5. 실험 결론
&nbsp;&nbsp; 금주 실험에서는 전주 실험에서 사용된 코드에서 블루투스 통신을 위한 usart2 부분을 추가로 작성하였고, 납땜을 통한 보드 모듈 사용이 주를 이루었다. 대부분 조원들이 납땜경험이 적거나, 없었기 때문에 간단한 작업임에도 생각보다 시간이 소비되었다. 실험내용 대로 납땜을 완료한 후 블루투스 모듈을 통한 PC와 스마트폰간의 통신을 이루어냈다.   
&nbsp;&nbsp; 향후, 프로젝트에서 블루투스 통신이 필수적인 만큼 금주 실험 내용을 잘 기억해놓을 필요성이 있을 것 같다.

