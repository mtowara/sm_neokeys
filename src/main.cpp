#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_NeoKey_1x4.h"
#include "seesaw_neopixel.h"

#include "Keyboard.h"
#include <array>

Adafruit_NeoKey_1x4 neokey;  // Create the NeoKey object
const int key_delay_ms = 150;
const std::array<char,3> keys = {'x','c','v'};

void setup() {  
    // connect to neokeys via I2C 
    neokey.begin(0x30);
  
    // begin keyboad HID
    Keyboard.begin();

    // Pulse all the LEDs on to show we're working
    for (uint16_t i=0; i<neokey.pixels.numPixels(); i++) {
        neokey.pixels.setPixelColor(i, 0x808080); // make each LED white
        neokey.pixels.show();
        delay(100);
    }
    for (uint16_t i=0; i<neokey.pixels.numPixels(); i++) {
        neokey.pixels.setPixelColor(i, 0x000000);
        neokey.pixels.show();
        delay(100);
    }
}

void loop() {
    static std::array<bool,4> pressed = {false,false,false,false};
    static std::array<bool,4> pressed_earlier = pressed;
    static std::array<float,4> bright = {0.0f, 0.0f, 0.0f, 0.0f};
  
    // poll current button state
    uint8_t buttons = neokey.read();

    // Check each button, if pressed, light the matching neopixel
    for(int i=0; i<4; i++){
        pressed_earlier[i] = pressed[i];
        pressed[i] = buttons & (1<<(3-i)); // keys are inverted left to right
    }
  
    if (pressed[0] && !pressed_earlier[0]) {
        Keyboard.press(KEY_LEFT_CTRL);
    } else if (!pressed[0] && pressed_earlier[0]) {
        Keyboard.release(KEY_LEFT_CTRL);
    }

    for(int i=0; i<4; i++){
        neokey.pixels.setPixelColor(i, seesaw_NeoPixel::Color(int(bright[i]),0,0));
        if (pressed[i]){
            bright[i] = 255;
            if(i>0){
                Keyboard.print(keys[i-1]);
                delay(key_delay_ms);
            }
        } else {
            bright[i] = max(0,bright[i]-5);
        }
    }
    neokey.pixels.show();
  
    delay(10);
}