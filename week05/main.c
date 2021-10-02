//embedded system lab group7 5th week mission
#include "stm32f10x.h"

#define RCC_APB2ENR *(volatile unsigned int *)0x40021018

//릴레이모듈
#define GPIOC_CRL *(volatile unsigned int *)0x40011000 //Joystick Configuration Regiester Low
#define GPIOC_CRH *(volatile unsigned int *)0x40011004 //Joystick Configuration Regiester Low
#define GPIOC_IDR *(volatile unsigned int *)0x40011008 //Joystick Input Data Register
#define GPIOC_BSRR *(volatile unsigned int *)0x40011010 //BIT SET, RESET WRITE

//LED, 버튼
#define GPIOD_CRL *(volatile unsigned int *)0x40011400 //LED Configuration Regiester Low
#define GPIOD_CRH *(volatile unsigned int *)0x40011404 //LED Configuration Regiester High
#define GPIOD_BSRR *(volatile unsigned int *)0x40011410 //LED Bit Set/Reset Register
#define GPIOD_IDR *(volatile unsigned int *)0X40011408 //LED LOW, HIGH READ

void init() {
  RCC_APB2ENR = 0x30; // Port C,D Clock Enable
 
  // LED PD7
  GPIOD_CRL = 0x44444444; // reset
  GPIOD_CRL = 0x30000000; // led PD 7 output push-pull
 
  //버튼: PD11(S1), PD12(S2)
  GPIOD_CRH = 0x44444444; // reset
  GPIOD_CRH = 0x00088000; // 버튼 configuration input 
  GPIOD_IDR = 0x00000000; // 버튼 INPUT DATA REGISTER RESET
  GPIOD_BSRR = 0x00000000; // 버튼 bsrr 리셋
 
  //릴레이 모듈: PC8
  GPIOC_CRH = 0x44444444; // reset
  GPIOC_CRH = 0x00000003; // 릴레이 PC8 컨피규어레이션, output  push-pull
  GPIOC_BSRR = 0x00000000; // 릴레이 bsrr 리셋
}

void led7on() { 
  GPIOD_BSRR = 0x00000080;
}
void led7off() {   
  GPIOD_BSRR = 0x00800000; // led 7 reset
}
void delay(void) {
  int i=0;
  for(i=0;i<1000000;i++)   {}  //empty Loop
}

int main(void)
{
  init();
 
  while(1){
    if (~GPIOD_IDR & 0x00000800) { // pd 11번 버튼 눌렸을때 모터 작동
      GPIOC_BSRR = 0x0100; // pc8 set
      delay();
      GPIOC_BSRR = 0x01000000; // pc8 reset
    } else if (~GPIOD_IDR & 0x00001000) { //pd 12번 버튼 눌렸을 때  led 켬
      led7on();
      delay();
      led7off();
    } 
  }
} 