#include <Servo.h>
#include <EEPROM.h>
#include "pitches.h"
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


#define BIT_LASER       0


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



#define VBAT_SMOOTH_LEVEL       16
u16         mVoltBuf[VBAT_SMOOTH_LEVEL];
u16         mVoltSum;
u8          mVoltIdx;

u8 getBattVolt(void)
{
    u16 v;
    u16 maxVolt;

    v = analogRead(PIN_ANALOG_VOLT);

    mVoltSum += v;
    mVoltSum -= mVoltBuf[mVoltIdx];
    mVoltBuf[mVoltIdx++] = v;
    mVoltIdx %= VBAT_SMOOTH_LEVEL;

    // max volt = ((r1 + r2) / r2) * 5V
    maxVolt = (BATT_DIVIDER_R1 / BATT_DIVIDER_R2) * 50 + 50;
    return map(mVoltSum / VBAT_SMOOTH_LEVEL, 0, 1023, 0, maxVolt - 6);
}

void setup()
{
    Serial.begin(115200);

    pinMode(PIN_SPEAKER, OUTPUT);
    pinMode(PIN_LASER, OUTPUT);

    pinMode(PIN_MISSILE, OUTPUT);
    digitalWrite(PIN_MISSILE, LOW);

    for (u8 i = 0; i < VBAT_SMOOTH_LEVEL; i++) {
        getBattVolt();
    }

    mRobot.init();

    mSerial.begin(SERIAL_BPS);
    mESP.begin(&mSerial);
    mSerial.registerCallback(mspCallback);
}


void loop()
{
    u8  ch;
    u8  tmp;

    mSerial.handleMSP();

    mCurTS = millis();

    // every 5sec
    if (mCurTS - mLastBattTS > 300) {
        mLastBatt = getBattVolt();
        LOG(F("VOLT:%4d\n"), mLastBatt);

        mLastBattTS = mCurTS;
    }


    if (Serial.available()) {
        ch = Serial.read();

        tmp = mLastAuxBtn;

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

            case 'l':
                tmp = (tmp & BV(BIT_LASER)) ^ (BV(BIT_LASER));
                LOG(F("LASER:%4d\n"), tmp);
                mLastAuxBtn = (mLastAuxBtn & ~(BV(BIT_LASER))) | tmp;
                if (tmp) {
                    analogWrite(PIN_LASER, 255);
                    tone(PIN_SPEAKER, 392, 300);
                } else {
                    analogWrite(PIN_LASER, 0);
                    noTone(PIN_SPEAKER);
                }
                break;

            case 't':
                mRobot.trimming();
                break;
        }
    }
}

