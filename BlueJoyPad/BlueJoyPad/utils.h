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

#ifndef _UTILS_H_
#define _UTILS_H_
#include <Arduino.h>
#include <avr/pgmspace.h>
#include "common.h"
#include "config.h"
#include "SerialProtocol.h"

// Bit vector from bit position
#define BV(bit)     (1 << (bit))
#define min(a,b)    ((a)<(b)?(a):(b))
#define max(a,b)    ((a)>(b)?(a):(b))

u32  rand32_r(u32 *seed, u8 update);
u32  rand32();


template <typename T> void PROGMEM_read(const T * sce, T& dest)
{
    memcpy_P(&dest, sce, sizeof (T));
}

template <typename T> T PROGMEM_get(const T * sce)
{
    static T temp;
    memcpy_P(&temp, sce, sizeof (T));
    return temp;
}


#if __FEATURE_DEBUG__
    #if __STD_SERIAL__
        void LOG(char *fmt, ... );
        void LOG(const __FlashStringHelper *fmt, ... );
        void DUMP(char *name, u8 *data, u16 cnt);
    #else
        #define LOG     SerialProtocol::printf
        #define DUMP    SerialProtocol::dumpHex
    #endif

    #define __PRINT_FUNC__  //LOG(F("%08ld : %s\n"), millis(), __PRETTY_FUNCTION__);
#else
    #define LOG(...)
    #define DUMP(...)
    #define __PRINT_FUNC__
#endif

#endif
