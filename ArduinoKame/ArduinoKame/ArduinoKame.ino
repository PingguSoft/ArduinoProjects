#include <Servo.h>
#include <EEPROM.h>
#include "utils.h"
#include "minikame.h"


static MiniKame robot;

void setup()
{
    Serial.begin(115200);
    robot.init();
}

void loop()
{
    u8  ch;

    if (Serial.available()) {
        ch = Serial.read();

        switch(ch) {
            case 'w':
                robot.walk(1,550);
                break;

            case 'a':
                robot.turnL(1,550);
                break;

            case 'd':
                robot.turnR(1,550);
                break;

            case '1':
                robot.pushUp(2,5000);
                break;

            case '2':
                robot.upDown(4,250);
                break;

            case '3':
                robot.jump();
                break;

            case '4':
                robot.hello();
                break;

            case '5':
                robot.frontBack(4,200);
                break;

            case '6':
                robot.dance(2,1000);
                break;

            case '7':
                robot.home();
                break;

            case 't':
                robot.trimming();
                break;
        }
    }
}

