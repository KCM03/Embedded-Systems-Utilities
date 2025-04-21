#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "stm32_utils.h"
#include "control.h"
#include "adbms6830.h"



uint16_t pec10Table[256];
uint16_t pec15Table[256];
uint8_t tmr_flag;
uint8_t cmd_count = 0;


//DO ENUM DEFINES, CHECK IF THESE ARE ALL THE FUNCTIONS YOU NEED!
#define ISOSPI_REVERSE False
extern SPI_HandleTypeDef SPI_HANDLE;


void wakeup_idle()
{
	  OUTPUT_RESET(OUT_NSS);
	  delayu(10); //Guarantees the isoSPI will be in ready mode
	  OUTPUT_SET(OUT_NSS);
}


void wakeup_sleep()
{
  OUTPUT_RESET(OUT_NSS);
  HAL_Delay(1); // Guarantees the LTC6804 will be in standby
  OUTPUT_SET(OUT_NSS);
}


int8_t parse_ADBMS6830_cv(uint8_t current_ic, // Current IC
					uint8_t cell_reg,  // Type of register
					uint8_t cell_data[], // Unparsed data
					uint16_t *cell_codes, // Parsed data
					uint8_t *ic_pec // PEC error
					)
{
	const uint8_t BYT_IN_REG = 6;
	const uint8_t CELL_IN_REG = 3;
	const uint8_t  NUM_RX_BYT = 8;
	int8_t pec_error = 0;
	uint16_t parsed_cell;
	uint16_t received_pec;
	uint16_t data_pec;
	uint8_t data_counter = current_ic*NUM_RX_BYT; //data counter


	for (uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++) // This loop parses the read back data into the register codes, it
	{																		// loops once for each of the 3 codes in the register

		parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);//Each code is received as two bytes and is combined to
																				   // create the parsed code
		cell_codes[current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;

		data_counter = data_counter + 2;                       //Because the codes are two bytes, the data counter
															  //must increment by two for each parsed code
	}
	received_pec = (cell_data[data_counter] << 8) | cell_data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																			   //after the 6 cell voltage data bytes
	 data_pec = get_cmd_pec(&cell_data[(current_ic) * NUM_RX_BYT], BYT_IN_REG);

	if (received_pec != data_pec)
	{
		pec_error = 1;                             //The pec_error variable is simply set negative if any PEC errors
		ic_pec[cell_reg-1]=1;
	}
	else
	{
		ic_pec[cell_reg-1]=0;
	}
	data_counter=data_counter+2;

	return(pec_error);
}

void SPI_START(){
	OUTPUT_RESET(OUT_NSS);
	  HAL_TIM_Base_Stop_IT(&SOFTCLK_TIMER_TYPE);
	  //HAL_NVIC_DisableIRQ(EXTI4_IRQn);
}
void SPI_END(){
	//HAL_NVIC_EnableIRQ(EXTI4_IRQn);
	  HAL_TIM_Base_Start_IT(&SOFTCLK_TIMER_TYPE);
	  OUTPUT_SET(OUT_NSS);
}



void delay_ms(uint32_t time)
{
	delayu(time*1000);

}


