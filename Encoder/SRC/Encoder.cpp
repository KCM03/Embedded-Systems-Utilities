/*
 * Endoder.cpp
 *
 *  Created on: 25 Mar 2023
 *      Author: kisha
 */


#include "Encoder.hpp"
Encoder::Encoder(TIM_HandleTypeDef* handle,int max){

	this->handle = handle;
	this->max = max;
	this->buf = 0;
}

    int Encoder::Count(){

    	this->buf[1] = this->buf[0];
    	this->buf[0] = (this->handle->Instance->CNT)>>2;
    		   if (this->buf[1]<this->buf[0] && this->buf[2]!=max){
    			   this->buf[2] += 1;
    		   }
    		   if (this->buf[1]>this->buf[0] && this->buf[2]!=0){
    			   this->buf[2] -=1;
    		   }
    	return this->buf[2];


    }
	int Encoder::getCount(){

		return this->buf[2];
	}

    void Encoder::setmax(int max){

    	this->max = max;
    }
    int Encoder::getmax(int max){

    return this->max;
    }

