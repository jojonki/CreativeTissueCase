// Serial
#include "Wire.h"

// I2C MPU6050
#include "I2Cdev.h"
#include "MPU6050.h"

// LED, Adafruit
#include <Adafruit_NeoPixel.h>

// I2C MPU6050
#define MAX_VAL 13684.0
#define UP_TH 50.0
#define DOWN_TH 0.0
MPU6050 accelgyro;
boolean isUp = false;
int16_t ax, ay, az;
int16_t gx, gy, gz;
float xdeg;
float ydeg;

// LED
#define TIMER_LED_PIN 6
#define AMBIENT_LED_PIN 11
Adafruit_NeoPixel timer_strip = Adafruit_NeoPixel(16, TIMER_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ambient_strip = Adafruit_NeoPixel(30, AMBIENT_LED_PIN, NEO_GRB + NEO_KHZ800);

// Servo
#define SERVO_PIN 10
#include <Servo.h>
Servo servo;

void setup() {
  accelgyro.initialize();
  
  timer_strip.begin();
  ambient_strip.begin();

  timer_strip.show();
  ambient_strip.show();
  
  servo.attach(SERVO_PIN);
  servo.write(120);
  
  Serial.begin(38400);
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

void loop() {
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  xdeg = 90.0 * (ax / MAX_VAL);
  ydeg = 90.0 * (ay / MAX_VAL);
  Serial.print(xdeg); Serial.print("\t"); Serial.println(ydeg);

  if(xdeg > 30) {
    isUp = true;
  } else if(xdeg < 0) {
    if(isUp) { // detect a tissue action
      servo.detach();
      rainbowCycle(2);
      ambient_strip.show();
      uint16_t i;
      for(i=0; i< ambient_strip.numPixels(); i++) {
        ambient_strip.setPixelColor(i, ambient_strip.Color(0, 0, 0));
      }
      ambient_strip.show();
      servo.attach(SERVO_PIN);
      servo.write(0);
      delay(400);
      servo.detach();
      timer(10);
      servo.attach(SERVO_PIN);
      servo.write(120);
      isUp = false;
      delay(100);
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return ambient_strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return ambient_strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return ambient_strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// ambient light animation
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  float alpha = 1.0;
  int fadeCount = 5;
  for(j=0; j<256; j++) {
    for(i=0; i< ambient_strip.numPixels(); i++) {
      ambient_strip.setPixelColor(i, Wheel(((i * 256 / ambient_strip.numPixels()) + j) & 255));
    }
    ambient_strip.setBrightness((int)(255.0 * alpha));
    ambient_strip.show();
    alpha *= 0.98;
    delay(wait);
  }
}

// timer animation
void timer(uint8_t wait) {  
  uint16_t i, j;

  float alpha = 1.0;
  int fadeCount = 5;
  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< timer_strip.numPixels(); i++) {
      timer_strip.setPixelColor(i, Wheel(((i * 256 / timer_strip.numPixels()) + j) & 255));
    }
    timer_strip.setBrightness((int)(255.0 * alpha));
    timer_strip.show();
    alpha *= 0.975;
    delay(wait);
  }
}
