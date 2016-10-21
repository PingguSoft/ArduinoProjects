#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>
#include "utils.h"
#include "PT2258.h"

#define OLED_RESET 4

Adafruit_SSD1306 mDisp(OLED_RESET);
PT2258           mVol;
IRrecv           mIR(2, 13);

void initLCD(void)
{
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    mDisp.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

    // init done
    // Show image buffer on the mDisp hardware.
    // Since the buffer is intialized with an Adafruit splashscreen
    // internally, this will mDisp the splashscreen.
    mDisp.display();
    delay(2000);
}

void showResult(u32 result)
{
    mDisp.clearDisplay();

    mDisp.setTextSize(2);
    mDisp.setTextColor(WHITE);
    mDisp.setCursor(0,8);
    mDisp.print("0x"); 
    mDisp.println(result, HEX);
    mDisp.display();
}

void setup()
{
    Wire.begin();
    Serial.begin(115200);
    while (!Serial);             // Leonardo: wait for serial monitor

    LOG(F("Start !!\n"));
    mIR.enableIRIn();
    mVol.init(0x40);
    initLCD();
}
 
void loop()
{
    decode_results  results;        // Somewhere to store the results

    if (mIR.decode(&results)) {     // Grab an IR code
        showResult(results.value);
        Serial.println(results.decode_type, DEC);
        Serial.println(results.value, HEX);
        mIR.resume();               // Prepare for the next value
    }
}
