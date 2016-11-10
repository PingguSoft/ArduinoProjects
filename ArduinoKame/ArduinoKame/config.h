/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 see <http://www.gnu.org/licenses/>
*/

#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "common.h"


/*
*****************************************************************************************
* OPTIONS
*****************************************************************************************
*/
// board type
#define __BOARD_PRO_MINI__          1
#define __BOARD_PRO_MICRO__         2

// controller
#define __CON_OSC__                 1
#define __CON_MSP__                 2


/*
*****************************************************************************************
* CONFIGURATION
*****************************************************************************************
*/
#define __FEATURE_BOARD__           __BOARD_PRO_MICRO__
#define __FEATURE_CONTROLLER__      __CON_MSP__

// debug
#define __FEATURE_DEBUG__           1


/*
*****************************************************************************************
* RULE CHECK
*****************************************************************************************
*/
#if __FEATURE_MOTOR_TEST__ && !__FEATURE_DEBUG__
#undef  __FEATURE_DEBUG__
#define __FEATURE_DEBUG__           1
#endif

#if defined(__AVR_ATmega328P__)
#define __STD_SERIAL__              0
#elif defined(__AVR_ATmega32U4__)
#define __STD_SERIAL__              1
#endif

#if defined(__AVR_ATmega32U4__) && (__FEATURE_BOARD__ != __BOARD_PRO_MICRO__)
#error "Check __FEATURE_BOARD__, it doesn't match with arduino board type"
#endif

#if defined(__AVR_ATmega328P__) && (__FEATURE_BOARD__ != __BOARD_PRO_MINI__)
#error "Check __FEATURE_BOARD__, it doesn't match with arduino board type"
#endif

/*
*****************************************************************************************
* PINS
*****************************************************************************************
*/

//---------------------------------------------------------------------------------------
// ANALOG
#define PIN_ANALOG_VOLT         0       // A0
#define BATT_DIVIDER_R1         40      // 40K Ohm
#define BATT_DIVIDER_R2         20      // 20K Ohm

// wifi module - default firmware (1), tcp2serial firmware (0)
#define __FEATURE_WIFI_DEFAULT__    1


/*
*****************************************************************************************
* COMMON SETTINGS
*****************************************************************************************
*/
#if __FEATURE_WIFI_DEFAULT__
#define SERIAL_BPS              115200
#else
#define SERIAL_BPS              57600
#endif

#endif
