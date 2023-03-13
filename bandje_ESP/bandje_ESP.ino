
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define stripPin 14
#define stripCount 16  //neopixels wired after ring

#define raveBright 200    //initial RaveMode brightness
#define raveDecrement 20  //raveMode brightness reduction



Adafruit_NeoPixel strip(stripCount, stripPin, NEO_GRB);

//initialising global vars

float N = 30;
int n = ceil(12 * (1 - pow(2.72, -N / 18)));
int raveState = 0;
unsigned int raveHue = 0;
int ravePix = 0;
bool buttonState = 0;
bool buddyState = 0;
uint32_t bright = strip.Color(255, 255, 255);

unsigned long long prevMillis = 0;



void setup() {
  pinMode(32, INPUT);
  pinMode(35, INPUT);
  strip.begin();
  strip.show();
  randomSeed(analogRead(34));
  Serial.begin(9600);
  prevMillis = millis();
}

void loop() {
  buttonState = digitalRead(35);
  buddyState = digitalRead(32);

  prevMillis = millis();

  strip.setBrightness(40);

  if (buttonState) {
    raveState++;
    raveState %= 3;
    while (buttonState) {  //only register on release => prevent going through state increments multiple times
      buttonState = digitalRead(35);
      if (millis() - prevMillis >= 1200) {  //check for hold after 1.2 seconds without delay()
        raveState = 5;
        strip.setBrightness(255);
        strip.fill(bright, 0, stripCount);
        strip.show();
      }
    }
  }

  if (buddyState) {
    N = random(1, 40);
    n = ceil(16 * (1 - pow(2.72, -N / 18)));    //calculate the number of leds lighting up.
    unsigned int color = random(0, 65535);
    for (int i = 0; i < 3; i++) {  //
      for (int j = 0; j < 18; j++) {
        strip.setPixelColor(j, strip.ColorHSV(color, 255, 80));
        strip.setPixelColor((j - 2) % stripCount, 0);
        strip.show();
        delay(50);
      }
    }
    strip.fill(0, 0, stripCount);
    //Find your friends
    for (int i = 0; i < n; i++) {
      strip.setPixelColor(i, strip.ColorHSV(color, 255, 80));
      strip.show();
      delay(50);
    }
    while (buddyState) {  //lock the band when in buddyMode
      buddyState = digitalRead(32);
    }
    if (raveState == 5) {  //don't reconnect into flashlightMode
      raveState = 0;
    }
  }

  switch (raveState) {
    case 0:  //Lights out
      for (int i = 0; i < 18; i++) {
        strip.setPixelColor(i, 0);
      }
      break;
    case 1:  //fast rave
      raveHue += 1000;
      ravePix += 2;
      ravePix %= stripCount * 2;
      for (int i = 0; i < (stripCount / 2); i++) {
        strip.setPixelColor((ravePix / 2) - i, strip.ColorHSV(raveHue, 255, max(raveBright - (raveDecrement * i), 0)));
        strip.setPixelColor(((ravePix / 2) - i + (stripCount / 2)) % stripCount, strip.ColorHSV((raveHue + 16384) % 65536, 255, max(raveBright - (raveDecrement * i), 0)));
      }
      break;
    case 2:  //slow rave
      raveHue += 500;
      ravePix++;
      ravePix %= stripCount * 2;
      for (int i = 0; i < (stripCount / 2); i++) {
        strip.setPixelColor(int(ravePix / 2) - i, strip.ColorHSV(raveHue, 255, max(raveBright - (raveDecrement * i), 0)));
        strip.setPixelColor((int(ravePix / 2) - i + (stripCount / 2)) % stripCount, strip.ColorHSV((raveHue + 16384) % 65536, 255, max(raveBright - (raveDecrement * i), 0)));
      }
      break;

    case 5:  //flashlight
      strip.setBrightness(255);
      strip.fill(bright, 0, stripCount);
      break;
  }
  strip.show();
  delay(30);
}
