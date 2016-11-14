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

/*
#  Servos: Pro Micro
#   _________   ________   _________
#  |(14)______)(15)    (6)(______(7)|
#  |__|       |   KAME   |       |__|
#             |          |
#             |          |
#             |          |
#   _________ |          | _________
#  |(16)_____)(10)_____(9)(______(8)|
#  |__|                          |__|
#                  /\
#                  |
#             USBs |
*/

//    board_pins[0] = D1; // front left inner
//    board_pins[1] = D4, // front right inner
//    board_pins[2] = D8; // front left outer
//    board_pins[3] = D6; // front right outer
//    board_pins[4] = D7; // back left inner
//    board_pins[5] = D5; // back right inner
//    board_pins[6] = D2; // back left outer
//    board_pins[7] = D3; // back right outer

#if defined(__AVR_ATmega328P__)
#define PIN_FL_COXA     13
#define PIN_FR_COXA      6
#define PIN_FL_TIBIA    12
#define PIN_FR_TIBIA     7
#define PIN_RL_COXA     10
#define PIN_RR_COXA      9
#define PIN_RL_TIBIA    11
#define PIN_RR_TIBIA     8
#elif defined(__AVR_ATmega32U4__)
#define PIN_FL_COXA     15
#define PIN_FR_COXA      6
#define PIN_FL_TIBIA    14
#define PIN_FR_TIBIA     7
#define PIN_RL_COXA     10
#define PIN_RR_COXA      9
#define PIN_RL_TIBIA    16
#define PIN_RR_TIBIA     8
#endif

//---------------------------------------------------------------------------------------
// ANALOG
#define PIN_ANALOG_VOLT         0       // A0
#define BATT_DIVIDER_R1         10      // 10K Ohm
#define BATT_DIVIDER_R2         10      // 10K Ohm

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
