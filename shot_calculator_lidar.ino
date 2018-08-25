/* This file was built using various open source libraries, and all licenses used for these libraries apply 
 *  The main body of this was written poorly by Martin Johnson to make a distance measuring radiographic shot calculator for
 *  Iridium 192 and 3 speeds of film being shot for a 2.7-3.1 film density
 */



#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#include <Wire.h>
#include <math.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_ADDRESS 0x3C

#define RST_PIN -1

float inches, i, feet,  rate;
float thk=0.0;
  float ci=0.0;
  float constant=5200;
float rf=2500.0;
float intensity=0.0;
char shot[50];

const int butPin=9;
int butState=0;
float dataTHK=0.0;
float dataFILM=0.0;
float dataCI=90.0;

SSD1306AsciiWire oled;

 float roundto( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}

//------------------------------------------------------------------------------
void setup() {
  Wire.begin();
  Wire.setClock(400000L);
  Serial.begin(115200);
  char showTHK[30];
  int x=1;
  int a=0;
  float t=1.1/1023;
  //float c=102/1023;
  pinMode(butPin,INPUT);
  int potPin;
  
  #if RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
  #else // RST_PIN >= 0
    oled.begin(&Adafruit128x64, I2C_ADDRESS);
  #endif // RST_PIN >= 0

    if (!lox.begin()) {
      Serial.println(F("Failed to boot VL53L0X"));
     // while(1);
    }

    oled.setFont(Adafruit5x7);
  //oled.print("Shot Calculator by:\n Martin Johnson");
  //delay(3000);
   while (butState!=1){
      butState=digitalRead(butPin);
      if (potPin=analogRead(A0)){
        oled.setCursor(1, 1);
        dataCI=round((.11*potPin));
        oled.print("Ci: ");oled.print(dataCI); oled.print("      ");
        //Serial.print("Ci: ");Serial.print(dataCI); Serial.print("      ");
      }else{oled.print("      ");}
   }
   butState=0;
   ci=dataCI;
   oled.clear();
   //delay(1000);
   Serial.println(butState);
   delay(250);
   butState=0;
   while (butState!=1){   
      if (potPin=analogRead(A0)){
        oled.setCursor(1, 1);
        dataTHK=(.00108*potPin);
        oled.print("Thickness: ");oled.print(dataTHK); oled.print("         ");
        Serial.println(butState);
      }else{oled.print("         ");}
      delay(100);
      butState=digitalRead(butPin);
       Serial.println(butState);
   }
   thk=dataTHK;
   butState=0;
   delay(250);
   while(butState!=1){
      if (potPin=analogRead(A0)){
        oled.setCursor(1,1);
        dataFILM=round((potPin*.003));
        if (dataFILM<=3 && dataFILM>2){
          oled.print("FILM: D3 7R"); oled.print("     ");
          rf=7000.0;
        }
        if (dataFILM<=2 && dataFILM>1){
          oled.print("FILM: D5 2.5R"); oled.print("     ");
          rf=2500.0;
        }
        if (dataFILM<=1 && dataFILM>0){
          oled.print("FILM: F8 200mR"); oled.print("     ");
          rf=200.0;
        }
        delay(100);
        butState=digitalRead(butPin);
         Serial.println(butState);
      }
   }
  
   oled.clear();
}
//------------------------------------------------------------------------------

void loop() {
  float hvl=pow(2, (thk/.53));
  intensity=ci*constant;
  float known=intensity/hvl;
  //oled.clear();
  oled.setCursor(1,1);
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4) {  
    inches=(measure.RangeMilliMeter/10)*.3937;
    oled.print("Distance (in): "); oled.println(inches); 
    //oled.setCursor(15, 50);
    feet=pow((inches/12), 2);
    rate=(known/feet)/3600.0;
    i=rf/rate;
   // sprintf(shot, "Shot time: %f", i);
    oled.print("Shot Time: "); oled.print(round(i)); oled.print(" sec     ");
    //Serial.println(ci);
  } else {
    oled.print("out of range                   ");
  }
    
  delay(100);
  
}

