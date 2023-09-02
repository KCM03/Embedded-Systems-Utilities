


#include "stm32f1xx_hal.h"
#include "DS3231.h"
I2C_HandleTypeDef *DS3132_hi2c1;

void DS3231_Init(I2C_HandleTypeDef *hi2c){

	DS3132_hi2c1 = hi2c;
}

int DS3231_getInt(uint8_t Reg_Address){
	HAL_StatusTypeDef ret;

	uint8_t buf[12];
	//uint8_t val;
	//int ans;
	buf[0]= Reg_Address;

	   ret = HAL_I2C_Master_Transmit(DS3132_hi2c1, SLAVE_ADDRESS, buf, 1, HAL_MAX_DELAY);
	   if (ret != HAL_OK){
	  	   return 404;   }
	   else{
		   ret = HAL_I2C_Master_Receive(DS3132_hi2c1, SLAVE_ADDRESS, buf, 1, HAL_MAX_DELAY);
		   if (ret != HAL_OK){
		   	  	   return 404;   }
		   	   else{
		   //val = buf[0];
           //ans = val;
		   return BCDtoD(buf[0]);
   }

}
}

int DS3231_setInt(uint8_t Reg_Address,uint8_t num){
	HAL_StatusTypeDef ret;

		uint8_t buf[12];
		buf[0] = Reg_Address;
		buf[1]= DtoBCD(num);



		   ret = HAL_I2C_Master_Transmit(DS3132_hi2c1, SLAVE_ADDRESS, buf, 2, HAL_MAX_DELAY);
		   if (ret != HAL_OK){
		  	   return 1;   }
		   else{
			   return 0;
	   }
}

int DS3231_getSeconds(){return DS3231_getInt(SECS_ADDRESS);}
int DS3231_getMinutes(){return DS3231_getInt(MINS_ADDRESS);}
int DS3231_getHours(){return DS3231_getInt(HRS_ADDRESS);}
int DS3231_getDays(){return DS3231_getInt(DAYS_ADDRESS);}
int DS3231_getDate(){return DS3231_getInt(DATE_ADDRESS);}
int DS3231_getMonths(){return DS3231_getInt(MONTHS_ADDRESS);}
int DS3231_getYears(){return DS3231_getInt(YEARS_ADDRESS);}
int DS3231_setSeconds(uint8_t num){return DS3231_setInt(SECS_ADDRESS,num);}
int DS3231_setMinutes(uint8_t num){return DS3231_setInt(MINS_ADDRESS,num);}
int DS3231_setHours(uint8_t num){return DS3231_setInt(HRS_ADDRESS,num);}
int DS3231_setDays(uint8_t num){return DS3231_setInt(DAYS_ADDRESS,num);}
int DS3231_setDate(uint8_t num){return DS3231_setInt(DATE_ADDRESS,num);}
int DS3231_setMonths(uint8_t num){return DS3231_setInt(MONTHS_ADDRESS,num);}
int DS3231_setYears(uint8_t num){return DS3231_setInt(YEARS_ADDRESS,num);}
uint8_t DtoBCD(uint8_t dec){return (dec % 10 + ((dec / 10) << 4));}
uint8_t   BCDtoD(uint8_t bin){return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);}

void DS3231_UpdateTime(struct tm *timeinfo){
	timeinfo->tm_hour = DS3231_getHours();
	timeinfo->tm_min = DS3231_getMinutes();
	timeinfo->tm_sec = DS3231_getSeconds();
	timeinfo->tm_wday = DS3231_getDays();
	timeinfo->tm_mon = DS3231_getMonths();
	timeinfo->tm_mday = DS3231_getDate();

}
int DS3231_setTime(uint8_t hrs,uint8_t mins,uint8_t secs){

return DS3231_setHours(hrs)*DS3231_setMinutes(mins)*DS3231_setSeconds(secs);
}

