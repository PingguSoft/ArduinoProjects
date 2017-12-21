#include <Arduino.h>
#include <stdarg.h>
#include "common.h"
#include "config.h"
#include "utils.h"
#include "SerialProtocol.h"

/*
*****************************************************************************************
* CONSTANTS
*****************************************************************************************
*/


/*
*****************************************************************************************
* MACROS
*****************************************************************************************
*/
#define CLR(x,y)                (x&=(~(1<<y)))
#define SET(x,y)                (x|=(1<<y))


/*
*****************************************************************************************
* VARIABLES
*****************************************************************************************
*/




/*
*****************************************************************************************
* setup
*****************************************************************************************
*/
void setup()
{
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(6, INPUT);
    pinMode(7, INPUT);
    pinMode(8, INPUT);
    pinMode(9, INPUT);
    pinMode(10, INPUT);
    pinMode(11, INPUT);
    pinMode(12, INPUT);
    pinMode(A4, INPUT);
    pinMode(A5, INPUT);
    pinMode(A6, INPUT);
    Serial.begin(SERIAL_BPS);
    LOG(F("---- BlueJoyPad ---- \n"));


    // TIMER1 for motor step
    TCCR1A = 0;                                 // Timer1 normal mode 0, OCxA,B outputs disconnected
    TCCR1B = BV(CS11);                          // Prescaler=8, => 2Mhz
    OCR1A  = ZERO_SPEED;
    TCNT1  = 0;
    TIMSK1 |= (BV(OCIE1A);                      // Enable Timer1 interrupt
}


/*
*****************************************************************************************
* loop
*****************************************************************************************
*/
void loop()
{
    u8  pd = ~(PIND >> 2) & 0x3f;
    u8  pb = ~PINB & 0x0f;
    u32 st;

    if (PINC & BV(4)) {
        while(PINC & BV(4));
        st = micros();
        while((PINC & BV(4)) == 0);
        LOG(F("PERIOD:%ld\n"), micros() - st);
    }


//    LOG(F("PD:%02x, PB:%02x, A4:%04d, A5:%04d, A6:%04d\n"), pd, pb, analogRead(4), analogRead(5), analogRead(6));
//    delay(20);
}

/*
*****************************************************************************************
* ISR for TIMER1
*****************************************************************************************
*/
ISR(TIMER1_COMPA_vect)
{
    if (mDirs[MOT_1] == 0) {
        return;
    }

    OCR1A = TCNT1 + mPeriod[MOT_1];
    SET(PORT_MOT_1_STEP, BIT_MOT_1_STEP);
    delay1uS();
    CLR(PORT_MOT_1_STEP, BIT_MOT_1_STEP);
}