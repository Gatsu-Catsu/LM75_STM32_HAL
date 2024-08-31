/*******************************************************
 * File Name: lm75.c
 * Author: Gatsu-Catsu (https://github.com/Gatsu-Catsu )
 * Creation Date: 2024-08-31
 * Description: Source file containing LM75 functions definitions.
 *
 * License:
 * The MIT License (MIT)
 *******************************************************/


#include <math.h>

#include "lm75.h"


/* Pointers to registers */
#define LM75_TEMP_REG       0x00
#define LM75_CONF_REG       0x01
#define LM75_THYST_REG      0x02
#define LM75_TOS_REG        0x03


/* Configuration register bits */
#define SHUTDOWN            0x01
#define CMP_MODE            0x00
#define INT_MODE            0x20
#define OS_ACT_LOW          0x00
#define OS_ACT_HIGH         0x04
#define ONE_FAULT           0x00
#define TWO_FAULTS          0x08
#define FOUR_FAULTS         0x10
#define SIX_FAULTS          0x18


/* Register lengths */
#define MAX_REG_SIZE        2
#define MIN_REG_SIZE        1


/* Maximum transmission time */
#define TIMEOUT             500


/* Limits of THYST and TOS register */
#define MAX_TEMP           125
#define MIN_TEMP           -55  


static LM75_Status write_config(LM75 *dev, uint8_t *data);
static LM75_Status read_config(LM75 *dev, uint8_t *dest);
static LM75_Status write_temperature(LM75 *dev, uint8_t mem_addr, float temp);
static LM75_Status read_temperature(LM75 *dev, uint8_t mem_addr, uint16_t *dest);


/* Write to configuration register */
static LM75_Status write_config(LM75 *dev, uint8_t *data)
{
    if (HAL_OK != HAL_I2C_Mem_Write(dev->i2c, dev->addr, LM75_CONF_REG, I2C_MEMADD_SIZE_8BIT, data, MIN_REG_SIZE, TIMEOUT))
    {
        return LM75_ERROR;
    }

    return LM75_OK;
}

/* Read from the configuration register */
static LM75_Status read_config(LM75 *dev, uint8_t *dest)
{

    if (HAL_OK != HAL_I2C_Mem_Read(dev->i2c, dev->addr, LM75_CONF_REG, I2C_MEMADD_SIZE_8BIT, dest, MIN_REG_SIZE, TIMEOUT))
    {
        return LM75_ERROR;
    }

    return LM75_OK;
}

/* Write to Tos or Thyst register */
static LM75_Status write_temperature(LM75 *dev, uint8_t mem_addr, float temp)
{
    int8_t value[MAX_REG_SIZE] = {0};

    value[0] = temp;

    if ( fabs(temp - value[0]) >= 0.5f )
    {
        value[1] = 0x80;
    }
    else
    {
        value[1] = 0x00;
    }
    
    if (HAL_OK != HAL_I2C_Mem_Write(dev->i2c, dev->addr, mem_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)value, MAX_REG_SIZE, TIMEOUT))
    {
        return LM75_ERROR;
    }

    return LM75_OK;
}

/* Read from Temp, Tos or Thyst register */
static LM75_Status read_temperature(LM75 *dev, uint8_t mem_addr, uint16_t *dest)
{
    uint8_t temp_data[MAX_REG_SIZE] = {0};

    if (HAL_OK != HAL_I2C_Mem_Read(dev->i2c, dev->addr, mem_addr, I2C_MEMADD_SIZE_8BIT, temp_data, MAX_REG_SIZE, TIMEOUT))
    {
        return LM75_ERROR;
    }

    *dest = (temp_data[0] << 8) | temp_data[1];

    return LM75_OK;
}


/* Initialisation of a new sensor */
LM75_Status LM75_Init(LM75 *dev, I2C_HandleTypeDef *hi2c, LM75_Version ver, uint8_t addr, float low_lim, float upp_lim)
{
    /* Configure the sensor */
    uint8_t cfg_reg_value = ( TWO_FAULTS | OS_ACT_LOW | CMP_MODE );

    /* Set struct parameters */
    dev->i2c = hi2c;
    dev->ver = ver;
    dev->addr = (addr << 1);
    dev->thyst_c = 0.0f;
    dev->tos_c = 0.0f;
    dev->temp_c = 0.0f;

    /* TOS value must be greater than THYST */
    if (low_lim >= upp_lim)
    {
        return LM75_ERROR;
    }

    /* Set Conf register value */
    if (LM75_OK != write_config(dev, LM75_CONF_REG, &cfg_reg_value))
    {
        return LM75_ERROR;
    }   

    /* Set Thyst register value */
    if (LM75_OK != LM75_SetHysteresis(dev, low_lim))
    {
        return LM75_ERROR;
    }

    return LM75_OK;
}

/* Set the limit at which the O.S. pin will no longer be driven */
LM75_Status LM75_SetHysteresis(LM75 *dev, float low_lim)
{
    if (low_lim > MAX_TEMP || low_lim < MIN_TEMP)
    {
        return LM75_ERROR;
    }

    if (LM75_OK != write_temperature(dev, LM75_THYST_REG, low_lim))
    {
        return LM75_ERROR;
    }

    dev->thyst_c = low_lim;

    return LM75_OK;
}