#ifndef RC5_H
#define RC5_H

#include "stm32f1xx_hal.h"
#include "dwt_stm32_delay.h"

class IRRC5
{
public:
	IRRC5(GPIO_TypeDef *_GPIOx,uint16_t _GPIO_Pin);
	void WaitForRead(void);
	uint32_t GetData(void);
	HAL_StatusTypeDef IRStatus(void);
private:
	void set_gpio_input(void);

	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_StatusTypeDef IRState;
	uint16_t GPIO_Pin;
	GPIO_TypeDef *GPIOx;
	uint32_t Data;
};

#endif
