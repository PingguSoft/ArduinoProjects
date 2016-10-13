#include <Arduino.h>
#include <avr/pgmspace.h>
#include "utils.h"
#include "SerialProtocol.h"


SerialProtocol::SerialProtocol(void)
{
    mSerial = &CONFIG_CTRL_SERIAL;
    mSerial->begin(CONFIG_CTRL_BAUD);
}

SerialProtocol::SerialProtocol(HardwareSerial *serial)
{
    mSerial = serial;
}

void SerialProtocol::init(s8 (*callback)(u8 cmd, u8 *data, u8 size, u8 *res))
{
    mLX = 500;
    mLY = 500;
    mRX = 500;
    mRY = 500;
    mState = STATE_IDLE;
    mButtons = 0;
    mCallback = callback;
}


void SerialProtocol::putChar2TX(u8 data)
{
    chkSumTX ^= data;
    mSerial->write(data);
}

void SerialProtocol::sendResponse(bool ok, u8 cmd, u8 *data, u8 size)
{
    putChar2TX('$');
    putChar2TX('M');
    putChar2TX((ok ? '>' : '!'));
    chkSumTX = 0;
    putChar2TX(size);
    putChar2TX(cmd);
    for (u8 i = 0; i < size; i++)
        putChar2TX(*data++);
    putChar2TX(chkSumTX);
}

void SerialProtocol::evalCommand(u8 cmd, u8 *data, u8 size)
{
    static  u8 batt = 0;
    static u16 wmCycleTime = 0;

    u8  buf[22];
    u16 *rc;

    memset(&buf, 0, sizeof(buf));
    switch (cmd) {
        case MSP_IDENT:
            buf[0] = 240;
            buf[1] = 3;
            sendResponse(TRUE, cmd, buf, 7);
            break;

        case MSP_STATUS:
            *((u16*)&buf[0]) = wmCycleTime++;
            sendResponse(TRUE, cmd, buf, 11);
            break;

        case MSP_MISC:
            rc = (u16*)buf;
            rc[2] = 2000;
            rc[3] = 1000;
            rc[4] = 1000;
            buf[18] = 100;
            buf[19] = 110;
            buf[20] = 105;
            buf[21] = 100;
            sendResponse(TRUE, cmd, buf, 22);
            break;

        case MSP_SET_RAW_RC:
            rc = (u16*)data;

            //mLX = min(255, (*rc - 1000) * 10 / 39);   // 1000 -> 255 range, roll
            mLX = (*rc - 1000);
            rc++;
            //mLY = min(255, (*rc - 1000) * 10 / 39);   // 1000 -> 255 range, pitch
            //mLY = 255 - mLY;
            mLY = (*rc - 1000);
            rc++;
            //mRX = min(255, (*rc - 1000) * 10 / 39);   // 1000 -> 255 range, yaw
            mRX = (*rc - 1000);
            rc++;
            //mRY = min(255, (*rc - 1000) * 10 / 39);   // 1000 -> 255 range, throttle
            mRY = (*rc - 1000);
            rc++;

            mButtons &= 0xf0;
            for (u8 i = 0; i < 4; i++) {                // AUX1 - AUX4
                if (*rc++ > 1700)
                    mButtons |= BV(i);
            }
            break;

        case MSP_SET_USER_BUTTON:
            mButtons &= 0x0f;
            mButtons |= ((*data << 4) & 0xf0);          // SW BUTTON 5 - 8
            sendResponse(TRUE, cmd, buf, 0);
            break;
    }

    if (mCallback) {
        s8 ret = (*mCallback)(cmd, data, size, buf);
        if (ret >= 0)
            sendResponse(TRUE, cmd, buf, ret);
    }
}

u8 SerialProtocol::handleRX(void)
{
    u8 ret = 0;
    u8 rxSize = mSerial->available();

    if (rxSize == 0)
        return ret;

    while (rxSize--) {
        u8 ch = mSerial->read();

        switch (mState) {
            case STATE_IDLE:
                if (ch == '$')
                    mState = STATE_HEADER_START;
                break;

            case STATE_HEADER_START:
                mState = (ch == 'M') ? STATE_HEADER_M : STATE_IDLE;
                break;

            case STATE_HEADER_M:
                mState = (ch == '<') ? STATE_HEADER_ARROW : STATE_IDLE;
                break;

            case STATE_HEADER_ARROW:
                if (ch > MAX_PACKET_SIZE) { // now we are expecting the payload size
                    mState = STATE_IDLE;
                    continue;
                }
                mDataSize = ch;
                mCheckSum = ch;
                mOffset   = 0;
                mState    = STATE_HEADER_SIZE;
                break;

            case STATE_HEADER_SIZE:
                mCmd       = ch;
                mCheckSum ^= ch;
                mState     = STATE_HEADER_CMD;
                break;

            case STATE_HEADER_CMD:
                if (mOffset < mDataSize) {
                    mCheckSum           ^= ch;
                    mRxPacket[mOffset++] = ch;
                } else {
                    if (mCheckSum == ch) {
                        ret = mCmd;
                        evalCommand(ret, mRxPacket, mDataSize);
                    }
                    mState = STATE_IDLE;
                    //rxSize = 0;             // no more than one command per cycle
                }
                break;
        }
    }
    return ret;
}

void SerialProtocol::getStick(u16 *lx, u16 *ly, u16 *rx, u16 *ry)
{
    *lx = mLX;
    *ly = mLY;
    *rx = mRX;
    *ry = mRY;
}

u8 SerialProtocol::getButtons(void)
{
    return mButtons;
}

