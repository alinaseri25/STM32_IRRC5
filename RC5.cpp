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

	/* The START Sequence begin here
	* there will be a pulse of 9ms LOW and
	* than 4.5 ms space (HIGH)
	*/
	IRTimeOut = HAL_GetTick() + 9;
	while (!(HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)))  // wait for the pin to go high.. 9ms LOW
	{
		if(HAL_GetTick() >= IRTimeOut)
		{
			IRState = HAL_TIMEOUT;
			return;
		}
	}
	
	IRTimeOut = HAL_GetTick() + 5;
	while ((HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)))  // wait for the pin to go low.. 4.5ms HIGH
	{
		if(HAL_GetTick() >= IRTimeOut)
		{
			IRState = HAL_TIMEOUT;
			return;
		}
	}
	

	/* START of FRAME ends here*/

	/* DATA Reception
	* We are only going to check the SPACE after 562.5us pulse
	* if the space is 562.5us, the bit indicates '0'
	* if the space is around 1.6ms, the bit is '1'
	*/
	
	for (int i=0; i<32; i++)
	{
		count=0;

		while (!(HAL_GPIO_ReadPin (GPIOx, GPIO_Pin))); // wait for pin to go high.. this is 562.5us LOW

		while ((HAL_GPIO_ReadPin (GPIOx, GPIO_Pin)))  // count the space length while the pin is high
		{
			count++;
			DWT_Delay_us(100);
		}

		if (count > 12) // if the space is more than 1.2 ms
		{
			CurData |= (1UL << (31-i));   // write 1
		}

		else CurData &= ~(1UL << (31-i));  // write 0
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
