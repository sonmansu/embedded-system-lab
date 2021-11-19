//�ȵ��ư��³������
//�������
//7�� 12����
//main.c
#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "lcd.h"
#include "touch.h"
#include "misc.h"

volatile uint32_t ADC_Value[1]; //���������� ������ adc��

/* function prototype */
void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configure(void);
void ADC_Configure(void);
void Delay(void);

int color[12] = {WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY};
//---------------------------------------------------------------------------------------------------

//uint16_t lightSenser = 0;
//uint16_t x1 = 0;
//uint16_t y1 = 0;
void RCC_Configure(void)
{
  // TODO: Enable the APB2 peripheral clock using the function
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE); // ADC1, port C RCC ENABLE

  /* Alternate Function IO clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

}

void GPIO_Configure(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* ADC1 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  //@ Set Pin Mode Input Pull-up/down UP
  //@ Set Pin Speed Max : 50MHz
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //@ Set Pin Mode General Output Push-Pull
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//ADC init �Լ�
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
  //  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE); // interrupt enable
  ADC_DMACmd(ADC1, ENABLE); //dma enable
  ADC_Cmd(ADC1, ENABLE); // ADC1 enable
  ADC_ResetCalibration(ADC1);

  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
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

void NVIC_Configure(void) {
  NVIC_InitTypeDef NVIC_InitStructure;
  // TODO: fill the arg you want
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  // ADC1
  //@ NVIC Line ADC1
  NVIC_EnableIRQ(ADC1_2_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
//// ADC�� ���ͷ�Ʈ ���̽��̹Ƿ� �ڵ鷯 �ۼ� �ʿ�
//void ADC1_2_IRQHandler() {
//  if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET){
//    lightSenser = ADC_GetConversionValue(ADC1);
//    ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
//  }
//}

//���� �߰���
void Delay(void) {
  int i;

  for (i = 0; i < 2000000; i++) {}
}

int main(void)
{

  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  DMA_Configure();

  ADC_Configure();
//  DMA_Configure();
  NVIC_Configure();

  LCD_Init();
  Touch_Configuration();
//  Touch_Adjust();
//  int threshold = 700; //��ҿ� 1200~1500 / �÷��� ���߸� 200
  uint32_t threshold = 700; //��ҿ� 1200~1500 / �÷��� ���߸� 200

  LCD_Clear(YELLOW);
  LCD_ShowString(10, 10, "THU_Team07", BLACK, WHITE);
  int adc=0;
  while (1) {
    // TODO: implement
//        Touch_GetXY(&x1, &y1, 1);
//        Convert_Pos(x1, y1, &x1, &y1);
//
//        LCD_ShowNum(50,50, x1, 4, BLACK, WHITE);
//        LCD_ShowNum(50, 70, y1, 4, BLACK, WHITE);
//
//        LCD_ShowNum(60,100, lightSenser, 4, BLACK, WHITE);
//        LCD_DrawCircle(x1, y1, 4);

//    printf("jodo :%d\n", ADC_Value[0]); //�̰� �־ �� �ȵƴ� �ű���
    adc= ADC_Value[0];

    LCD_ShowNum(50, 70, ADC_Value[0], 4, BLUE, YELLOW);
    LCD_ShowNum(50, 90, '0', 4, MAGENTA, GREEN); //'0'�� �ƴ϶� ���ڰ� ����
    LCD_ShowNum(50, 110, adc, 4, BLACK, WHITE);

    if (ADC_Value[0] < threshold) {
      LCD_Clear(GRAY);
      LCD_ShowNum(50, 50, ADC_Value[0], 5, BLACK, GRAY);
    } else {
      LCD_Clear(WHITE);
      LCD_ShowNum(50, 50, ADC_Value[0], 5, BLACK, WHITE);
    }
    Delay();


  }
  return 0;
}
