/*
 * CDCFS.h
 *
 *  Created on: 21 May 2023
 *      Author: kisha
 */

#ifndef INC_CDCFS_H_
#define INC_CDCFS_H_
#include "usbd_cdc_if.h"
#include <stdarg.h>


 uint8_t CDCFS_println(const char* string,char* buffer);
 uint8_t CDCFS_CSV_println(char* buffer, int n, ...);



#endif /* INC_CDCFS_H_ */
