/*
 * MAX5216.c
 *
 *  Created on: Sep 7, 2023
 *      Author: kisha
 */


#include "MAX5216.h"

#define  voltage_pos 21//bits.
static  SPI_HandleTypeDef * hSPI;
//static GPIO_TypeDef* CS_Port;
//static uint16_t CS_Pin_Name;
static uint8_t* SPI_Buffer;




static uint8_t* DAC_Write(uint8_t* buffer, uint16_t voltage) {



	uint32_t Bytes_3 = DAC_WRITE;

	Bytes_3 |= voltage << (voltage_pos - 15);


	buffer[0] = Bytes_3 >> 16;
	buffer[1] = Bytes_3 >> 8;
	buffer[2] = Bytes_3;
	return buffer;
}






static uint8_t* DAC_Power_On(uint8_t* buffer)
{
	uint32_t Bytes_3 = DAC_POWER_ON;




	buffer[0] = Bytes_3 >> 16;
	buffer[1] = Bytes_3 >> 8;
	buffer[2] = Bytes_3;
	return buffer;
}

static uint8_t* DAC_Power_Off(uint8_t* buffer) {

	uint32_t Bytes_3 = DAC_POWER_OFF;



	buffer[0] = Bytes_3 >> 16;
	buffer[1] = Bytes_3 >> 8;
	buffer[2] = Bytes_3;
	return buffer;


}

void DAC_Init_SPI(SPI_HandleTypeDef* handle,uint8_t* buffer){



   hSPI = handle;
   SPI_Buffer = buffer;
 //  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);

}

HAL_StatusTypeDef DAC_Write_SPI(uint16_t voltage){


    DAC_Write(SPI_Buffer, voltage);

   // HAL_GPIO_WritePin(CS_Port, CS_Pin_Name, GPIO_PIN_RESET);
   return  HAL_SPI_Transmit_DMA(hSPI, SPI_Buffer, 3);



}
HAL_StatusTypeDef DAC_Power_On_SPI(){





	    DAC_Power_On(SPI_Buffer);

	  //  HAL_GPIO_WritePin(CS_Port, CS_Pin_Name, GPIO_PIN_RESET);
	   return  HAL_SPI_Transmit_DMA(hSPI, SPI_Buffer, 3);



}
HAL_StatusTypeDef DAC_Power_Off_SPI(){



	    DAC_Power_Off(SPI_Buffer);
	 //   HAL_GPIO_WritePin(CS_Port, CS_Pin_Name, GPIO_PIN_RESET);
	   return  HAL_SPI_Transmit_DMA(hSPI, SPI_Buffer, 3);


}

