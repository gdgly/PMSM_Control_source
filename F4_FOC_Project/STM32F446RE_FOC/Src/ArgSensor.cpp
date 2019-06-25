/*
 * ArgSensor.cpp
 *
 *  Created on: Jun 25, 2019
 *      Author: watashi
 */

#include "ArgSensor.hpp"

ArgSensor::ArgSensor() {
	// TODO Auto-generated constructor stub
	mArg = 0;
}

ArgSensor::~ArgSensor() {
	// TODO Auto-generated destructor stub
}

void ArgSensor::Init(void) {
	mArg = 0;
}

void ArgSensor::increment(float pArg){
	mArg = mArg + pArg;
}

void ArgSensor::decrement(float pArg){
	mArg = mArg - pArg;
}

float ArgSensor::getArg(void){
	return mArg;
}
