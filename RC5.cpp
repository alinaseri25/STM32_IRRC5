#include "RC5.h"

IRRC5::IRRC5(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin)
{
	GPIOx = _GPIOx;
	GPIO_Pin = _GPIO_Pin;
	set_gpio_input();
	Data = 0;
	IRState = HAL_OK;
}

void IRRC5::WaitForRead(void)
{
	IRState = HAL_BUSY;
	uint32_t CurData = 0,IRTimeOut;
	uint8_t count;

	IRTimeOut = HAL_GetTick() + 9;
	while (!(HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)))
	{
		if(HAL_GetTick() >= IRTimeOut)
		{
			IRState = HAL_TIMEOUT;
			return;
		}
	}
	
	IRTimeOut = HAL_GetTick() + 5;
	while ((HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)))
	{
		if(HAL_GetTick() >= IRTimeOut)
		{
			IRState = HAL_TIMEOUT;
			return;
		}
	}
	
	for (int i=0; i<32; i++)
	{
		count=0;

		while (!(HAL_GPIO_ReadPin (GPIOx, GPIO_Pin))); 

		while ((HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)))  
		{
			count++;
			DWT_Delay_us(100);
		}

		if (count > 12) 
		{
			CurData |= (1UL << (31-i));
		}

		else CurData &= ~(1UL << (31-i));
	}
	Data = CurData;
	IRState = HAL_OK;
}

uint32_t IRRC5::GetData(void)
{
	return Data;
}

HAL_StatusTypeDef IRRC5::IRStatus(void)
{
	return IRState;
}

void IRRC5::set_gpio_input(void)
{
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx,&GPIO_InitStruct);
}
