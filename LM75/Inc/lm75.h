/*******************************************************
 * File Name: lm75.h
 * Author: Gatsu-Catsu (https://github.com/Gatsu-Catsu )
 * Creation Date: 2024-08-31
 * Description: Header file containing LM75 functions declarations.
 *
 * License:
 * The MIT License (MIT)
 *******************************************************/


#ifndef __LM75__
#define __LM75__


/* Replace this line with your version of HAL */
#include "stm32f0xx_hal.h"


/* Status returned by LM75 functions*/
typedef enum {
    LM75_OK,
    LM75_ERROR
} LM75_Status;


/* LM75 sensor version depending on how the Temp register is read */
typedef enum {
    LM75_9BIT,
    LM75_11BIT
} LM75_Version;


/* Structure storing the sensor properties */
typedef struct {
    /* I2C interface to which the sensor is connected */
    I2C_HandleTypeDef *i2c;

    /* Sensor version */
    LM75_Version ver;

    /* Sensor address */
    uint8_t addr;

    /* Actual temperature in degrees celsius stored in the Thyst register */
    float thyst_c;

    /* Actual temperature in degrees celsius stored in the Tos register */
    float tos_c;

    /* Actual temperature in degrees celsius stored in the Temp register */
    float temp_c;
} LM75;


LM75_Status LM75_Init(LM75 *dev, I2C_HandleTypeDef *hi2c, LM75_Version ver, uint8_t addr, float low_lim, float upp_lim);
LM75_Status LM75_SetHysteresis(LM75 *dev, float low_lim);
LM75_Status LM75_SetOverTemperatureShutdown(LM75 *dev, float upp_lim);

#endif