#include "LedControl.h"       //Use of the MAX7219 implemented.

const int digits = 4;          // 4 digits 7 segment display
const int DIPin = 0;
const int LoadPin = 1;
const int ClkPin = 2;
const int sensorPin = 4;      //Hall effect sensor

const double pi = 3.14159;
const int magnets = 2;         //1 magnet in the application
const double radius = 0.3556;      //28 inch wheel -> 14 inch radius ~0.3556 m

double speed;
long double prevTime = 0;     //Previous time

LedControl lc = LedControl(DIPin, ClkPin, LoadPin, 1); // Define the SPI bus used ot control the 4* 7 Segment display

void setup() {
  pinMode(sensorPin, INPUT);
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);
  prevTime = millis();
  speed = 0;
}

double calculateSpeed(double deltaT) {
  if (deltaT == 0) {
    return 0;
  }
  return (((2 * radius * pi) / deltaT) * 3600);        // 1 m.ms-1 = 3600 km.h-1
}

void display(double value) {
  if (value > 99.99) {
    display(99.99);
  }
  else if (value < 0) {
    display(-value);
  }
  else {
    int val = value * 100;
    byte units[digits];        // tens, ones, tenths, hundredths;
    for (int i = 0; i < digits; i++) {
      units[i] = (byte)(val % 10);
      val = val / 10;
    }
    lc.setDigit(0, 0, units[3], false);
    lc.setDigit(0, 1, units[2], true);
    lc.setDigit(0, 2, units[1], false);
    lc.setDigit(0, 3, units[0], false);
  }
}

void loop() {
  //if the magnet goes in front of the sensor, calculate speed and display it
  if (!digitalRead(sensorPin)) {
    long double deltaT = magnets * (millis() - prevTime); // lets consider this approximation
    prevTime = millis();
    speed = calculateSpeed(deltaT);
    while (!digitalRead(sensorPin))
	{	
	  lc.shutdown(0, false);//powerOn the 7 segment display
      display(speed);
	}
  }
  else if (millis() - prevTime > 6000 / magnets) {
   lc.shutdown(0, true);	//powerOff the 7 segment display
  }
}
