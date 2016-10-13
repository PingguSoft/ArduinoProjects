#include <Arduino.h>
#include <Servo.h>
#include <math.h>
#include "utils.h"
#include "SerialProtocol.h"

#define PIN_PWM     6
#define PIN_IN1     7
#define PIN_IN2     8
#define PIN_STEER   9
#define PIN_L_LED   A2
#define PIN_R_LED   A3

#define DIR_STOP    0x00
#define DIR_FWD     0x10
#define DIR_REV     0x20
#define DIR_MASK    0x30
#define DIR_LEFT    0x01
#define DIR_RIGHT   0x02

#define DEG_CENTER  90
#define DEG_LAMOUNT  45
#define DEG_RAMOUNT  50
#define DEG_SIGNAL  20

static Servo            servoSteer;
static SerialProtocol   *ctrl;
static uint8_t          mDir = DIR_STOP;

void setDir(int dir, int angle)
{
    uint8_t in1;
    uint8_t in2;

    if (dir & DIR_FWD) {
        in1 = HIGH;
        in2 = LOW;
    } else if (dir & DIR_REV) {
        in1 = LOW;
        in2 = HIGH;
    } else {
        in1 = LOW;
        in2 = LOW;
    }
    digitalWrite(PIN_IN1, in1);
    digitalWrite(PIN_IN2, in2);
    servoSteer.write(angle);
}

void setSpeed(int speed)
{
    analogWrite(PIN_PWM, speed);
}

static u8 scale = 30;
s8 inputCallback(u8 cmd, u8 *data, u8 size, u8 *res)
{
    s8 ret = -1;

    switch (cmd) {
        case SerialProtocol::MSP_ANALOG:
/*
            if (core) {
                u8 *ptr = (u8*)res;

                *ptr = core->getBattLevel(scale);
                *(ptr + 3) = core->getBattLevel();
                ret = 7;
            }
*/
            break;

        case SerialProtocol::MSP_SET_MISC:
            scale = *(data + 18);
            ret = 0;
            break;

        case SerialProtocol::MSP_SET_RAW_RC:
            {
                int  angle;
                u16  btn, lx, ly, rx, ry;

                mDir = DIR_STOP;

                ctrl->getStick(&lx, &ly, &rx, &ry);
                btn = ctrl->getButtons();

                if (btn & 0x01) {
                    ry = ly;
                    rx = lx;
                } else if (btn & 0x02) {
                    rx = lx;
                }

                if (ry > 500) {
                    mDir = DIR_FWD;
                    ry = constrain(ry - 500, 0, 500);
                    ry = map(ry, 0, 500, 50, 254);
                } else if (ry < 500) {
                    mDir = DIR_REV;
                    ry = constrain(-(ry - 500), 0, 500);
                    ry = map(ry, 0, 500, 50, 254);
                } else {
                    mDir = DIR_STOP;
                    ry = 0;
                }
                angle = DEG_CENTER + map(rx, 0, 1000, -DEG_LAMOUNT, DEG_RAMOUNT);
                if (angle >= DEG_CENTER + DEG_SIGNAL) {
                    mDir |= DIR_RIGHT;
                } else if (angle <= DEG_CENTER - DEG_SIGNAL) {
                    mDir |= DIR_LEFT;
                }
                setDir(mDir, angle);
                setSpeed(ry);
            }
            break;
    }
    return ret;
}

void setup()
{
    pinMode(PIN_PWM, OUTPUT);
    pinMode(PIN_IN1, OUTPUT);
    pinMode(PIN_IN2, OUTPUT);
    pinMode(PIN_L_LED, OUTPUT);
    pinMode(PIN_R_LED, OUTPUT);


    setDir(DIR_FWD, 0);
	servoSteer.attach(PIN_STEER);
    servoSteer.write(DEG_CENTER);

    ctrl = new SerialProtocol();
	ctrl->init(inputCallback);

    digitalWrite(PIN_L_LED, HIGH);
    digitalWrite(PIN_R_LED, HIGH);

//	Serial.begin(57600);
//	while (!Serial);
//  printf(F("angle : %d\n"), angle);
}


static uint32_t mLastTime = 0;
static uint8_t  mToggle = 0;
void loop()
{
    uint32_t time;

    time = millis();
    if (time - mLastTime > 300) {
        if (mToggle) {
            if ((mDir & DIR_REV) || mDir == DIR_STOP) {
                digitalWrite(PIN_L_LED, HIGH);
                digitalWrite(PIN_R_LED, HIGH);
            } else if (mDir & DIR_LEFT) {
                digitalWrite(PIN_L_LED, HIGH);
            } else if (mDir & DIR_RIGHT) {
                digitalWrite(PIN_R_LED, HIGH);
            }
        } else {
            if (mDir & DIR_FWD) {
                digitalWrite(PIN_L_LED, LOW);
                digitalWrite(PIN_R_LED, LOW);
            }
        }
        mToggle = !mToggle;
        mLastTime = time;
    }
    ctrl->handleRX();
}

