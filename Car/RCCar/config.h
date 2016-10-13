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


#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #define CONFIG_CPU_PROMINI
#elif defined(__AVR_ATmega32U4__) || defined(TEENSY20)
  #define CONFIG_CPU_PROMICRO
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
  #define CONFIG_CPU_MEGA
#endif

//#define CONFIG_DBG_SERIAL   Serial
#define CONFIG_CTRL_SERIAL  Serial
#define CONFIG_CTRL_BAUD    57600


#endif