void gen_crc_15(void)                          //for global pec15 table
{


    uint16_t CRC15_POLY = 0x4599;
    uint16_t i, count, remainder;
    remainder = 0;										// this is the latest crc value
    for (i = 0; i < 256; i++)
    {
        /* The following lines align the MSB of the current CRC with the MSB of the polynomial. We
         * could use the MSB of the full polynomial (0xC599), but the MSBs of the data and the polynomial
         * are both 1, so they will XOR to zero, thus we can ignore both MSBs. Therefore, we
         * ignore the MSB of the full polynomial (to create a new polynomial = 0x4599). Thus we now
         * need to check the MSB - 1 (0x4000) and if it is 1, then we rotate it into the place
         * of the MSB. The MSB of the data drops off the left hand side and we are now XORing the
         * polynomial and the data, without their original MSBs */

        remainder = i << 7;
        for (count = 0; count < 8; count++)
        {
            if (remainder & 0x4000)
            {
                remainder = (remainder << 1);
                remainder = (remainder ^ CRC15_POLY);
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
        pec15Table[i] = (remainder & 0xFFFF);
    }
}


uint16_t get_cmd_pec(uint8_t* data, uint8_t len)
{



    uint8_t i;
    uint16_t remainder, address;
    remainder = 16; 									// PEC seed

    for (i = 0; i < len; i++)
    {
        address = ((remainder >> 7) ^ data[i]) & 0xFF; 	// calculate PEC table address
        remainder = (remainder << 8) ^ pec15Table[address];
    }
    return (remainder * 2); 							// CRC15 has LSB = 0 so the final value must be multiplied by 2
}

void gen_crc_10(void)
{

    uint16_t CRC10_POLY = 0x8F;
    uint16_t i, count, remainder;

    for (i = 0; i < 256; i++)
    {
        remainder = i << 2;
        for (count = 0; count < 8; count++)
        {
            if (remainder & 0x200)
            {
                remainder = (remainder << 1);
                remainder = (remainder ^ CRC10_POLY);
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
        pec10Table[i] = (remainder & 0x3FF);
    }
}


uint16_t get_data_pec(uint8_t* data, uint8_t rx, uint8_t len)
{
    /*
     * pec10Table[256] is global
     * get this code from the ADBMS6815 datasheet
     *
     * Set RX when calculating PEC on data being received from ADBMS6830
     * Clear RX when calculating PEC on data being written to ADBMS6830
     *
     * The data PEC is only calculated on the data, NOT the data and command count. The datasheet is
     * misleading on this
     */

    uint8_t i;											// byte index
    uint8_t x;											// bit index
    uint16_t remainder, address;
    uint16_t CRC10_POLY = 0x8F;
    remainder = 16; 									// PEC seed

    for (i = 0; i < len; i++)
    {
        address = ((remainder >> 2) ^ data[i]) & 0xFF; 	// calculate PEC table address
        remainder = (remainder << 8) ^ pec10Table[address];
    }

    // if array is from received buffer add command counter to CRC calculation
    if (rx)
    {
        remainder ^= (uint16_t)(((uint16_t)data[len] & (uint8_t)0xFC) << 2);
    }
    // perform modulo 2 division, a bit at a time, on rest of bits
    for (x = 0; x < 6; x++)
    {
        // try to divide by current data bit
        if ((remainder & 0x200) > 0)
        {
            remainder = remainder << 1;
            remainder = remainder ^ CRC10_POLY;
        }
        else
        {
            remainder = remainder << 1;
        }
    }
    return (remainder & 0x3FF);
}




void load_array(uint8_t* array, uint16_t data)
{
    array[0] = (uint8_t)((data >> 8) & 0xFF);			// cmd_array must contain MS byte
    array[1] = (uint8_t)(data & 0xFF);					// cmd_array must contain LS byte
}



// ****************** Read Command + PEC ***************************************





void get_spare_voltages(uint32_t* spare_volts)   //gets S-ADC voltages
{
    uint8_t		count;									// counter
    uint8_t		offset;									// address offset
    uint8_t 	rx_data[8];								// RX data from ADBMS6830. 6 regs + 2 pecs

    // read cell voltage registers
    for (offset = 0; offset < 3; offset++)
    {
        // load cell voltages SV1 - SV9 via RDSVA, RDSVB, RDSVC
        read_cmd(RDSVA + (2 * offset), rx_data);

        for (count = 0; count < 3; count++)
        {
            // need to unload the array before we ask for the next 6 bytes
            spare_volts[(3 * offset) + count] = 0;
            spare_volts[(3 * offset) + count] = rx_data[2 * count] | (rx_data[(2 * count) + 1] << 8);
        }
    }

    for (offset = 0; offset < 3; offset++)
    {
        // load cell voltages SV10 - SV16 via RDSVD, RDSVE, RDSVF
        read_cmd((RDSVD + offset), rx_data);
        for (count = 0; count < 3; count++)
        {
            // need to unload the array before we ask for the next 6 bytes
            spare_volts[(3 * offset) + count + 9] = 0;
            spare_volts[(3 * offset) + count + 9] = rx_data[2 * count] | (rx_data[(2 * count) + 1] << 8);
        }
    }

    // sort out 2's complement    2^n - number
    for (count = 0; count < 16; count++)
    {
        if (spare_volts[count] > 32767)
        {
            spare_volts[count] = (65536 - spare_volts[count]);
            spare_volts[count] = (spare_volts[count] * 15) / 100;				// convert to mV
            spare_volts[count] = 1500 - spare_volts[count];
        }
        else
        {
            spare_volts[count] = (spare_volts[count] * 15) / 100;				// convert to mV
            spare_volts[count] = spare_volts[count] + 1500;
        }
    }

    // if cell voltage is not between 1V - 5V, set to 0V (for disconnected cells)
    for (count = 0; count < 16; count++)
    {
        if ((spare_volts[count] > 5000) || (spare_volts[count] < 1000))
        {
            spare_volts[count] = 0;
        }
    }

    // load minimum of all cells into spare_volts[16]
    spare_volts[16] = spare_volts[0];
    for (count = 1; count < 16; count++)
    {
        // exclude disconnected cells
        if ((spare_volts[count] < spare_volts[16]) && (spare_volts[count] != 0))
        {
            spare_volts[16] = spare_volts[count];
        }
    }
}


// ****************** Read AUX Reg *********************************************
void get_aux_voltages(uint32_t* aux_volts)
{
    uint8_t		count;									// counter
    uint8_t		offset;									// address offset
    uint8_t 	rx_data[8];								// RX data from ADBMS6830. 6 regs + 2 pecs

    // read cell voltage registers
    for (offset = 0; offset < 2; offset++)
    {
        // load GPIO values G1 - G9 (RDAUXA - RDAUXC)
        read_cmd((RDSTATA + offset), rx_data);

        for (count = 0; count < 3; count++)
        {
            // need to unload the array before we ask for the next 6 bytes
            aux_volts[(3 * offset) + count] = 0;
            aux_volts[(3 * offset) + count] = rx_data[2 * count] | (rx_data[(2 * count) + 1] << 8);
        }
    }

    // process VREF2
    aux_volts[0] = (aux_volts[0] * 15) / 100;				// convert to mV
    aux_volts[0] = (aux_volts[0]) + 1500;

    // process ITMP
    aux_volts[1] = (aux_volts[1] * 15) / 100;				// convert to mV
    aux_volts[1] = (aux_volts[1]) + 1500;
    aux_volts[1] = (aux_volts[1] * 10) / 75;
    aux_volts[1] = aux_volts[1] - 273;

    // process VD
    aux_volts[3] = (aux_volts[3] * 15) / 100;				// convert to mV
    aux_volts[3] = (aux_volts[3]) + 1500;

    // process VA
    aux_volts[4] = (aux_volts[4] * 15) / 100;				// convert to mV
    aux_volts[4] = (aux_volts[4]) + 1500;
}




void stcomm_send(uint8_t byte_number)					// send STCOMM + 24 clock cycles per byte
{
    uint16_t	data;
    uint8_t		cmd[2];									// 2 byte command
    uint8_t		cmd_pec[2];								// 2 byte command pec
    uint8_t		nothing[1];								// dummy array
    uint8_t		count;
  //  uint8_t tmp_tx_buffer[2] = {0};
    nothing[0] = 0;

    isospi_wake();										// wake iso-SPI

    load_array(cmd, STCOMM);							// put command into 2 byte array, cmd[]
    data = get_cmd_pec(cmd, 2);							// get command PEC
    load_array(cmd_pec, data);							// put command pec into 2 byte array, cmd_pec[]

    // Send the command - 2 bytes
   /* req.len = 2;									// send 2 bytes of command
    req.tx_data = cmd;
    req.rx_data = NULL;
    req.deass = 0;									// set to 0 to stop CS line rising
    SPIM_Trans(MXC_SPIM1, &req);*/

    SPI_START();
    SPI_(HAL_SPI_Transmit(&SPI_HANDLE,cmd, 2, 1000));
    SPI_END();

    // Send the cmd pec - 2 bytes
  /*  req.tx_data = cmd_pec;
    req.deass = 0;									// keep CS low
    SPIM_Trans(MXC_SPIM1, &req);*/

    SPI_START();
       SPI_(HAL_SPI_Transmit(&SPI_HANDLE,cmd_pec, 2, 1000));
       SPI_END();

    // send 24 clock cycles per byte
       SPI_START();
    for (count = 0; count < ((3 * byte_number) - 1); count++)
    {
      /*  req.len = 1;
        req.tx_data = nothing;							// this line expects an array
        req.rx_data = NULL;
        req.deass = 0;								// keep CS low
        SPIM_Trans(MXC_SPIM1, &req);*/

    	    SPI_(HAL_SPI_Transmit(&SPI_HANDLE,nothing, 1, 1000));

    }
    // and one final byte to ensure CS goes high
   /* req.deass = 1;									// set CS high at end of transaction
    SPIM_Trans(MXC_SPIM1, &req);*/
    SPI_END();
}

void isospi_wake(void)									// wake up iso-SPI
{
    /* iso_SPI goes to sleep after T_idle (5.5ms)
     * This does not refresh the watchdog. It only wakes up the iso-SPI
     * The watchdog is only refreshed if a valid command + PEC is received
     *
     * All I need to do is pull the CS line low momentarily but the Maxim micro
     * does not allow me to do this, so I have to write an entire dummy byte to the
     * ADBMS in order to wake it up
     */
    uint8_t		nothing[1];								// dummy array

    nothing[0] = 0;
    // Send 0x00 via iso-SPI port just to wake it up
   /* req.len = 1;									// send 1 byte
    req.tx_data = nothing;								// this line expects an array
    req.rx_data = NULL;
    req.deass = 1;
    SPIM_Trans(MXC_SPIM1, &req); */

    SPI_START();
        SPI_(HAL_SPI_Receive(&SPI_HANDLE,nothing,1, 1000));
        SPI_END();

        delay_ms(1);										// wait for part to wake up (500us)

}

// ****************** Start PWM ************************************************
void set_pwm(uint16_t data)
{
    /* 'data' is a 16 bit word, each bit representing a cell. This function converts
     * each bit into a PWM value, then send to WRPWMA and WRPWMB
     *
     * set a non zero values into DCTO for PWM to work
     *
     * Ensure the RD bit in the ADCV command is cleared to ensure discharging is not interrupted
     * If you don't, it can really ruin your day */

    uint8_t 	duty = 0x07;							// set duty cycle for all channels
    uint8_t 	tx_data[6];								//
    uint8_t		array_count;							// steps through array

    // program WRPWMA
    for (array_count = 0; array_count < 6; array_count++)
    {
        tx_data[array_count] = 0;
        if (data & 0x0001)
        {
            tx_data[array_count] = duty;
        }
        if (data & 0x0002)
        {
            tx_data[array_count] = tx_data[array_count] | (duty << 4);
        }
        data >>= 2;										// look at the next 2 bits
    }

    write_cmd(WRPWMA, tx_data);

    // program WRPWMB
    for (array_count = 0; array_count < 6; array_count++)
    {
        tx_data[array_count] = 0;
        if (data & 0x0001)
        {
            tx_data[array_count] = duty;
        }
        if (data & 0x0002)
        {
            tx_data[array_count] = tx_data[array_count] | (duty << 4);
        }
        data >>= 2;										// look at the next 2 bits
    }

    write_cmd(WRPWMB, tx_data);
}

void clear_flags(void)
{
    uint8_t 	tx_data[6];								// 6 bytes

    // Clear UV/OV Flags
    tx_data[0] = 0xFF;
    tx_data[1] = 0xFF;
    tx_data[2] = 0xFF;
    tx_data[3] = 0xFF;
    tx_data[4] = 0xFF;
    tx_data[5] = 0xFF;

    write_cmd(CLOVUV, tx_data);

    // Clear flags in Status register C
    tx_data[0] = 0xFF;									// clear CS8FLT - CS1FLT
    tx_data[1] = 0xFF;									// clear CS16FLT - CS9FLT
    tx_data[2] = 0xFF;
    tx_data[3] = 0xFF;
    tx_data[4] = 0xFF;									// VA and VD UV/OV etc
    tx_data[5] = 0xFF;

    write_cmd(CLRFLAG, tx_data);
}

// ****************** Clear Sticky Flags ***************************************
void clear_cmflags(void)
{
    uint16_t	data;
    uint8_t		cmd[2];									// 2 byte command
    uint8_t		cmd_pec[2];								// 2 byte command pec
    uint8_t		data_pec[2];							// 2 byte data pec
    uint8_t		tx_data[2];

    // we only send 2 bytes (and need to have a data PEC that matches that
    tx_data[0] = 0xFF;
    tx_data[1] = 0xFF;

    // get_cmd_pec expects a pointer to an array
    load_array(cmd, CLRCMFLAG);							// put command into 2 byte array, cmd[]
    data = get_cmd_pec(cmd, 2);							// get command PEC
    load_array(cmd_pec, data);							// put command pec into 2 byte array, cmd_pec[]
    data = get_data_pec(tx_data, 0, 2);					// get pec on 2 bytes of data
    load_array(data_pec, data);							// fill data pec array

    ADBMS6830_write(cmd, cmd_pec, tx_data, data_pec, 2);
   // write_ADBMS8630(total_ic, tx_cmd, data)
}



void Poll_ADBMS6830(uint8_t tx_cmd[2]) //The command to be transmitted

{
	uint8_t cmd[4];
	uint16_t cmd_pec;
	//uint8_t md_bits;

	cmd[0] = tx_cmd[0];
	cmd[1] =  tx_cmd[1];
	cmd_pec = get_cmd_pec(cmd, 2);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	SPI_START();
	//spi_write_array(4,cmd);
	 SPI_(HAL_SPI_Transmit(&SPI_HANDLE, cmd, 4, 1000));
	SPI_END();
}

void write_ADBMS8630(uint8_t total_ic, //Number of ICs to be written to
			  uint8_t tx_cmd[2], //The command to be transmitted
			  uint8_t data[] // Payload Data
			  )
{
	const uint8_t BYTES_IN_REG = 6;
	const uint8_t CMD_LEN = 4+(8*total_ic);
	uint8_t *cmd;
	uint16_t data_pec;
	uint16_t cmd_pec;
	uint8_t cmd_index;

	cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));
	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = get_cmd_pec(cmd,2);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	cmd_index = 4;
	for (uint8_t current_ic = total_ic; current_ic > 0; current_ic--)               // Executes for each LTC681x, this loops starts with the last IC on the stack.
    {	                                                                            //The first configuration written is received by the last IC in the daisy chain
		for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
		{
			cmd[cmd_index] = data[((current_ic-1)*6)+current_byte];
			cmd_index = cmd_index + 1;
		}

		data_pec = (uint16_t)get_cmd_pec(&data[(current_ic-1)*6], BYTES_IN_REG);    // Calculating the PEC for each ICs configuration register data
		cmd[cmd_index] = (uint8_t)(data_pec >> 8);
		cmd[cmd_index + 1] = (uint8_t)data_pec;
		cmd_index = cmd_index + 2;
	}

	SPI_START();
	//spi_write_array(CMD_LEN, cmd);
	 SPI_(HAL_SPI_Transmit(&SPI_HANDLE, cmd, CMD_LEN, 1000));
	SPI_END();

	free(cmd);
}


int8_t Read_ADBMS8630( uint8_t total_ic, // Number of ICs in the system
				uint8_t tx_cmd[2], // The command to be transmitted
				uint8_t *rx_data // Data to be read
				)
{
	const uint8_t BYTES_IN_REG = 8;
	uint8_t cmd[4];
	uint8_t data[256];
	int8_t pec_error = 0;
	uint16_t cmd_pec;
	uint16_t data_pec;
	uint16_t received_pec;

	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = get_cmd_pec(cmd, 2);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	SPI_START();
	//spi_write_read(cmd, 4, data, (BYTES_IN_REG*total_ic));         //Transmits the command and reads the configuration data of all ICs on the daisy chain into rx_data[] array
	  SPI_(HAL_SPI_Transmit(&SPI_HANDLE, cmd, 4, 1000));
	  SPI_(HAL_SPI_Receive(&SPI_HANDLE, data, (BYTES_IN_REG*total_ic), 1000));
	SPI_END();

	for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++) //Executes for each LTC681x in the daisy chain and packs the data
	{																//into the rx_data array as well as check the received data for any bit errors
		for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
		{
			rx_data[(current_ic*8)+current_byte] = data[current_byte + (current_ic*BYTES_IN_REG)];
		}

		received_pec = (rx_data[(current_ic*8)+6]<<8) + rx_data[(current_ic*8)+7];
		data_pec = get_cmd_pec( &rx_data[current_ic*8], 6);

		if (received_pec != data_pec)
		{
		  pec_error = -1;
		}
	}

	return(pec_error);
}

uint8_t Read_CellVoltage_ADBMS8630(uint8_t reg, // Controls which cell voltage register is read back.
                     uint8_t total_ic, // The number of ICs in the system
					 uint16_t cell_codes[][16] // Array of the parsed cell codes
                    )
{

	int8_t pec_error = 0;
	uint8_t *cell_data;
	uint8_t c_ic = 0;
	const uint8_t NUM_RX_BYT = 8;
	const uint8_t NUM_CV_REG = 6;

	cell_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));

	if (reg == 0)
	{
		for (uint8_t cell_reg = 1; cell_reg<NUM_CV_REG+1; cell_reg++) //Executes once for each of the LTC681x cell voltage registers
		{
			Read_CellVoltage_Reg_ADBMS8630(cell_reg, total_ic, cell_data);
			for (int current_ic = 0; current_ic<total_ic; current_ic++)
			{
			if (ISOSPI_REVERSE== False)
			{
			  c_ic = current_ic;
			}
			else
			{
			  c_ic = total_ic - current_ic - 1;
			}
			pec_error = pec_error + parse_ADBMS6830_cv(current_ic,cell_reg, cell_data,
												&cell_codes[c_ic][0],
												NULL);                           //PEC RESULT SHOULD GO HERE, NOT NULL!!!
			}
		}
	}

	else
	{
		Read_CellVoltage_Reg_ADBMS8630(reg, total_ic, cell_data);

		for (int current_ic = 0; current_ic<total_ic; current_ic++)
		{
			if (ISOSPI_REVERSE == False)
			{
			c_ic = current_ic;
			}
			else
			{
			c_ic = total_ic - current_ic - 1;
			}
			pec_error = pec_error + parse_ADBMS6830_cv(current_ic,reg, &cell_data[8*c_ic],
					                          &cell_codes[current_ic][0],
											  NULL);                        //PEC RESULT SHOULD GO HERE
		}
	}

	//LTC681x_check_pec(total_ic,CELL,ic);      Struct is redundant now
	free(cell_data);

	return(pec_error);
}

