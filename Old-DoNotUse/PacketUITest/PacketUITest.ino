/* 

IMTS 

*/

#include <HAMShield.h>
#include <Wire.h>

HAMShield radio;

byte SendData[29] = {0x86,   0xA2,  0x40,  0x40,  0x40,  0x40,  0x60,  0xAE,  0x64,  0x8C,  0xA6,
0x40,   0x40,   0x68, 0xA4, 0x8A,  0x98,  0x82,   0xB2,  0x40,   0x61,  0x03, 
0xF0,   0x54,   0x65,  0x73, 0x74};

void setup() { 
  Serial.begin(9600);
  Serial.println("starting up..");
  Wire.begin();
  pinMode(9,OUTPUT);
  Serial.print("Radio status: ");
  int result = radio.testConnection();
  Serial.println(result,DEC);
  radio.initialize();                         // setup radio
  radio.dangerMode();                         // turn off band restrictions
  radio.setFrequency(446000);                 // set to first IMTS channel (we are only using one)
  Serial.println("Done with radio setup.");
}

int state = 0;
int ones = 0;
int tones = 0;

void loop() {
  radio.setModeTransmit();
  delay(500);
      sendFlag(); 
  for(int x = 0; x < sizeof(SendData); x++) {
    sendPacket(SendData[x]);
    
  }
  sendFlag(); 
  noTone(9);
  delay(500);
  radio.setModeReceive();
  delay(1000);
}

void sendPacket(uint8_t binary) { 
  for(int x = 0; x < 8; x++) { 
   uint8_t buff = binary >> x & 0b1;
   if(buff == 0) { delayMicroseconds(833); ones++; }
   if(ones == 5) { flip(); ones = 0; }
   if(buff == 1) { flip(); ones = 0; } 
  }
}  
    
void flip() { 
   switch(tones) { 
      case 0: 
          tones = 1;
          tone(9,1200);
          delayMicroseconds(833);
          break;
      case 1:
          tones = 0;
          tone(9,2200);
          delayMicroseconds(833);
          break;
   }
}

void sendFlag() { 
  flip();
  delayMicroseconds(4998);
  flip();
  delayMicroseconds(833);
}
     
  

