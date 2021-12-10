#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"

void RCC_Configure(void);
void GPIO_Configure(void);
void TIM2_Configure(void);
void RCC_Configure(void){
      /* Enable the GPIO BUZZER Clock */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
}

void GPIO_Configure(void){
      GPIO_InitTypeDef  GPIO_InitStructure;

      /* Configure the GPIO BUZZER pin */
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure); 
  
}
void TIM2_Configure(void)

{

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	NVIC_InitTypeDef NVIC_InitStructure;



	/* TIM2 clock enable */

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);



	/* Time base configuration */

	TIM_TimeBaseStructure.TIM_Period = 10;  // Overflow Interrupt On 10 usec 타이머주기

	TIM_TimeBaseStructure.TIM_Prescaler = 35; // Timer/Count2 Clock = 36Mhz / (35 + 1) = 1Mhz = 1 usec

	TIM_TimeBaseStructure.TIM_ClockDivision = 0;

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 카운터모드동작

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);



        /* TIM2 counter enable */

        TIM_Cmd(TIM2, ENABLE);



	/* TIM IT enable */

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);



	/* Enable the TIM2 gloabal Interrupt */

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

}



/*

unut : sec				

도 0.030578 	0.015289 	0.007645 	0.003822 	0.001911 	0.000956 	0.000478 

C# 0.028862 	0.014431 	0.007215 	0.003608 	0.001804 	0.000902 	0.000451 

레 0.027242 	0.013621 	0.006810 	0.003405 	0.001703 	0.000851 	0.000426 

D# 0.025713 	0.012856 	0.006428 	0.003214 	0.001607 	0.000804 	0.000402 

미 0.024270 	0.012135 	0.006067 	0.003034 	0.001517 	0.000758 	0.000379 

파 0.022908 	0.011454 	0.005727 	0.002863 	0.001432 	0.000716 	0.000358 

F# 0.021622 	0.010811 	0.005405 	0.002703 	0.001351 	0.000676 	0.000338 

솔 0.020408 	0.010204 	0.005102 	0.002551 	0.001276 	0.000638 	0.000319 

G# 0.019263 	0.009631 	0.004816 	0.002408 	0.001204 	0.000602 	0.000301 

라 0.018182 	0.009091 	0.004545 	0.002273 	0.001136 	0.000568 	0.000284 

A# 0.017161 	0.008581 	0.004290 	0.002145 	0.001073 	0.000536 	0.000268 

시 0.016198 	0.008099 	0.004050 	0.002025 	0.001012 	0.000506 	0.000253 

						

0.000033 	0.000065 	0.000131 	0.000262 	0.000523 	0.001047 	0.002093 

0.000035 	0.000069 	0.000139 	0.000277 	0.000554 	0.001109 	0.002217 

0.000037 	0.000073 	0.000147 	0.000294 	0.000587 	0.001175 	0.002349 

0.000039 	0.000078 	0.000156 	0.000311 	0.000622 	0.001245 	0.002489 

0.000041 	0.000082 	0.000165 	0.000330 	0.000659 	0.001319 	0.002637 

0.000044 	0.000087 	0.000175 	0.000349 	0.000698 	0.001397 	0.002794 

0.000046 	0.000092 	0.000185 	0.000370 	0.000740 	0.001480 	0.002960 

0.000049 	0.000098 	0.000196 	0.000392 	0.000784 	0.001568 	0.003136 

0.000052 	0.000104 	0.000208 	0.000415 	0.000831 	0.001661 	0.003322 

0.000055 	0.000110 	0.000220 	0.000440 	0.000880 	0.001760 	0.003520 

0.000058 	0.000117 	0.000233 	0.000466 	0.000932 	0.001865 	0.003729 

0.000062 	0.000123 	0.000247 	0.000494 	0.000988 	0.001976 	0.003951 

*/



#define MUSIC_REST 0

#define MUSIC_DO 191

#define MUSIC_C_SHARP 180

#define MUSIC_RE 170

#define MUSIC_D_SHARP 161

#define MUSIC_MI 152

#define MUSIC_PA 143

#define MUSIC_F_SHARP 135

#define MUSIC_SOL 128

#define MUSIC_G_SHARP 120

#define MUSIC_RA 114

#define MUSIC_A_SHARP 107

#define MUSIC_SI 101

#define MUSIC_HDO 96





uint32_t Sound = 0;

uint32_t Music = 0;

void TIM2_IRQHandler(void) // 1mS Timer
{

      if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)

      {

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

        Sound++;

        if(Sound >= Music){

            GPIOB->ODR ^= GPIO_Pin_0;

            Sound = 0;

        }

      }
}
void Delay_Us(int n)
{
   for(int j = 0; j <n; j++) {
      continue;
   }
}
int main(void)
{

      void RCC_Configure(void);
      void GPIO_Configure(void);
      void TIM2_Configure(void);

      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_REST;
      Delay_Us(10000);
      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_RA;
      Delay_Us(100000);
      Music = MUSIC_REST;
      Delay_Us(10000);
      Music = MUSIC_RA;
      Delay_Us(100000);
      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_REST;
      Delay_Us(10000);
      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_MI;
      Delay_Us(100000);
      Delay_Us(100000);

      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_REST;
      Delay_Us(10000);
      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_MI;			
      Delay_Us(100000);
      Music = MUSIC_REST;
      Delay_Us(10000);
      Music = MUSIC_MI;
      Delay_Us(100000);
      Music = MUSIC_RE;
      Delay_Us(100000);
      Delay_Us(100000);


      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_REST;
      Delay_Us(10000);
      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_RA;
      Delay_Us(100000);
      Music = MUSIC_REST;
      Delay_Us(10000);
      Music = MUSIC_RA;
      Delay_Us(100000);
      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_REST;
      Delay_Us(10000);
      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_MI;
      Delay_Us(100000);
      Delay_Us(100000);

      Music = MUSIC_SOL;
      Delay_Us(100000);
      Music = MUSIC_MI;
      Delay_Us(100000);
      Music = MUSIC_RE;
      Delay_Us(100000);
      Music = MUSIC_MI;
      Delay_Us(100000);
      Music = MUSIC_DO;
      Delay_Us(100000);
      Delay_Us(100000);
                            
      TIM_Cmd(TIM2, DISABLE);
      GPIOB->BRR = GPIO_Pin_0;	
}