void Read_CellVoltage_Reg_ADBMS8630(uint8_t reg, //Determines which cell voltage register is read back
                      uint8_t total_ic, //the number of ICs in the
                      uint8_t *data //An array of the unparsed cell codes
                     )
{
	const uint8_t REG_LEN = 8; //Number of bytes in each ICs register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (reg == 1)     //1: RDCVA
	{

		load_array(&cmd[0],RDCVA);
		//cmd[1] = 0x04;
		//cmd[0] = 0x00;
	}
	else if (reg == 2) //2: RDCVB
	{
		load_array(&cmd[0],RDCVB);
		//cmd[1] = 0x06;
		//cmd[0] = 0x00;
	}
	else if (reg == 3) //3: RDCVC
	{
		load_array(&cmd[0],RDCVC);
		//cmd[1] = 0x08;
		//cmd[0] = 0x00;
	}
	else if (reg == 4) //4: RDCVD
	{
		load_array(&cmd[0],RDCVD);
		//cmd[1] = 0x0A;
		//cmd[0] = 0x00;
	}
	else if (reg == 5) //4: RDCVE
	{
		load_array(&cmd[0],RDCVE);
		//cmd[1] = 0x09;
		//cmd[0] = 0x00;
	}
	else if (reg == 6) //4: RDCVF
	{
		load_array(&cmd[0],RDCVF);
		//cmd[1] = 0x0B;
		//cmd[0] = 0x00;
	}

	cmd_pec = get_cmd_pec(cmd, 2);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	SPI_START();


	SPI_(HAL_SPI_Transmit(&SPI_HANDLE, cmd, 4, 1000));
	SPI_(HAL_SPI_Receive(&SPI_HANDLE, data, (REG_LEN*total_ic), 1000));
	SPI_END();
}


