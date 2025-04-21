/*
 * ADBMS6830_Reg.h
 *
 *  Created on: 6 Dec 2023
 *      Author: Kishan Maharaj
 */

#ifndef INC_ADBMS6830_REG_H_
#define INC_ADBMS6830_REG_H_


enum CFGRA3{ //GPIO REG1

	CFGRA3_GPIO1 = 0x01,   //bit 1
	CFGRA3_GPIO2 = 0x02,   //bit 2
	CFGRA3_GPIO3 = 0x04,   //bit 3
	CFGRA3_GPIO4 = 0x08,   //bit 4
	CFGRA3_GPIO5 = 0x10,   //bit 5
	CFGRA3_GPIO6 = 0x20,   //bit 6
	CFGRA3_GPIO7 = 0x40,   //bit 7
	CFGRA3_GPIO8 = 0x80,   //bit 8
	CFRGA3_GPIOALL = 0xFF,

//-------------GPIO REG2
	CFGRA4_GPIO9 = 0x100, //bit 9  (next register)
    CFGRA4_GPIO10 = 0x200, //bit 10 (next register)
    CFRGA4_GPIOALL = 0x300,

	CFRGA_GPIOALL = 0x3FF

};




enum CFGRB4{ //DCC REG1

	CFGRB4_DCC1 = 0x01,
	CFGRB4_DCC2 = 0x02,
	CFGRB4_DCC3 = 0x04,
	CFGRB4_DCC4 = 0x08,
	CFGRB4_DCC5 = 0x10,
	CFGRB4_DCC6 = 0x20,
	CFGRB4_DCC7 = 0x40,
	CFGRB4_DCC8 = 0x80,

};

enum CFGRB5{ //DCC REG2

	CFGRB5_DCC9  = 0x01,
	CFGRB5_DCC10 = 0x02,
	CFGRB5_DCC11 = 0x04,
	CFGRB5_DCC12 = 0x08,
	CFGRB5_DCC13 = 0x10,
	CFGRB5_DCC14 = 0x20,
	CFGRB5_DCC15 = 0x40,
	CFGRB5_DCC16 = 0x80,

};

enum CFGRB3{ //DCTO REG

	CFGRB3_DCTO0 = 0x01,
	CFGRB3_DCTO1 = 0x02,
	CFGRB3_DCTO2 = 0x04,
	CFGRB3_DCTO3 = 0x08,
	CFGRB3_DCTO4= 0x10,
	CFGRB3_DCTO5 = 0x20,
	CFGRB3_DTRNG = 0x40,
	CFGRB3_DTMEN = 0x80,

};


#endif /* INC_ADBMS6830_REG_H_ */
