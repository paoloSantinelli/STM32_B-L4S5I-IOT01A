/*
 * hts221.c
 *
 *  Created on: Nov 24, 2021
 *      Author: Paolo Santinelli, Alessio Cuoghi
 *	Rev 1, Jan 07, 2023: Fixed name hst with hts
 */
#include "hts221.h"

calib_params hts221Params;

int16_t hts221Init(i2c_transaction *i2cTrans)
{
	int16_t retVal;
	uint8_t C1reg[2] =	{0x20,0x80}; 	// CTRL_1 REG, POWER UP

	i2cTrans->tx_len = 2;
	i2cTrans->tx_buf = (void*) C1reg;
	i2cTrans->rx_buf = (void*) 0;
	i2cTrans->rx_len = 0;
	retVal = i2c_transfer(i2cTrans);

 return retVal;
}

int16_t hts221ReadId(i2c_transaction *i2cTrans, uint8_t * id)
{
	int16_t retVal;
	uint8_t whoAmIReg[2] = {0x0f,0x00}; 	// WHO AM I register

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 1;
	i2cTrans->tx_buf = (void*) &(whoAmIReg[0]);
	i2cTrans->rx_buf = (void*) &(whoAmIReg[1]);
	retVal = i2c_transfer(i2cTrans);
	*id = whoAmIReg[1];

 return retVal;
}

