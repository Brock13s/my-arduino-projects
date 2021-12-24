// All in one matrix display, temperature readout, time readout, and draw a pattern on the matrix display :)

#include <DMD.h>
#include <SPI.h>
#include <DHT.h>
#include <DS1302.h>
#include <TimerOne.h>
#include "SystemFont5x7.h"

#define display_across 1
#define display_down 1
#define temp_sensor A0
#define temp_type DHT11

DHT dht(temp_sensor, temp_type);
DMD dmd(display_across, display_down);
DS1302 rtc(5, 4, 3);
Time t;

unsigned long previousTime = 0;
const long interval = 5000;
char u[8];

byte screen = 0;
byte screenMax = 2;
bool screenChanged = true;
#define TEMPERATURE 0
#define TIME 1
#define PATTERN 2

void scanDMD() {
  dmd.scanDisplayBySPI();
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  Timer1.initialize(1000);
  Timer1.attachInterrupt(scanDMD);
  dmd.clearScreen(true);
  //rtc.setDOW(FRIDAY);
  //rtc.setTime(12, 0, 0); //SET TIMES HERE
  //rtc.setDate(6, 8, 2010);
}

void drawText(char* text) {
  dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
  dmd.drawMarquee(text, strlen(text), (32 * display_across) - 1, 4);
  long start = millis();
  long timer = start;
  boolean ret = false;
  while (!ret) {
    if ((timer + 30) < millis()) {
      ret = dmd.stepMarquee(-1, 0);
      timer = millis();
    }
  }
}

void drawPattern(byte line_direction, byte ypos, byte xpos, byte line_length) {
#define DOWN 1
#define UP 2
#define UP_STRAIGHT 3
#define DOWN_STRAIGHT 4
#define DRAW_BOX 5
  switch (line_direction) {
    case DOWN:
      for (byte x = 0; x <= line_length; x++) {
        dmd.writePixel(ypos + x, xpos + x, GRAPHICS_NORMAL, 1);
        delay(200);
      }
      break;

    case UP:
      for (byte z = 0; z <= line_length; z++) {
        dmd.writePixel(ypos - z, xpos - z, GRAPHICS_NORMAL, 1);
        delay(200);
      }
      break;

    case UP_STRAIGHT:
        for (byte p = 0; p <= line_length; p++) {
          dmd.writePixel(ypos, xpos - p, GRAPHICS_NORMAL, 1);
          delay(200);
        }
        break;
  }
}

void tempReadout() {
  dmd.clearScreen(true);
  dmd.selectFont(SystemFont5x7);
  drawText("Temperature");
  dtostrf(dht.readTemperature(), 1, 0, u);
  dmd.drawChar(21, 4, 'c', GRAPHICS_NORMAL);
  dmd.drawString(9, 4, (u), 2, GRAPHICS_NORMAL);
}

void timeReadout() {
  dmd.drawString(5, 4, "time", 4, GRAPHICS_NORMAL);
  delay(2000);
  dmd.clearScreen(true);
  t = rtc.getTime();
  uint8_t hour12 = t.hour % 12 == 0 ? 12 : t.hour % 12;
  if (hour12 <= 9) {
    dtostrf(hour12, 1, 0, u);
    dmd.drawString(5, 1, (u), 2, GRAPHICS_NORMAL);
    dtostrf(t.min, 2, 0, u);
    dmd.drawString(18, 1, (u), 2, GRAPHICS_NORMAL);
    if (t.min <= 9) {
      dmd.drawChar(17, 1, '0', GRAPHICS_NORMAL);
    }
    dmd.drawChar(12, 1, ':', GRAPHICS_NORMAL);
  }

  else {
    dtostrf(hour12, 2, 0, u);
    dmd.drawString(1, 1, (u), 2, GRAPHICS_NORMAL);
    dtostrf(t.min, 2, 0, u);
    dmd.drawString(20, 1, (u), 2, GRAPHICS_NORMAL);
    if (t.min <= 9) {
      dmd.drawChar(19, 1, '0', GRAPHICS_NORMAL);
    }
    dmd.drawChar(14, 1, ':', GRAPHICS_NORMAL);
  }
  if (t.hour >= 00 && t.hour <= 11) {
    dmd.drawString(11, 9, "am", 2, GRAPHICS_NORMAL);
  }

  else if (t.hour >= 12 && t.hour <= 23) {
    dmd.drawString(11, 9, "pm", 2, GRAPHICS_NORMAL);
  }
}

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= interval) {
    previousTime = currentTime;
    screen++;
    if (screen > screenMax) screen = 0;
    screenChanged = true;
  }

  if (screenChanged) {
    screenChanged = false;
    switch (screen) {
      case TEMPERATURE:
        tempReadout();
        break;
      case TIME:
        timeReadout();
        break;
      case PATTERN: // Draw a cool pattern ;)
        dmd.clearScreen(true);
        drawPattern(DOWN, 0, 0, 16);
        drawPattern(UP_STRAIGHT, 16, 16, 16);
        drawPattern(DOWN, 17, 0, 15);
        //drawPattern(UP, 0, 17, 15);
        break;
    }
  }
}
