/*******************************************************
 * File Name: lm75.c
 * Author: Gatsu-Catsu (https://github.com/Gatsu-Catsu )
 * Creation Date: 2024-08-31
 * Description: Source file containing LM75 functions definitions.
 *
 * License:
 * The MIT License (MIT)
 *******************************************************/


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