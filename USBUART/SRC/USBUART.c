/*
 * USBUART.c
 *
 *  Created on: May 26, 2023
 *      Author: kisha
 */

#include "USBUART.h"

 const char ln[] = "\r\n";
 UART_HandleTypeDef* husbuartx;


 void USBUART_Init(UART_HandleTypeDef* huartx){

	 husbuartx = huartx;
 }


 uint8_t USBUART_println(const char* string,char* buffer){

     strcpy(buffer,string);
	 strcat(buffer,ln);

	 return HAL_UART_Transmit(husbuartx,(uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
 }


  uint8_t USBUART_CSV_println(char* buffer, int n, ...){
  va_list ptr;

  va_start(ptr,n);

  strcpy(buffer,va_arg(ptr,const char*));
  strcat(buffer,",");
  for (int i = 1; i < n-1; i++){


strcat(buffer,va_arg(ptr,const char*));
strcat(buffer,",");

  }
  strcat(buffer,va_arg(ptr,const char*));
  va_end(ptr);
  strcat(buffer,ln);

return HAL_UART_Transmit(husbuartx, (uint8_t*)buffer, strlen(buffer),HAL_MAX_DELAY);
  }


