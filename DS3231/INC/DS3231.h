

#ifndef INC_DS3231_H_
#define INC_DS3231_H_
#include "stm32f1xx_hal.h"
#include <time.h>

#define SECS_ADDRESS   0x00
#define MINS_ADDRESS   0x01
#define HRS_ADDRESS    0x02
#define DAYS_ADDRESS   0x03
#define DATE_ADDRESS   0x04
#define MONTHS_ADDRESS 0x05
#define YEARS_ADDRESS  0x06
static const uint8_t  SLAVE_ADDRESS = 0x68 << 1;



uint8_t DtoBCD(uint8_t dec);
uint8_t   BCDtoD(uint8_t);

void DS3231_Init(I2C_HandleTypeDef *hi2c);
int DS3231_getInt(uint8_t Reg_Address);
int DS3231_getSeconds();
int DS3231_getMinutes();
int DS3231_getHours();
int DS3231_getDays();
int DS3231_getDate();
int DS3231_getMonths();
int DS3231_getYears();
void DS3231_UpdateTime(struct tm *timeinfo);

int DS3231_setInt(uint8_t Reg_Address,uint8_t num);
int DS3231_setSeconds(uint8_t num);
int DS3231_setMinutes(uint8_t num);
int DS3231_setHours(uint8_t num);
int DS3231_setDays(uint8_t num);
int DS3231_setDate(uint8_t num);
int DS3231_setMonths(uint8_t num);
int DS3231_setYears(uint8_t num);
int DS3231_setTime(uint8_t hrs,uint8_t mins,uint8_t secs);



#endif /* INC_DS3231_H_ */
