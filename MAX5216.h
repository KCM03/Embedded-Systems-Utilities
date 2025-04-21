/*
 * MAX5216.h
 *
 *  Created on: Sep 7, 2023
 *      Author: kisha
 */

#ifndef INC_MAX5216_H_
#define INC_MAX5216_H_
#define DAC_POWER_ON 0x800000
#define DAC_POWER_OFF 0x8C0000
#define DAC_WRITE 0x400000
#include <stdio.h>
#include "stm32h7xx_hal.h"


//volatile uint8_t rx_ready = 1;
//REMEMBER TO DISABLE CS PIN AFTER CALLBACK!!!!
void DAC_Init_SPI(SPI_HandleTypeDef* handle,uint8_t* buffer);
HAL_StatusTypeDef DAC_Write_SPI(uint16_t voltage);
HAL_StatusTypeDef DAC_Power_On_SPI();
HAL_StatusTypeDef DAC_Power_Off_SPI();

#endif /* INC_MAX5216_H_ */
