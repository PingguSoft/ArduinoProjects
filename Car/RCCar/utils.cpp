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

#include <stdarg.h>
#include <EEPROM.h>
#include "config.h"
#include "common.h"
#include "utils.h"

#ifdef CONFIG_DBG_SERIAL
void printf(char *fmt, ... )
{
    char buf[128]; // resulting string limited to 128 chars
    va_list args;
    va_start (args, fmt );
    vsnprintf(buf, 128, fmt, args);
    va_end (args);
    CONFIG_DBG_SERIAL.print(buf);
}

void printf(const __FlashStringHelper *fmt, ... )
{
    char buf[128]; // resulting string limited to 128 chars
    va_list args;
    va_start (args, fmt);
#ifdef __AVR__
    vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
#else
    vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
#endif
    va_end(args);
    CONFIG_DBG_SERIAL.print(buf);
}
#else
void printf(char *fmt, ... )
{
}
void printf(const __FlashStringHelper *fmt, ... )
{
}
#endif

void Utils::dumpEEPROM(u16 addr, u16 cnt)
{
    u8  i;
    u8  b;

    while (cnt) {
        printf(F("%08x - "), addr);

        for (i = 0; (i < 16) && (i < cnt); i ++) {
            b = EEPROM.read(addr + i);
            printf(F("%02x "), b);
        }

        printf(F(" : "));
        for (i = 0; (i < 16) && (i < cnt); i ++) {
            b = EEPROM.read(addr + i);
            if ((b > 0x1f) && (b < 0x7f))
                printf(F("%c"), b);
            else
                printf(F("."));
        }
        printf(F("\n"));
        addr += i;
        cnt  -= i;
    }
}

