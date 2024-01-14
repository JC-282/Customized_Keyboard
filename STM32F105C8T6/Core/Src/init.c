/*
 * init.c
 *
 *  Created on: Feb 8, 2023
 *      Author: cai282
 */
#include "init.h"

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}




void init_tim2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 80-1;
    TIM2->ARR = 100-1;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] = 1<<TIM2_IRQn;
}

void init_usart1() {
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRH &= ~0x00f0;
    GPIOA->CRH |= 0x00a0;
    // GPIOA->AFR[1] |= 0x440;

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    USART1->CR1 &= ~USART_CR1_UE;
    USART1->CR1 &= ~(1<<28);
    USART1->CR1 &= ~(1<<12);
    USART1->CR2 &= ~(3<<12);
    USART1->CR1 &= ~(1<<10);
    USART1->CR1 &= ~(1<<16);
    // USART1->BRR = 0xd05; // 32MHz
    //USART1->BRR = 0xda; // 2.08MHz
    USART1->BRR = 833; // 8MHz
    USART1->CR1 |= 3<<2;
    USART1->CR1 |= USART_CR1_UE;
    while(!(USART1->CR1  & USART_CR1_TE)){ }
}



void init_eeprom(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	// PA5 I2C CLK
	// PA6 I2C DATA
	GPIOA->CRL &= ~(0xff00000);
	GPIOA->CRL |= 0x6600000;

	return;
}



void enable_ports(void)
{
//	 Column output
//
//    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
//    GPIOA->CRL &= ~(0xffffffff);
//    GPIOA->CRL |= (0x66666666);
//    //GPIOA->CRL |= (0x22222222);
//
//    // Row input
//    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
//    GPIOB->CRL &= ~(0xfffff000);
//    GPIOB->CRL |= (0x88888000);
//    GPIOB->CRH &= ~(0xfff);
//    GPIOB->CRH |= (0x888);
//    GPIOB->BSRR = 0x07f8;
//



    // feb 23
	// pb0-1, 3-15 column (input)
	// pa0-4, row (drive row -> output)
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->CRL &= ~(0xFFFFF);
	GPIOA->CRL |= 0x66666;
	//GPIOA->BSRR = 0x1F;

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRL &= ~(0xFFFFF0FF);
    GPIOB->CRL |= 0x88888088;
    GPIOB->CRH &= ~(0xFFFFFFFF);
    GPIOB->CRH |= 0x88888888;
    GPIOB->BSRR = 0xFFFB;

    GPIOA->CRH &= ~(0x00000f00);
    GPIOA->CRH |= 0x00000800;
    GPIOA->BSRR = 0x04000000;



    //mode selection



    /////////////////////////
}
/*
void enable_ports(void)
{
	for(int i = 0; i < 8; i++) { gpio_out(GPIOA, i); }
	for(int i = 3; i < 11; i++) { gpio_in(GPIOB, i); }


}*/

void gpio_in(GPIO_TypeDef *x, int a)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	  GPIO_InitStruct.Pin = 0x1<<a;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = GPIO_PULLUP;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(x, &GPIO_InitStruct);
}

void gpio_out(GPIO_TypeDef *x, int a)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = 0x1<<a;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	//GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(x, &GPIO_InitStruct);
}



void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

}



void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
