/*
 * USBUART.h
 *
 *  Created on: May 26, 2023
 *      Author: kisha
 */

#ifndef INC_USBUART_H_
#define INC_USBUART_H_

#include "main.h"
#include "string.h"
#include <stdarg.h>

void USBUART_Init(UART_HandleTypeDef* huartx);
uint8_t USBUART_println(const char* string,char* buffer);
 uint8_t USBUART_CSV_println(char* buffer, int n, ...);

#endif /* INC_USBUART_H_ */
