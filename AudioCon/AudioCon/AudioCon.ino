#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>
#include "utils.h"
#include "PT2258.h"

#define PIN_IR          2
#define PIN_LED         13
#define PIN_AUX_B0      4
#define PIN_AUX_B1      5
#define PIN_AUX_B2      6

#define REM_VOL_UP      0xFFA857
#define REM_VOL_DOWN    0xFFE01F
#define REM_VOL_MUTE    0xFF906F
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

static Adafruit_SSD1306 mDisp(-1);
static PT2258           mVolCtrl;
static IRrecv           mIR(PIN_IR, PIN_LED);
static u8               mVol = 0;
static u8               mCh  = 0;
static u8               mAux  = 0;
static u32              mLastKey = 0;
static u8               mMute = 0;

static const u8 PROGMEM ICON_MIX[] =
{
    0xff, 0xff,
    0x80, 0x01,
    0x81, 0x09,
    0x81, 0x15,
    0xb1, 0x19,
    0xb1, 0x89,
    0x82, 0x89,
    0x81, 0x09,
    0x81, 0x09,
    0x80, 0x01,
    0x80, 0x01,
    0xb3, 0x9d,
    0xca, 0x65,
    0xda, 0x65,
    0xbb, 0x9d,
    0xff, 0xff
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

static const u8 PROGMEM ICON_MUTE[] =
{
    0xc0, 0x00,
    0xe1, 0x80,
    0x73, 0x80,
    0x3f, 0x80,
    0x1f, 0x80,
    0xff, 0x80,
    0xff, 0x80,
    0xff, 0x80,
    0xff, 0xc0,
    0xff, 0xe0,
    0xff, 0xf0,
    0x1f, 0xb8,
    0x07, 0x9c,
    0x03, 0x8e,
    0x01, 0x87,
    0x00, 0x03
};

static const u8 PROGMEM ICON_AUX[] =
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

void initLCD(void)
{
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    mDisp.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
    drawScreen(mCh, mVol, mAux);
}

void drawScreen(u8 ch, u8 vol, u8 aux)
{
    const u8 len = 128 - 24;

    mDisp.clearDisplay();
    mDisp.setTextSize(2);
    mDisp.setTextColor(WHITE);

    mDisp.drawBitmap(0, 0, ICON_MIX, 16, 16, 1);
    mDisp.setCursor(20, 0);
    mDisp.println(ch + 1, DEC);

    mDisp.drawBitmap(64, 0, ICON_AUX, 16, 16, 1);
    mDisp.setCursor(84, 0);
    mDisp.println(aux + 1, DEC);

    if (vol == 0xff) {
        mDisp.drawBitmap(0, 16, ICON_MUTE, 16, 16, 1);
    } else {
        mDisp.drawBitmap(0, 16, ICON_SPK, 16, 16, 1);
        u8  x = map(vol, 0, 20, 4, len);

        for (u8 i = 0; i < x; i++) {
            u8 y = map(i, 0, len, 1, 16);
            mDisp.drawFastVLine(16 + i, 31 - y, y, 1);
        }
    }
    mDisp.display();
}

void outAux(u8 aux)
{
    digitalWrite(PIN_AUX_B0, (aux & 0x01));
    digitalWrite(PIN_AUX_B1, (aux & 0x02));
    digitalWrite(PIN_AUX_B2, (aux & 0x04));
}

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    while (!Serial);             // Leonardo: wait for serial monitor

    pinMode(PIN_AUX_B0, OUTPUT);
    pinMode(PIN_AUX_B1, OUTPUT);
    pinMode(PIN_AUX_B2, OUTPUT);
    outAux(mAux);

    LOG(F("Start !!\n"));
    mIR.enableIRIn();
    mVolCtrl.init(0x40);
    initLCD();
}

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
                mCh = (mCh + 1) % 3;
                break;

            case REM_CH_DOWN:
                if (mCh > 0)
                    mCh = (mCh - 1) % 3;
                else
                    mCh = 2;
                break;

            case REM_1:
                mAux = 0;
                break;
            case REM_2:
                mAux = 1;
                break;
            case REM_3:
                mAux = 2;
                break;
            case REM_4:
                mAux = 3;
                break;
            case REM_5:
                mAux = 4;
                break;
            case REM_6:
                mAux = 5;
                break;
            case REM_7:
                mAux = 6;
                break;
            case REM_8:
                mAux = 7;
                break;
            case REM_VOL_MUTE:
                mMute = !mMute;
                if (mMute) {
                    mVolCtrl.setMute(1);
                } else {
                    mVolCtrl.setMute(0);
                }
                break;
        }

        if (mMute) {
            drawScreen(mCh, 0xff, mAux);
        } else {
            u8 vol = 79 - map(mVol, 0, 20, 0, 79);
            mVolCtrl.setChannelVolume(mCh, vol);
            drawScreen(mCh, mVol, mAux);
        }
        outAux(mAux);
        mIR.resume();               // Prepare for the next value
    }
}

