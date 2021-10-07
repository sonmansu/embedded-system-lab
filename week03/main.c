//embedded system lab group7 3rd week mission
#include "stm32f10x.h"

#define RCC_APB2ENR *(volatile unsigned int *)0x40021018

#define GPIOC_CRL *(volatile unsigned int *)0x40011000 //Joystick Configuration Regiester Low
#define GPIOC_IDR *(volatile unsigned int *)0x40011008 //Joystick Input Data Register

#define GPIOD_CRL *(volatile unsigned int *)0x40011400 //LED Configuration Regiester Low
#define GPIOD_BSRR *(volatile unsigned int *)0x40011410 //LED Bit Set/Reset Register

void init() {
  RCC_APB2ENR = 0x30; // Port C,D Clock Enable
  
  GPIOC_CRL = 0x44444444; // reset
  GPIOC_CRL = 0x00888800; // joystick PC 2,3,4,5 input mode 
  GPIOC_IDR = 0x00000000; // joystick Input Data Register reset 
  
  GPIOD_CRL = 0x44444444; // reset
  GPIOD_CRL = 0x30033300; // led PD 2,3,4,7 output push-pull 
}

void up_led2on() { //only pd 2 led on
  GPIOD_BSRR = 0x00000004; // pin 2 set 
}
void down_led3on() { //only pd 3 led on
  GPIOD_BSRR = 0x00000008; 
}
void left_led4on() { //only pd 4 led on 
  GPIOD_BSRR = 0x00000010; 
}
void right_led7on() { //only pd 7 led on
  GPIOD_BSRR = 0x00000080; 
}
void off() {    //turn off all LEDs
  GPIOD_BSRR = 0x009C0000; // 2,3,4,7 reset 
}
int main(void)
{
  init();
  
  while(1){
    if(~GPIOC_IDR & 0x4){    //0100
      down_led3on();           //down 하면 PD3 LED 가 켜짐
    }
    else if(~GPIOC_IDR& 0x20){  //0010 0000 
      up_led2on();              //up 하면 PD2 LED 가 켜짐
    }
    else if(~GPIOC_IDR & 0x8) {  //0000 1000
      left_led4on();             //left 하면 PD4 LED 가 켜짐
    }
    else if(~GPIOC_IDR & 0x10){  //0001 0000
      right_led7on();            //right 하면 PD7 LED 가 켜짐
    }
    else off();
  }
}
