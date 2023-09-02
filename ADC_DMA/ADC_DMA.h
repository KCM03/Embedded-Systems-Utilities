/*
 * ADC_DMA.h
 *
 *  Created on: Aug 15, 2023
 *      Author: kisha
 */

#ifndef INC_ADC_DMA_H_
#define INC_ADC_DMA_H_
#include "main.h"

#if defined( __ICCARM__ )
  #define DMA_BUFFER \                          //Data Buffer MUST be  of this type!
      _Pragma("location=\".dma_buffer\"")
#else
  #define DMA_BUFFER \
      __attribute__((section(".dma_buffer")))
#endif

void MPU_Config(void);
void DMA_Cache_Init(void);

#endif /* INC_ADC_DMA_H_ */
