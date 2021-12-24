#include <DMD.h>
#include <SPI.h>
#include <DHT.h>
#include <DS1302.h>
#include <TimerOne.h>
#include "SystemFont5x7.h"

#define display_across 1
#define display_down 1

DMD dmd(display_across, display_down);
DS1302 rtc(3, 4, 5);
Time t;

unsigned long previousTime = 0;
bool screenChanged = true;
byte screenMax = 1;
byte screen = 0;
byte ChristmasDay = 25;
byte daysLeft = 0;
char dmdText[2];

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

void scanDMD(){
  dmd.scanDisplayBySPI();
}
void setup() {
  rtc.halt(false);
  
  Serial.begin(9600);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(scanDMD);
  //dmd.clearScreen(true);
}

void loop() {
  String dateString = rtc.getDateStr();
  byte dayy = dateString.toInt();
  char* monthh = rtc.getMonthStr(); 
  
  unsigned long currentTime = millis();
  static unsigned long previousTime = 0;
  if (currentTime - previousTime >=1000){
    previousTime = currentTime;
    screen++;
    if (screen > screenMax) screen = 0;
    screenChanged = true;
  }

  if (!strcmp(monthh, "December")){
    while (dayy == 25){
          drawText("MERRY CHRISTMAS!");
          if (dayy !=25){
            break;
          }
        }
  }
  if(screenChanged){
    screenChanged = false;
    switch (screen) {
      case 0:
      dmd.clearScreen(true);
      dmd.selectFont(SystemFont5x7);
      drawText("Days till christmas");
      break;
      case 1: 
      dmd.clearScreen(true);
      dmd.selectFont(SystemFont5x7);
      if (!strcmp(monthh, "December")){
        daysLeft = ChristmasDay - dayy;
        dmd.selectFont(SystemFont5x7);
        itoa(daysLeft, dmdText, 3);
        dmd.drawString(13, 5, dmdText, 1, GRAPHICS_NORMAL);
        delay(5000);
      }

      else {
        drawText("Sorry mate it aint december :(");
      }
      break;
    }
  }
}