int8_t Read_AUX_ADBMS6830(uint8_t reg, //Determines which GPIO voltage register is read back.
                     uint8_t total_ic,//The number of ICs in the system
					 uint16_t aux_codes[][10]//A two dimensional array of the gpio voltage codes.
                    )
{


	const uint8_t NUM_RX_BYT = 8;
	const uint8_t NUM_GPIO_REG = 4;
	uint8_t *data;
	int8_t pec_error = 0;
	uint8_t c_ic =0;


	data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));

	if (reg == 0)


	{
		for (uint8_t gpio_reg = 1; gpio_reg<NUM_GPIO_REG+1; gpio_reg++) //Executes once for each of the LTC681x aux voltage registers
		{
			Read_AUX_Reg_ADBMS6830(gpio_reg, total_ic, data); //Reads the raw auxiliary register data into the data[] array

			for (int current_ic = 0; current_ic<total_ic; current_ic++)
			{
				if (ISOSPI_REVERSE == False)
				{
				  c_ic = current_ic;
				}
				else
				{
				  c_ic = total_ic - current_ic - 1;
				}

				pec_error = parse_ADBMS6830_cv(current_ic,gpio_reg, data,
										&aux_codes[c_ic][0],
										NULL);                                //NULL SHOULDN'T GO HERE, FIND WHAT DOES.
			}
		}
	}
	else
	{
		Read_AUX_Reg_ADBMS6830(reg, total_ic, data);

		for (int current_ic = 0; current_ic<total_ic; current_ic++)
		{
			if (ISOSPI_REVERSE == False)
			{
			c_ic = current_ic;
			}
			else
			{
			c_ic = total_ic - current_ic - 1;
			}
			pec_error = parse_ADBMS6830_cv(current_ic,reg, data,
													&aux_codes[c_ic][0],
													NULL);
		}
	}
	//LTC681x_check_pec(total_ic,AUX,ic);       a function like this would be helpful to implement later
	free(data);

	return (pec_error);

}
void Read_AUX_Reg_ADBMS6830(uint8_t reg, //Determines which GPIO voltage register is read back
                       uint8_t total_ic, //The number of ICs in the system
                       uint8_t *data //Array of the unparsed auxiliary codes
                      )
{
	const uint8_t REG_LEN = 8; // Number of bytes in the register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (reg == 1)     //Read back auxiliary group A
	{

		load_array(&cmd[0], RDAUXA);
		//cmd[1] = 0x0C;
		//cmd[0] = 0x00;
	}
	else if (reg == 2)  //Read back auxiliary group B
	{
		load_array(&cmd[0], RDAUXB);
		//cmd[1] = 0x0E;
		//cmd[0] = 0x00;
	}
	else if (reg == 3)  //Read back auxiliary group C
	{
		load_array(&cmd[0], RDAUXC);
		//cmd[1] = 0x0D;
		//cmd[0] = 0x00;
	}
	else if (reg == 4)  //Read back auxiliary group D
	{
		load_array(&cmd[0], RDAUXD);
		//cmd[1] = 0x0F;
		//cmd[0] = 0x00;
	}
	else          //Read back auxiliary group A
	{
		load_array(&cmd[0], RDAUXD);
		//cmd[1] = 0x0C;
		//cmd[0] = 0x00;
	}


	cmd_pec = get_cmd_pec(cmd, 2);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	SPI_START();
	SPI_(HAL_SPI_Transmit(&SPI_HANDLE, cmd, 4, 1000));
	  SPI_(HAL_SPI_Receive(&SPI_HANDLE, data, (REG_LEN*total_ic), 1000));
	SPI_END();
}

