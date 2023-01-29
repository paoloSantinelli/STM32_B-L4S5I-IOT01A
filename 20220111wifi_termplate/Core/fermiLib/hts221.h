/*
 * hts221.h
 *
 *  Created on: Nov 24, 2021
 *      Author: Paolo Santinelli, Alessio Cuoghi
 *	Rev 1, Jan 07, 2023: Fixed name hst with hts
 */

#ifndef FERMILIB_HTS221_H_
#define FERMILIB_HTS221_H_
#include "i2c.h"

// Calibration parameters
typedef struct
{
	int16_t H0_rH;
	int16_t H1_rH;
	int16_t H0_T0_OUT;
	int16_t H1_T0_OUT;

	int16_t T0_degC;
	int16_t T1_degC;
	int16_t T0_OUT;
	int16_t T1_OUT;

} calib_params;

// Temperature & Humidity
typedef struct
{
	int16_t  temp;
	uint16_t hum;
} temp_hum;


int16_t hts221Init(i2c_transaction *i2cTrans);
int16_t hts221ReadId(i2c_transaction *i2cTrans, uint8_t * id);					// Read who am I register to get
int16_t hts221ReadParams(i2c_transaction *i2cTrans,  calib_params * params);	// the Device identification code
int16_t hts221ReadTempHum(i2c_transaction *i2cTrans,  calib_params * params, temp_hum * tempHum);
#endif /* FERMILIB_HTS221_H_ */
