
// BlueHAM Proto01 Connection Guide
/**********************
**
**  BlueHAM Proto01 <--> Arduino
**  ADC_SCL              A5
**  ADC_DIO              A4
**  GND                  GND
**  PWM_RF_CTL           D9
**
**  Setting Connections
**  MODE  -> GND
**  SENB  -> GND
**  PDN   -> 3.3V
**  AVDD  -> 5V (note this should be a beefy supply, could draw up to 4As)
**
**
**
**  Pinout information for RadioPeripheral01 Prototype board
**  GPIO0 - 
**  GPIO1 - 
**  GPIO2 - VHF_SEL
**  GPIO3 - UHF_SEL
**  GPIO4 - RX_EN
**  GPIO5 - TX_EN
**  GPIO6 - 
**  GPIO7 - 
**************************/

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"
#include "RDA.h"
// #include "I2Cdev_rda.h"

//typedef enum {
#define  MAIN_S 0
#define  RX_S 1
#define  TX_S 2
#define  FREQ_S 3
#define  UHF_S 4
#define  VHF_S 5
#define  PWR_S 6
#define  GPIO_S 7
//} menu_view;

int state;
int txcount = 0;

// create object for RDA
RDA1846 radio;

#define LED_PIN 13
bool blinkState = false;

void setup() {
  // initialize serial communication
  Serial.begin(115200);
  Serial.println("beginning radio setup");
  
  // join I2C bus (I2Cdev library doesn't do this automatically)
   Wire.begin();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(radio.testConnection() ? "RDA radio connection successful" : "RDA radio connection failed");

  // initialize device
  Serial.println("Initializing I2C devices...");
  radio.initialize(); // initializes automatically for UHF 12.5kHz channel

  Serial.println("setting default Radio configuration");


  // set frequency
  Serial.println("changing frequency");
  
  
  // set band
  // set for 70cm
  radio.setUHF();
  radio.setBand(00); // 00 is 400-520MHz
  radio.setFrequency(446000); // in kHz
  radio.setCtcssFreqToStandard();
  Serial.print("\nCTCSS: ");
  Serial.print(radio.getCtcssFreq(),DEC);
  Serial.print("\n");
  /*
  // set for 2m
  radio.setVHF();
  radio.setBand(3); // 0b11 is 134-174MHz
  radio.setFrequency(154130);
  */
  
  /*
  // set for 1.25m
  radio.setVHF();
  radio.setBand(2); // 10 is 200-260MHz
  radio.setFrequency(220000);
  */
  
/*
  // set to receive
  radio.setModeReceive();
  Serial.print("config register is: ");
  Serial.println(radio.readCtlReg());
  radio.setVolume1(0xF);
  radio.setVolume2(0xF);
  Serial.println(radio.getVolume1());
  Serial.println(radio.getVolume2());
  radio.setRfPower(0);
*/


  // set to transmit
    radio.setModeReceive();
  // maybe set PA bias voltage
  Serial.println("configured for transmit");
  radio.setTxSourceMic();
  radio.setRfPower(255); // 30 is 0.5V, which corresponds to 29 dBm out (see RF6886 datasheet)
  
  // configure Arduino LED for
  pinMode(LED_PIN, OUTPUT);

  //Serial.println(radio.readRSSI());
  // pinMode(led, OUTPUT);  
  radio.setSQLoThresh(80);
  radio.setSQOn();
}

void loop() {  
  if(Serial.available()) { 
    int text = Serial.read();
    switch (text) { 
       case 10:
         if(text == 32) { txcount = 0; }
         break;
       case 0:
         if(text == 32) { 
           radio.setRX(0);
           radio.setTX(1);
           state = 10;
           Serial.print("**TRANSMITTING**");
         }
     }
    txcount++;
    if(state == 10) { 
    if(txcount > 30000) { Serial.print("\nTransmit Off\n");radio.setRX(1); radio.setTX(0); state = 0; txcount = 0; }
    }
  }
}


        
    
