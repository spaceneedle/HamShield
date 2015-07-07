/*

Touchscreen Ham Radio

*/

#include "Wire.h"
#include "HAMShield.h"
#include <stdint.h>
#include <SeeedTouchScreen.h>
#include <TFTv2.h>
#include <SPI.h>

/* Set up touchscreen for 328 */

#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3 
#define TS_MINX 116*2
#define TS_MAXX 890*2
#define TS_MINY 83*2
#define TS_MAXY 913*2

TouchScreen ts = TouchScreen(XP, YP, XM, YM);
HAMShield radio;
uint8_t state = 0;

void setup() {
  Serial.begin(9600);
  Tft.TFTinit();
}

void loop() { 
  switch (state) { 
    case 0:
        state = 1;
        drawMainDisplay();
        break;
   case 1:
        while(1) { } 
  }
}

void drawMainDisplay() { 
   Tft.drawRectangle(2,2,318,32,BLUE);
   Tft.drawRectangle(2,34,32f,32,BLUE);
   Tft.drawRectangle(34,34,32,32,BLUE);
   Tft.drawRectangle(66,34,32,32,BLUE);
   
}


/*
        
        
  
  Serial.begin(115200);
  Serial.print(";;;;;;;;;;;;;;;;;;;;;;;;;;");
  Wire.begin();
  int result = radio.testConnection();
  Serial.print("*");
  Serial.print(result,DEC);
  Serial.print(";");
  radio.initialize(); // initializes automatically for UHF 12.5kHz channel
  Serial.print("*START;");  
  radio.frequency(freq);
  radio.setVolume1(0xF);
  radio.setVolume2(0xF);
  radio.setModeReceive();
  radio.setTxSourceMic();
  radio.setRfPower(255); // 30 is 0.5V, which corresponds to 29 dBm out (see RF6886 datasheet)
  radio.setSQLoThresh(80);
  radio.setSQOn();
}

void loop() {
  
  if(Serial.available()) { 

    int text = Serial.read();

    switch (state) { 

      case 10:
         if(text == 32) { timer = millis();}
         break;

       case 0:
         switch(text) {
           
           case 32:  // space - transmit
               if(repeater == 1) { radio.frequency(tx); } 
               radio.setRX(0);
               radio.setTX(1);
               state = 10;
               Serial.print("#TX,ON;");
               timer = millis();
               break;
           
           case 63: // ? - RSSI
               Serial.print(":");
               Serial.print(radio.readRSSI(),DEC);
               Serial.print(";");
               break;
             
           case 65: // A - CTCSS In
               getValue();
               ctcssin = atof(cmdbuff);
               radio.setCtcss(ctcssin);
               break;
           
           case 66: // B - CTCSS Out
               break;
            
           case 67: // C - CTCSS Enable
               break;
               
           case 68: // D - CDCSS Enable
               break;
               
           case 70: // F - frequency
               getValue();
               freq = atol(cmdbuff);
               if(radio.frequency(freq) == true) { Serial.print("@"); Serial.print(freq,DEC); Serial.print(";!;"); } else { Serial.print("X1;"); } 
               break;
               
           case 80: // P - power level
               getValue();
               temp = atol(cmdbuff);
               radio.setRfPower(temp);
               break;
           
           case 82: // R - repeater offset mode
               getValue();
               temp = atol(cmdbuff);
               if(temp == 0) { repeater = 0; }
               if(temp == 1) { repeater = 1; }
               break;
           
           case 83: // S - squelch
               getValue();
               temp = atol(cmdbuff);
               radio.setSQLoThresh(temp);
               break;
           
           case 84: // T - transmit offset 
               getValue();
               tx = atol(cmdbuff);
               break;
      
           
           case 94: // ^ - VSSI (voice) level
               Serial.print(":");
               Serial.print(radio.readVSSI(),DEC); 
               Serial.print(";");
         }
        break;
     }

  }
      if(state == 10) { 
    if(millis() > (timer + 500)) { Serial.print("#TX,OFF;");radio.setRX(1); radio.setTX(0); if(repeater == 1) { radio.frequency(freq); }  state = 0; txcount = 0; }
    }
}

void getValue() {
  int p = 0;
  char temp;
  for(;;) {
     if(Serial.available()) { 
        temp = Serial.read();
        if(temp == 59) { cmdbuff[p] = 0; Serial.print("@");
           for(int x = 0; x < 32; x++) {  Serial.print(cmdbuff[x]); }
         return;
        }
        cmdbuff[p] = temp;
        p++;
        if(p == 32) { 
         Serial.print("@");
           for(int x = 0; x < 32; x++) { 
             Serial.print(cmdbuff[x]);
           } 
          
          cmdbuff[0] = 0; 

        Serial.print("X0;"); return; }      // some sort of alignment issue? lets not feed junk into whatever takes this string in
     }
  }
}
*/


