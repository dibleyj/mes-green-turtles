/*
 * application.c
 *
 *  Created on: Nov 1, 2023
 *      Author: jamesdibley
 */
#include "main.h"
#include <stdbool.h>

bool gButtonPressed = false;

void application_init()
{

}

void application_main()
{
	if (gButtonPressed)
	{
		HAL_GPIO_TogglePin(GreenLED_GPIO_Port, GreenLED_Pin);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == BlueButton_Pin)
	{
		gButtonPressed = !(gButtonPressed);
	}
	else
	{
		__NOP();
	}
}
