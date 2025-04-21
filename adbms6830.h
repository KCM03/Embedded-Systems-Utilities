/*******************************************************************************
 * Header File containing ADBMS6830 Command Codes
 *
 ******************************************************************************/

/**
 * @file    ADBMS6830.h
 * @brief   Header File containing ADBMS6830 Command Codes
 */

#ifndef _adbms6830_H_
#define _adbms6830_H_

#define WRCFGA		0x0001
#define RDCVA		0x0004
#define RDCVB		0x0006
#define RDCVC		0x0008
#define RDCVD		0x000A
#define RDCVE		0x0009
#define RDCVF		0x000B

#define RDSVA		0x0003
#define RDSVB		0x0005
#define RDSVC		0x0007
#define RDSVD		0x000D
#define RDSVE		0x000E
#define RDSVF		0x000F

#define SRST		0x0027

#define RDACA		0x0044
#define RDACB		0x0046
#define RDACC		0x0048
#define RDACD		0x004A
#define RDACE		0x0049
#define RDACF		0x004B

#define RDFCA		0x0012
#define RDFCB		0x0013
#define RDFCC		0x0014
#define RDFCD		0x0015
#define RDFCE		0x0016
#define RDFCF		0x0017

#define CLRAUX		0x0712


#define ADAX_ALL	0x0410				// convert all GPIO + ITMP and VREF2

// LPCM Commands
#define CMDIS		0x0040				// Disable LPCM
#define CMEN		0x0041				// Enable LPCM
#define CMHB		0x0043				// LPCM Heartbeat
#define WRCMCFG		0x0058				// Write LPCM Config Reg
#define RDCMCFG		0x0059				// Read LPCM Config Reg
#define WRCMCELLT	0x005A				// Write LPCM Cell OV/UV/delta
#define RDCMCELLT	0x005B				// Read LPCM Cell OV/UV/delta
#define WRCMGPIOT	0x005C				// Write LPCM GPIO threshold
#define RDCMGPIOT	0x005D				// Read LPCM GPIO threshold
#define CLRCMFLAG	0x005E				// Clear LPCM flags
#define RDCMFLAG	0x005F				// Read LPCM flags

// get serial ID
#define RDSID		0x002C				// get serial ID

// AUX register commands
#define RDAUXA		0x0019				// GPIO1 - GPIO3
#define RDAUXB		0x001A				// GPIO4 - GPIO6
#define RDAUXC		0x001B				// GPIO7 - GPIO9
#define RDAUXD		0x001F				// GPIO10


#define RDCFGA		0x0002
#define CLOVUV		0x0715
#define CLRFLAG		0x0717
#define RDCFGB		0x0026
#define RSTCC		0x002E
#define RDSTATA		0x0030
#define RDSTATB		0x0012
#define RDSTATC		0x0032				// ERR not set
#define RDSTATD		0x0033
#define WRCFGB		0x0024
#define RDSID		0x002C
#define MUTE		0x0028
#define UNMUTE		0x0029
//#define ADCV		0x02E0				// with RD=0 DCP=0 CONT=1 OW=00
#define ADCV		0x03E0				// with RD=1 DCP=0 CONT=1 OW=00 **DCP only stops PWM discharge, not DCC discharge**
//#define ADCV		0x03F0				// with RD=1 DCP=1 CONT=1 OW=00 **DCP only stops PWM discharge, not DCC discharge**
//#define ADCV		0x0260				// with RD=0 DCP=0 CONT=0 OW=00

//#define ADCV		0x0360				// with RD=1 DCP=0 CONT=0 OW=00
//#define ADCV		0x0370				// with RD=1 DCP=1 CONT=0 OW=00

#define ADSV		0x0168				// with DCP=0 CONT=0 OW=00
#define MUTE		0x0028
#define UNMUTE		0x0029
#define SRST		0x0027

#define SNAP		0x002D
#define UNSNAP		0x002F

#define WRPWMA		0x0020
#define RDPWMA		0x0022
#define WRPWMB		0x0021
#define RDPWMB		0x0023

#define WRCOMM		0x0721
#define RDCOMM		0x0722
#define STCOMM		0x0723



//check all of these!!!!!!!!!!!!!!!!!

// ADD CONFIG COMMANDS?
void SPI_START();
void SPI_END();
void delay_ms(uint32_t time);   //DONE							// delay in milliseconds
void gen_crc_15(void);
uint16_t get_cmd_pec(uint8_t* data, uint8_t len);
void gen_crc_10(void);
uint16_t get_data_pec(uint8_t* data, uint8_t rx, uint8_t len);
void load_array(uint8_t* cmd_array, uint16_t command); //DONE
void get_spare_voltages(uint32_t* spare_volts);
void get_gpio_voltages(uint32_t* gpio_volts);
void get_aux_voltages(uint32_t* aux_volts);
void Read_CellVoltage_Reg_ADBMS8630(uint8_t reg,uint8_t total_ic,uint8_t *data);
void Read_AUX_Reg_ADBMS6830(uint8_t reg, uint8_t total_ic, uint8_t *data);
int8_t Read_ADBMS8630(uint8_t total_ic,uint8_t tx_cmd[2],uint8_t *rx_data);
void Poll_ADBMS6830(uint8_t tx_cmd[2]); //DONE
void write_ADBMS8630(uint8_t total_ic, uint8_t tx_cmd[2], uint8_t data[]); //DONE
void stcomm_send(uint8_t byte_number); //DONE , I2C & SPI ARE UNUSED
void isospi_wake(void); //DONE
void set_pwm(uint16_t data);
void clear_flags(void);
void clear_cmflags(void);


void wakeup_idle();
void wakeup_sleep();
uint8_t Read_CellVoltage_ADBMS8630(uint8_t reg, uint8_t total_ic, uint16_t cell_codes[][16]);
void Read_CellVoltage_Reg_ADBMS8630(uint8_t reg, uint8_t total_ic, uint8_t *data);
int8_t Read_AUX_ADBMS6830(uint8_t reg,  uint8_t total_ic,  uint16_t aux_codes[][10]);

void Read_AUX_Reg_ADBMS6830(uint8_t reg, uint8_t total_ic, uint8_t *data);
void WRCFGA_ADBMS6830(uint8_t total_ic, uint8_t config_data[][6]);
void WRCFGB_ADBMS6830(uint8_t total_ic, uint8_t config_data[][6]);
void Start_Cell_ADC_adbms6830();
void Start_AUX_ADC_adbms6830();


// NEED TO FINISH FUNCTION PROTOTPYES HERE!!!!!!!!!!!!!!!!

//LOG:





#endif /* _adbms6830_H_ */
