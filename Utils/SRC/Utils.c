/*
 * Utils.c
 *
 *  Created on: 19 Apr 2023
 *      Author: kisha
 */
#include "Utils.h"
int ilength(int i){
		int n = i;
		int count = 0;
	while(n != 0){
		n = n/10;
		count++;
	}
	return count;
	}


  char* ftoa(float num, int points,char* buffer){

     int  intlength  = ilength(num);
     int  justint =  abs(num * powf(10,points));
     char pointside[12];
     char* nopoint = itoa(justint,buffer,10);

     for (int i = intlength;i<= intlength+points;i++){

    	 pointside[i-intlength] = nopoint[i];
     }

 strcat(strcat(itoa(abs(num),buffer,10),"."),pointside);

 if(num<0)
 {
 for(int i = sizeof(buffer)-1;i>0;i--){

	 buffer[i] = buffer[i-1];
 }
 buffer[0] = '-';
 }

return buffer;
  }

