/*
 * i2c.c
 *
 *  Created on: Jul 8, 2021
 *      Author: Alessio Cuoghi, Paolo Santinelli
 *      Rev 1, Dec 20, 2021
 *      Rev 2, Jan 07, 2023: Added time out i2c transaction
 */

#include "cmsis_os2.h"
#include "i2c.h"

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

extern osMutexId_t i2c1MutexHandle;
extern osSemaphoreId_t i2c1BinarySemHandle;
extern osMutexId_t i2c2MutexHandle;
extern osSemaphoreId_t i2c2BinarySemHandle;

volatile uint8_t   		*receiveBuffPointer;
volatile uint16_t 		receiveCnt = 0;
volatile uint8_t   		*transmitBuffPointer;
volatile uint16_t		transmitCnt = 0;
volatile osSemaphoreId_t	I2C1Semaphore; 	// I2c1 semaphore handle
volatile osSemaphoreId_t	I2C2Semaphore; 	// I2c2 semaphore handle
//******************************************************************************
//
// Initialize I2Cx routine.
//
//******************************************************************************
uint16_t initI2Cx(i2c_transaction *data)
{
	osStatus_t ret;

	if(data->hi2c == &hi2c2){
		data->mutex 		= i2c2MutexHandle;
		data->semaphore 	= i2c2BinarySemHandle;

		ret = osSemaphoreAcquire(data->semaphore, 0);
		if(ret != osOK ) return HAL_ERROR;

		ret = HAL_I2C_RegisterCallback(
					&hi2c2,
					HAL_I2C_MASTER_RX_COMPLETE_CB_ID,
					HAL_I2C_MasterRxCpltCallbackI2C2);
		if(ret != HAL_OK ) return HAL_ERROR;

		ret = HAL_I2C_RegisterCallback(
					&hi2c2,
					HAL_I2C_MASTER_TX_COMPLETE_CB_ID,
					HAL_I2C_MasterTxCpltCallbackI2C2);
		if(ret != HAL_OK ) return HAL_ERROR;

		I2C2Semaphore = data->semaphore ; 	// I2c2 semaphore handle

		return 0;

	} else if(data->hi2c == &hi2c1) {
		data->mutex 		= i2c1MutexHandle;
		data->semaphore 	= i2c1BinarySemHandle;

		ret = osSemaphoreAcquire(data->semaphore, 0);
		if(ret != osOK ) return HAL_ERROR;

		ret = HAL_I2C_RegisterCallback(
				&hi2c1,
				HAL_I2C_MASTER_RX_COMPLETE_CB_ID,
				HAL_I2C_MasterRxCpltCallbackI2C1);
		if(ret != HAL_OK ) return HAL_ERROR;

		ret = HAL_I2C_RegisterCallback(
				&hi2c1,
				HAL_I2C_MASTER_TX_COMPLETE_CB_ID,
				HAL_I2C_MasterTxCpltCallbackI2C1);
		if(ret != HAL_OK ) return HAL_ERROR;

		I2C1Semaphore = data->semaphore; 	// I2c1 semaphore handle

		return 0;

	} else return HAL_ERROR;

	return HAL_OK;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//******************************************************************************
//
// Data transfer I2C routine.
//
//******************************************************************************

//------------------------------------------------------------------------------
int i2c_transfer(i2c_transaction *data)
{
	osStatus_t val;
    osStatus_t result = osMutexAcquire(data->mutex, osWaitForever);	// Acquire i2c mutex
    //currI2Semaphore = data->semaphore;

    if (result != osOK) {
    	return 1; // Error acquiring mutex
    }

    // Transmit tx buffer to i2c slave device
    // Transmit data if there is any

    if (data->tx_len > 0) {
        //Initialize transmitter data pointer and counter
    	transmitBuffPointer = (uint8_t *)data->tx_buf;
    	transmitCnt = data->tx_len;
    	HAL_I2C_Master_Transmit_IT(data->hi2c, (data->DevAddress) <<1, (uint8_t *) transmitBuffPointer, transmitCnt);

    	// Suspend current task until data transfer will be completed (see TX CallBack function)
   		val = osSemaphoreAcquire(data->semaphore, TIMEOUT_I2C);

   		if (val != osOK){
   			osMutexRelease(data->mutex);
   			return 2; // Master transmit error
   		}
   	}

    // Receive data if there is any
   	if (data->rx_len > 0) {
   		//Initialize receiver data pointer and counter
   		receiveBuffPointer = (uint8_t *)data->rx_buf;
   		receiveCnt = data->rx_len;
   		HAL_I2C_Master_Receive_IT(data->hi2c, (data->DevAddress) <<1, (uint8_t *) receiveBuffPointer, receiveCnt);

    	// Suspend current task until data transfer will be completed (see RX CallBack function)
   		val = osSemaphoreAcquire(data->semaphore, TIMEOUT_I2C);
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

//void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef * hi2c)
//{
//	//osSemaphoreRelease(i2cBinarySemHandle);
//	osSemaphoreRelease(currI2Semaphore);
//}

//void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef * hi2c)
//{
//	//osSemaphoreRelease(i2cBinarySemHandle);
//	osSemaphoreRelease(currI2Semaphore);
//}

void HAL_I2C_MasterRxCpltCallbackI2C2(I2C_HandleTypeDef * hi2c)
{
	//osSemaphoreRelease(i2cBinarySemHandle);
	osSemaphoreRelease(I2C2Semaphore);
}

void HAL_I2C_MasterTxCpltCallbackI2C2(I2C_HandleTypeDef * hi2c)
{
	//osSemaphoreRelease(i2cBinarySemHandle);
	osSemaphoreRelease(I2C2Semaphore);
}

void HAL_I2C_MasterRxCpltCallbackI2C1(I2C_HandleTypeDef * hi2c)
{
	//osSemaphoreRelease(i2cBinarySemHandle);
	osSemaphoreRelease(I2C1Semaphore);
}

void HAL_I2C_MasterTxCpltCallbackI2C1(I2C_HandleTypeDef * hi2c)
{
	//osSemaphoreRelease(i2cBinarySemHandle);
	osSemaphoreRelease(I2C1Semaphore);
}



