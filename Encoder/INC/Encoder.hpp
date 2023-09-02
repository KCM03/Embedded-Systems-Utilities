/*
 * Encoder.hpp
 *
 *  Created on: 25 Mar 2023
 *      Author: kisha
 */

#pragma once

class Encoder{

	Encoder(TIM_HandleTypeDef* handle, int count, int max);
	~Encoder();
	int Count();
	int getCount();
    void setmax(int max);
    int getmax(int max);


private:

    TIM_HandleTypeDef* handle;
	int max;
	int buf[3];
};





