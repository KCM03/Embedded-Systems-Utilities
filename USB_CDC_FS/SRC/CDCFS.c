/*
 * CDCFS.c
 *
 *  Created on: 21 May 2023
 *      Author: kisha
 */

#include "CDCFS.h"

 const char ln[] = "\r\n";

 uint8_t CDCFS_println(const char* string,char* buffer){

     strcpy(buffer,string);
	 strcpy(buffer,strcat(buffer,ln));

	 return CDC_Transmit_FS((uint8_t*)buffer,strlen(buffer));
 }


  uint8_t CDCFS_CSV_println(char* buffer, int n, ...){
  va_list ptr;

  va_start(ptr,n);

  for (int i = 0; i < n-1; i++){


  strcpy(buffer,strcat(buffer,va_arg(ptr,const char*)));
  strcpy(buffer,strcat(buffer,","));

  }
  strcpy(buffer,strcat(buffer,va_arg(ptr,const char*)));
  strcpy(buffer,strcat(buffer,ln));
return CDC_Transmit_FS((uint8_t*)buffer,strlen(buffer));
  }


