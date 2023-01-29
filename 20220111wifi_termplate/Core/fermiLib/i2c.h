/*
 * i2c.h
 *
 *  Created on: Jul 8, 2021
 *      Author: Alessio Cuoghi, Paolo Santinelli
 */

#ifndef FERMILIB_I2C_H_
#define FERMILIB_I2C_H_
#include "cmsis_os.h"
#include "stm32l4xx_hal.h"


typedef struct
{
    I2C_HandleTypeDef *hi2c; //I2c handle address
    uint16_t DevAddress; //I2C device address
    osMutexId_t mutex;  //i2c Mutex handle
    osSemaphoreId_t semaphore; //i2c semaphore handle
	unsigned int tx_len;
    void * tx_buf;
    unsigned int rx_len;
    void * rx_buf;

} i2c_transaction;

int i2c_transfer(i2c_transaction *data);


#endif /* FERMILIB_I2C_H_ */