int16_t hts221ReadParams(i2c_transaction *i2cTrans,  calib_params * params)
{
	int16_t retVal = 0;

	uint8_t H0_rH_x2[2]   = {0x30,0x00}; 		// H0_rH_x2   register
	uint8_t H1_rH_x2[2]   = {0x31,0x00}; 		// H1_rH_x2   register
	uint8_t H0_T0_OUT[3]  = {0xB6,0x00,0x00}; 	// H0_T0_OUT  register
	uint8_t H1_T0_OUT[3]  = {0xBA,0x00,0x00}; 	// H1_T0_OUT  register

	uint8_t T0_degC_x8[2] = {0x32,0x00}; 		// T0_degC_x8 register
	uint8_t T1_degC_x8[2] = {0x33,0x00}; 		// T1_degC_x8 register
	uint8_t T0_OUT[3]  	  = {0xBC,0x00,0x00}; 	// T0_OUT  	  register
	uint8_t T1_OUT[3]  	  = {0xBE,0x00,0x00}; 	// T1_OUT  	  register
	uint8_t T1_T0_msb[2]  = {0x35,0x00}; 		// T1_T0_msb  register

	// 1 Read H0_rH_x2 and H1_rH_x2 coefficients

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 1;

	i2cTrans->tx_buf = (void*) &(H0_rH_x2[0]);
	i2cTrans->rx_buf = (void*) &(H0_rH_x2[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	i2cTrans->tx_buf = (void*) &(H1_rH_x2[0]);
	i2cTrans->rx_buf = (void*) &(H1_rH_x2[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	// 2 Read H0_T0_OUT

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 2;

	i2cTrans->tx_buf = (void*) &(H0_T0_OUT[0]);
	i2cTrans->rx_buf = (void*) &(H0_T0_OUT[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	// 3 Read H1_T0_OUT

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 2;

	i2cTrans->tx_buf = (void*) &(H1_T0_OUT[0]);
	i2cTrans->rx_buf = (void*) &(H1_T0_OUT[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	// 4 Read T0_degC_x8 and T1_degC_x8 coefficients

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 1;

	i2cTrans->tx_buf = (void*) &(T0_degC_x8[0]);
	i2cTrans->rx_buf = (void*) &(T0_degC_x8[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	i2cTrans->tx_buf = (void*) &(T1_degC_x8[0]);
	i2cTrans->rx_buf = (void*) &(T1_degC_x8[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	// 5 Read T1_T0_msb coefficients

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 1;

	i2cTrans->tx_buf = (void*) &(T1_T0_msb[0]);
	i2cTrans->rx_buf = (void*) &(T1_T0_msb[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	// 6 Read T0_OUT

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 2;

	i2cTrans->tx_buf = (void*) &(T0_OUT[0]);
	i2cTrans->rx_buf = (void*) &(T0_OUT[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	// 7 Read T1_OUT

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 2;

	i2cTrans->tx_buf = (void*) &(T1_OUT[0]);
	i2cTrans->rx_buf = (void*) &(T1_OUT[1]);
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	// Fill the calib_params data structure

	params->H0_rH		= H0_rH_x2[1] >>1;
	params->H1_rH		= H1_rH_x2[1] >>1;
	params->H0_T0_OUT 	= (((uint16_t)H0_T0_OUT[2]) <<8) | (uint16_t)H0_T0_OUT[1];
	params->H1_T0_OUT 	= (((uint16_t)H1_T0_OUT[2]) <<8) | (uint16_t)H1_T0_OUT[1];

	params->T0_degC 	= ((((uint16_t)(T1_T0_msb[1] & 0x03 )) <<8) | ((uint16_t)T0_degC_x8[1]) )>>3;
	params->T1_degC 	= ((((uint16_t)(T1_T0_msb[1] & 0x0c )) <<6) | ((uint16_t)T1_degC_x8[1]) )>>3;
	params->T0_OUT 		= (((uint16_t)T0_OUT[2])<<8)|(uint16_t)T0_OUT[1];
	params->T1_OUT 		= (((uint16_t)T1_OUT[2])<<8)|(uint16_t)T1_OUT[1];;

 return retVal;
}

int16_t hts221ReadTempHum(i2c_transaction *i2cTrans,  calib_params * params, temp_hum * tempHum)
{
	int16_t retVal = 0;
	int16_t T_out;
	int16_t H_T_out;
	int32_t tmp32;
	uint8_t CTRL_REG1[2]  = {0x20,0x00}; 	// CTRL_REG1  Control register 1
	uint8_t CTRL_REG2[2]  = {0x21,0x00}; 	// CTRL_REG2  Control register 2
	uint8_t STATUS_REG[2] = {0x27,0x00}; 	// STATUS_REG Status  register
	uint8_t OUT_DATA[5]   = {0xA8,0,0,0,0};	// H_OUT_L, H_OUT_H,T_OUT_L, T_OUT_H

	// Device activation, setting the PD bit in CTRL_REG1

	CTRL_REG1[1] = 0x80;
	i2cTrans->tx_len = 2;
	i2cTrans->rx_len = 0;

	i2cTrans->tx_buf = (void*) CTRL_REG1;
	i2cTrans->rx_buf = (void*) NULL;
	retVal = i2c_transfer(i2cTrans);
	if(retVal) return -1;

	// Start a new conversion, setting the ONE_SHOT bit in CTRL_REG2

	CTRL_REG2[1] = 0x01;
	i2cTrans->tx_len = 2;
	i2cTrans->rx_len = 0;

	i2cTrans->tx_buf = (void*) CTRL_REG2;
	i2cTrans->rx_buf = (void*) NULL;
	retVal = i2c_transfer(i2cTrans);

	if(retVal) return -1;

	// Wait for new data (hum & temp) to become available.
	// Read H_DA & T_DA bits from the STATUS_REG

	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 1;

	i2cTrans->tx_buf = (void*) STATUS_REG;
	i2cTrans->rx_buf = (void*) &(STATUS_REG[1]);
	do{
		retVal = i2c_transfer(i2cTrans);
		if(retVal) return -1;
		osDelay(5);
	}while((STATUS_REG[1] & 0x03)!= 0x03);

	// Read Results
	i2cTrans->tx_len = 1;
	i2cTrans->rx_len = 4;

	i2cTrans->tx_buf = (void*) OUT_DATA;
	i2cTrans->rx_buf = (void*) &(OUT_DATA[1]);
	retVal = i2c_transfer(i2cTrans);

	if(retVal) return -1;

	// Read H_T_OUT & T_OUT values

	T_out 	= (((int16_t)OUT_DATA[4])<<8) | ((int16_t)OUT_DATA[3]);
	H_T_out = (((int16_t)OUT_DATA[2])<<8) | ((int16_t)OUT_DATA[1]);

	// Compute the Temperature value by linear interpolation

	tmp32 = ((int32_t)(T_out - params->T0_OUT)) * ((int32_t)(params->T1_degC - params->T0_degC)*10);
	tempHum->temp = tmp32 /(params->T1_OUT - params->T0_OUT) + params->T0_degC*10;

	// Compute the RH [%] value by linear interpolation

	tmp32 = ((int32_t)(H_T_out - params->H0_T0_OUT)) * ((int32_t)(params->H1_rH - params->H0_rH)*10);
	tempHum->hum = (tmp32/(params->H1_T0_OUT - params->H0_T0_OUT) + params->H0_rH*10);

	// Check Saturation condition

	if(tempHum->hum >1000) tempHum->hum = 1000;

	return retVal;
}
