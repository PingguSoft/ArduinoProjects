#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>
#include "utils.h"
#include "PT2258.h"

#define OLED_RESET 4

Adafruit_SSD1306 mDisp(OLED_RESET);
PT2258           mVolCtrl;
IRrecv           mIR(2, 13);
u8               mVol = 0;
u8               mCh  = 0;


static const u8 PROGMEM ICON_MIX[] =
{
    0x00, 0x00,
    0x21, 0x84,
    0x21, 0x84,
    0x21, 0x84,
    0x7b, 0xde,
    0x7b, 0xde,
    0x7b, 0xde,
    0x7b, 0xde,
    0x00, 0x00,
    0x73, 0xce,
    0x7b, 0xde,
    0x73, 0xce,
    0x31, 0x8c,
    0x21, 0x84,
    0x21, 0x84,
    0x00, 0x00
};

static const u8 PROGMEM ICON_SPK[] =
{
    0x00, 0x00,
    0x01, 0x80,
    0x03, 0x80,
    0x07, 0x84,
    0x1f, 0x82,
    0xff, 0x9b,
    0xff, 0x8b,
    0xff, 0x89,
    0xff, 0x89,
    0xff, 0x8b,
    0xff, 0x9b,
    0x1f, 0x82,
    0x07, 0x84,
    0x03, 0x80,
    0x01, 0x80,
    0x00, 0x00
};

void initLCD(void)
{
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    mDisp.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
    showResult(mCh + 1, mVol);
}

void showResult(u8 ch, u8 vol)
{
    mDisp.clearDisplay();

    mDisp.setTextSize(2);
    mDisp.setTextColor(WHITE);
    mDisp.drawBitmap(0, 0, ICON_MIX, 16, 16, 1);
    mDisp.setCursor(32, 0);
    mDisp.println(ch + 1, DEC);

    mDisp.drawBitmap(0, 16, ICON_SPK, 16, 16, 1);

    u8  len = 128 - 32;
    u8  x = map(vol, 0, 20, 0, len);

    for (u8 i = 0; i < x; i++) {
        u8 y = map(i, 0, len, 0, 16);
        mDisp.drawFastVLine(32 + i, 31 - y, y, 1);
    }
    mDisp.display();
}

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    while (!Serial);             // Leonardo: wait for serial monitor

    LOG(F("Start !!\n"));
    mIR.enableIRIn();
    mVolCtrl.init(0x40);
    initLCD();
}


// V+ : A3C8EDDB
// V- : F076C13B
// C+ : EE886D7F
// C- : E318261B

#define REM_VOL_UP      0xFFA857
#define REM_VOL_DOWN    0xFFE01F
#define REM_CH_UP       0xFFE21D
#define REM_CH_DOWN     0xFFA25D
#define REM_0           0xFF6897
#define REM_1           0xFF30CF
#define REM_2           0xFF18E7
#define REM_3           0xFF7A85
#define REM_4           0xFF10EF
#define REM_5           0xFF38C7
#define REM_6           0xFF5AA5
#define REM_7           0xFF42BD
#define REM_8           0xFF4AB5
#define REM_9           0xFF52AD

u32 mLastKey = 0;

void loop()
{
    decode_results  results;        // Somewhere to store the results

    if (mIR.decode(&results)) {     // Grab an IR code
        Serial.println(results.decode_type, DEC);
        Serial.println(results.value, HEX);

        if (results.value == 0xFFFFFFFF) {
            if (mLastKey == REM_VOL_UP || mLastKey == REM_VOL_DOWN)
                results.value = mLastKey;
        } else {
            mLastKey = results.value;
        }

        switch (results.value) {
            case REM_VOL_UP:
                if (mVol < 20)
                    mVol++;
                break;

            case REM_VOL_DOWN:
                if (mVol > 0)
                    mVol--;
                break;

            case REM_CH_UP:
                mCh = (mCh + 1) % 8;
                break;

            case REM_CH_DOWN:
                if (mCh > 0)
                    mCh = (mCh - 1) % 8;
                else
                    mCh = 7;
                break;

            case REM_1:
                mCh = 0;
                break;
            case REM_2:
                mCh = 1;
                break;
            case REM_3:
                mCh = 2;
                break;
            case REM_4:
                mCh = 3;
                break;
            case REM_5:
                mCh = 4;
                break;
            case REM_6:
                mCh = 5;
                break;
            case REM_7:
                mCh = 6;
                break;
            case REM_8:
                mCh = 7;
                break;
        }

        showResult(mCh, mVol);
        mIR.resume();               // Prepare for the next value
    }
}

