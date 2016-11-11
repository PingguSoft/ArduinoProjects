#include <Servo.h>
#include <EEPROM.h>
#include "utils.h"
#include "minikame.h"
#include "esp8266.h"

static MiniKame         mRobot;
static ESP8266          mESP;
static SerialProtocol   mSerial;

static u32              mCurTS;
static u32              mLastTS;
static u32              mLastBattTS;
static u32              mLastHeartTS;
static u16              mCycleTime = 0;
static u16              mErrCtr = 0;
static u8               mLastBatt;
static bool             mIsShutdown = false;
static u8               mLastAuxBtn = 0;

/*
*****************************************************************************************
* mspCallback
*****************************************************************************************
*/
s8 mspCallback(u8 cmd, u8 *data, u8 size, u8 *res)
{
    u16 *rc;
    u32 *ptr;
    u16 val;
    s8  ret = -1;
    s16 dist;

    DUMP("MSP", data, size);

    switch (cmd) {

        case SerialProtocol::MSP_ANALOG:
            res[0] = mLastBatt;
            ret = 7;
            break;

        case SerialProtocol::MSP_STATUS:
            *((u16*)&res[0]) = mCycleTime;
            *((u16*)&res[2]) = mErrCtr;
            *((u32*)&res[6]) = mIsShutdown ? 0 : 1;
            ret = 11;
            break;

        case SerialProtocol::MSP_ALTITUDE:
            ptr = (u32*)res;
#if 0
            dist = mRobotAux.getDist(0);
            if (dist > 0)
                *ptr = dist;
            else
                *ptr = 0;
#endif
            *ptr = 0;
            ret = 6;
            break;

        case SerialProtocol::MSP_SET_RAW_RC:
            rc = (u16*)data;

            // roll
            val = (*rc++ - 1000);

            // pitch
            val = (*rc++ - 1000);

            // yaw
            val = (*rc++ - 1000);

            // throttle
            val = (*rc++ - 1000);

            // AUX1 - AUX4
            val = 0;
            for (u8 i = 0; i < 4; i++) {
                if (*rc++ > 1700)
                    val |= BV(i);
            }
            if (val != mLastAuxBtn) {
                if (val & BV(0)) {

                } else {

                }

                mIsShutdown = val & BV(3);
                if (mIsShutdown) {
                }
                mLastAuxBtn  = val;
            }
            break;
    }

    return ret;
}

void setup()
{
    Serial.begin(115200);
    mRobot.init();

    mSerial.begin(SERIAL_BPS);
    mESP.begin(&mSerial);
    mSerial.registerCallback(mspCallback);
}

void loop()
{
    u8  ch;

    mSerial.handleMSP();

    mCurTS = millis();

    // every 5sec
    if (mCurTS - mLastBattTS > 5000) {
//        mLastBatt = mRobotAux.getBattVolt();
        LOG(F("VOLT:%4d\n"), mLastBatt);
        mLastBattTS = mCurTS;
    }


    if (Serial.available()) {
        ch = Serial.read();

        switch(ch) {
            case 'w':
                mRobot.walk(1,550);
                break;

            case 'a':
                mRobot.turnL(1,550);
                break;

            case 'd':
                mRobot.turnR(1,550);
                break;

            case '1':
                mRobot.pushUp(2,5000);
                break;

            case '2':
                mRobot.upDown(4,250);
                break;

            case '3':
                mRobot.jump();
                break;

            case '4':
                mRobot.hello();
                break;

            case '5':
                mRobot.frontBack(4,200);
                break;

            case '6':
                mRobot.dance(2,1000);
                break;

            case '7':
                mRobot.home();
                break;

            case 't':
                mRobot.trimming();
                break;
        }
    }
}

