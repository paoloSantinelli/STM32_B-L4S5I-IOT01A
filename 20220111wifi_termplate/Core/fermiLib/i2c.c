/*
 * i2c.c
 *
 *  Created on: Jul 8, 2021
 *      Author: Alessio Cuoghi, Paolo Santinelli
 */


#include "i2c.h"
//#include "cmsis_os.h"

extern osSemaphoreId_t i2cBinarySemHandle;

volatile uint8_t *receiveBuffPointer;
volatile uint16_t receiveCnt = 0;
volatile uint8_t *transmitBuffPointer;
volatile uint16_t transmitCnt = 0;

//******************************************************************************
//
// Data transfer I2C routine.
//
//******************************************************************************

//------------------------------------------------------------------------------
int i2c_transfer(i2c_transaction *data)
{
	osStatus_t val;


    osStatus_t result = osMutexAcquire(data->mutex, osWaitForever);

    if (result != osOK) {
    	return 1; // Error acquiring mutex
    }

    if (data->tx_len > 0) {
    /* Transmit data if there is any */
        //Initialize transmitter data pointer and counter
    	transmitBuffPointer = (uint8_t *)data->tx_buf;
    	transmitCnt = data->tx_len;
    	HAL_I2C_Master_Transmit_IT(data->hi2c, (data->DevAddress) <<1, (uint8_t *) transmitBuffPointer, transmitCnt);
   		val = osSemaphoreAcquire(data->semaphore, osWaitForever);

   		if (val != osOK){
   			osMutexRelease(data->mutex);
   			return 2; // Master transmit error
   		}

   	}
    	/* Receive data if there is any */
   	if (data->rx_len > 0) {
   		//Initialize receiver data pointer and counter
   		receiveBuffPointer = (uint8_t *)data->rx_buf;
   		receiveCnt = data->rx_len;
   		HAL_I2C_Master_Receive_IT(data->hi2c, (data->DevAddress) <<1, (uint8_t *) receiveBuffPointer, receiveCnt);

   		val = osSemaphoreAcquire(data->semaphore, osWaitForever);
   		if (val != osOK){
   			osMutexRelease(data->mutex);
   			return 3; //Master receive error
   		}
   	}

    osMutexRelease(data->mutex);
    return 0;
}

//------------------------------------------------------------------------------
// CallBack functions
//
//------------------------------------------------------------------------------

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
{
	osSemaphoreRelease(i2cBinarySemHandle);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c)
{
	osSemaphoreRelease(i2cBinarySemHandle);
}

