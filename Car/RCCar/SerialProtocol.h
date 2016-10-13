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

#ifndef _SERIAL_PROTOCOL_H_
#define _SERIAL_PROTOCOL_H_

#include "config.h"
#include "utils.h"

#define MAX_PACKET_SIZE 32

class SerialProtocol
{
private:
    // stick
    u16        mLX;
    u16        mLY;
    u16        mRX;
    u16        mRY;
    u8        mButtons;
    HardwareSerial  *mSerial;

    typedef enum
    {
        STATE_IDLE,
        STATE_HEADER_START,
        STATE_HEADER_M,
        STATE_HEADER_ARROW,
        STATE_HEADER_SIZE,
        STATE_HEADER_CMD
    } STATE_T;
    //

    // variables
    u8   mRxPacket[MAX_PACKET_SIZE];

    u8   mState;
    u8   mOffset;
    u8   mDataSize;
    u8   mCheckSum;
    u8   mCmd;

    u8  chkSumTX;
    s8  (*mCallback)(u8 cmd, u8 *data, u8 size, u8 *res);

    void sendResponse(bool ok, u8 cmd, u8 *data, u8 size);
    void evalCommand(u8 cmd, u8 *data, u8 size);
    void putChar2TX(u8 data);

public:
    typedef enum {
        MSP_SET_USER_BUTTON = 51,
        MSP_IDENT  = 100,
        MSP_STATUS = 101,
        MSP_ANALOG = 110,
        MSP_MISC   = 114,
        MSP_SET_RAW_RC = 200,
        MSP_SET_MISC   = 207,
    } MSP_T;

    SerialProtocol(void);
    SerialProtocol(HardwareSerial *serial);
    void init(s8 (*callback)(u8 cmd, u8 *data, u8 size, u8 *res));
    u8   handleRX(void);
    void getStick(u16 *lx, u16 *ly, u16 *rx, u16 *ry);
    u8   getButtons(void);
};

#endif