void WRCFGA_ADBMS6830(uint8_t total_ic, //The number of ICs being written to
                   uint8_t config_data[][6]  // A two dimensional array of the configuration data that will be written
                  )
{

	uint8_t cmd[2];
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;

	load_array(cmd, WRCFGA);

	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ISOSPI_REVERSE == False)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}

		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = config_data[c_ic][data];
			write_count++;
		}
	}
	write_ADBMS8630(total_ic, cmd, write_buffer);
}

void WRCFGB_ADBMS6830(uint8_t total_ic, //The number of ICs being written to
		         uint8_t config_data[][6]) // A two dimensional array of the configuration data that will be written

{
	uint8_t cmd[2];
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;

	load_array(cmd, WRCFGB);

	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ISOSPI_REVERSE == False)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}

		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = config_data[c_ic][data];
			write_count++;
		}
	}
	write_ADBMS8630(total_ic, cmd, write_buffer);
}


void Start_Cell_ADC_adbms6830() //The command to be transmitted

{
	uint8_t cmd[2];
	load_array(cmd, ADCV);
	Poll_ADBMS6830(cmd);
}

void Start_AUX_ADC_adbms6830() //The command to be transmitted

{
	uint8_t cmd[2];
	load_array(cmd, ADCV);
	Poll_ADBMS6830(cmd);
}

 //UNFINISHED
void check_open_wire(void)
{

/*
    // this needs finishing


    uint8_t		count;
    uint32_t	spare_volts[17];						// cell voltages from S-ADC


    // measure all CADC (using ADCV), then compare these results with S-ADCs with open wire switch closed


    poll_cmd(ADSV | 0x01);								// measure OW on odd channels
    delay_ms(10);
    get_spare_voltages(spare_volts);					// read S-ADCs

    printf("		******** Open Wire Readings *******\n");

    for (count = 0; count < 16; count++)
    {
        if ((spare_volts[count] < 1000) && (spare_volts[count] > 0))
        {
            printf("			Wire Break?");
        }
    }

    poll_cmd(ADSV | 0x02);								// measure OW on even channels
    `_ms(10);

    poll_cmd(ADSV | 0x80);								// set CONT bit to 1
    delay_ms(10); */
}

