#include <LiquidCrystal_I2C.h>
#include "Adafruit_NeoPixel.h"
#include "GyverTimer/GyverTimer.h"
#include "GyverButton/GyverButton.h"
#include "math.h"
#include <Arduino.h>

#define LED_COUNT 180
#define LED_PIN 2

LiquidCrystal_I2C oled(0x27, 16, 2);
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
GTimer randomTimer(MS, 1000);
GTimer rainBowTimer(MS, 100);
GButton selectButton(3);

enum StateControl{Default, RandomColor, Rainbow, SelectColor};
enum StateSelect{Red, Green, Blue, Accept};
StateControl stateControl;
StateSelect stateSelect;
byte rgbArr[3] = {255, 255, 255};


void LedSetup(){
    ledStrip.begin();

    for (int i = 0; i < LED_COUNT; i++)
    {
        ledStrip.setPixelColor(i, ledStrip.Color(48, 199, 88));
        ledStrip.show();

        delay(10);
    }

        for (int i = LED_COUNT; i >= 0; i--)
    {
        ledStrip.setPixelColor(i, ledStrip.Color(0, 0, 0));
        ledStrip.show();

        delay(10);
    }
}

uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
        return ledStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else if(WheelPos < 170) {
        WheelPos -= 85;
        return ledStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    } else {
        WheelPos -= 170;
        return ledStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
}

void RainbowLED(){
    if(!rainBowTimer.isEnabled()) rainBowTimer.start();
    if(!rainBowTimer.isReady()) return;

    uint16_t i, j;
    
    for(j=0; j<256*5; j++) {
        for(i=0; i< ledStrip.numPixels(); i++) {
            ledStrip.setPixelColor(i, Wheel(((i * 256 / ledStrip.numPixels()) + j) & 255));
        }
        ledStrip.show();
    }
}

void RandomLED(){
  if(!randomTimer.isEnabled()) randomTimer.start();
  if(!randomTimer.isReady()) return;

  for (int i = 0; i < 3; i++)
  {
    rgbArr[i] = random(0, 256);
  }
  
  for (int i = 0; i < LED_COUNT; i++)
  {
      ledStrip.setPixelColor(i, ledStrip.Color(rgbArr[0], rgbArr[1], rgbArr[2]));
  }

  ledStrip.show();
}

void LedSetColor(){
    for (int i = LED_COUNT; i >= 0; i--)
    {
        ledStrip.setPixelColor(i, ledStrip.Color(rgbArr[0], rgbArr[1], rgbArr[2]));
    }

    ledStrip.show(); 
}

void StateUpdate(){
  selectButton.tick();
  int potentValue = analogRead(1);

  if(stateControl != SelectColor){
    if(potentValue <= 200 && selectButton.isClick()) stateControl = Default;
    else if(potentValue > 200 && potentValue <= 500 && selectButton.isClick()) stateControl = RandomColor;
    else if (potentValue > 500 && potentValue <= 800 && selectButton.isClick()) stateControl = Rainbow;
    else if(potentValue > 800 && selectButton.isClick()) stateControl = SelectColor; stateSelect = Red;
  }
}

void setup() {
  Serial.begin(9600);
  oled.init();
  oled.backlight();
  oled.clear();
  oled.print("Hello!");
  LedSetup();
  oled.clear();
  stateControl = Default;
}

void loop() {
  StateUpdate();

  switch (stateControl)
  {
    case Default:{
      oled.clear();
      oled.print("Default Mode");

      LedSetColor();
    }break;

    case RandomColor:{
      oled.clear();
      oled.print("Random Mode");

      RandomLED();
    }break;

    case Rainbow:{
      oled.clear();
      oled.print("RainBow Mode");

      RainbowLED();
    }break;

    case SelectColor:{
      oled.clear();
      oled.setCursor(0,0);
      oled.print("RED");
      oled.setCursor(5,0);
      oled.print("GREEN");
      oled.setCursor(11,0);
      oled.print("BLUE");

      switch (stateSelect)
      {
        case Red:{
          rgbArr[0] = analogRead(1) / 4;
          if(rgbArr[0] > 255) rgbArr[0] = 255;

          if(selectButton.isClick()) stateSelect = Green;
        }break;

        case Green:{
          rgbArr[1] = analogRead(1) / 4;
          if(rgbArr[1] > 255) rgbArr[1] = 255;

          if(selectButton.isClick()) stateSelect = Blue;
        }break;

        case Blue:{
          rgbArr[2] = analogRead(1) / 4;
          if(rgbArr[2] > 255) rgbArr[2] = 255;

          if(selectButton.isClick()) stateSelect = Accept;
        }break;

        case Accept:{
          stateControl = Default;
        }break;
      }

      oled.setCursor(0,1);
      oled.print(rgbArr[0]);
      oled.setCursor(5,1);
      oled.print(rgbArr[1]);
      oled.setCursor(11,1);
      oled.print(rgbArr[2]);

      LedSetColor();
    }break;
  }
}