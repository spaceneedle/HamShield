
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
#include <DTMF.h>
#include "RDA.h"
#include "I2Cdev_rda.h"

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

/* goertzel routines */

int sensorPin = A0;
int led = 13;

float n=128.0;
float sampling_rate = 8926.0;
int nochar_count = 0;
float d_mags[8];

DTMF dtmf = DTMF(n,sampling_rate);


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
  radio.setModeTransmit();
  // maybe set PA bias voltage
  Serial.println("configured for transmit");
  radio.setTxSourceMic();
  radio.setRfPower(255); // 30 is 0.5V, which corresponds to 29 dBm out (see RF6886 datasheet)
  
  // configure Arduino LED for
  pinMode(LED_PIN, OUTPUT);

  //Serial.println(radio.readRSSI());
   pinMode(led, OUTPUT);  
  state = MAIN_S;
  print_menu();
  radio.setSQLoThresh(80);
  radio.setSQOn();
}

void loop() {  
  char thischar;
  dtmf.sample(sensorPin);
  dtmf.detect(d_mags,506);
  thischar = dtmf.button(d_mags,1800.);
  if(thischar) { Serial.print(thischar);
  nochar_count = 0;
  #define DEBUG_PRINT
#ifdef DEBUG_PRINT
    for(int i = 0;i < 8;i++) {
      Serial.print("  ");
      Serial.print(d_mags[i]);
    }
    Serial.println("");
#endif
  } else {
    // print a newline 
    if(++nochar_count == 50)Serial.println("");
    // don't let it wrap around
    if(nochar_count > 30000)nochar_count = 51;
  }  
  while (Serial.available()) {
    if (state == FREQ_S) {
      char freq_khz[6];
      int i = 0;
      while(i < 6) {
         if (Serial.available()) {
           freq_khz[i] = Serial.read();
           i++;
         }
      }
      
      // interpret frequency
      uint32_t freq = 0;
      i = 0;
      while (i < 6) {
        uint32_t temp = freq_khz[i] - '0';
        for (int k = 5-i; k > 0; k--) {
          temp = temp * 10;
        }
        freq += temp;
        i++;
      }
      Serial.print("setting frequency to: ");
      Serial.println(freq);
      radio.setFrequency(freq);
      state = MAIN_S;
      
    } else if (state == PWR_S) {
      uint8_t pwr_raw[3];
      int i = 0;
      while(i < 3) {
        if (Serial.available()) {
          pwr_raw[i] = Serial.read();
          i++;
        }
      }
      
      // interpret power
      uint8_t pwr = 0;
      i = 0;
      while (i < 3) {
        uint8_t temp = pwr_raw[i] - '0';
        for (int k = 2-i; k > 0; k--) {
          temp = temp * 10;
        }
        pwr += temp;
        i++;
      }

      Serial.print("Setting power to: ");
      Serial.println(pwr);
      radio.setRfPower(pwr);
      state = MAIN_S;
      
    } else if (state == GPIO_S) {
      uint8_t gpio_raw[2];
      int i = 0;
      while(i < 2) {
        if (Serial.available()) {
          gpio_raw[i] = Serial.read();
          i++;
        }
      }
      uint16_t gpio_pin = gpio_raw[0] - 48; // '0';
      uint16_t gpio_mode = gpio_raw[1] - 48;

      radio.setGpioMode(gpio_pin, gpio_mode);
      state = MAIN_S;      
      
    } else {
      char action = Serial.read();
      if (action == 'r') { // get current state
        state = RX_S;
      } else if (action == 't') { 
        state = TX_S;
      } else if (action == 'f') { 
        state = FREQ_S;
      } else if (action == 'u') { 
        state = UHF_S;
      } else if (action == 'v') { 
        state = VHF_S;
      } else if (action == '1') {
        turn_on(state);
        state = MAIN_S; 
      } else if (action == '0') {
        turn_off(state);
        state = MAIN_S; 
      } else if (action == 'p') {
        state = PWR_S; 
      } else if (action == 'g') {
        state = GPIO_S; 
      } else if (action == 's') {
        int16_t rssi = radio.readRSSI();
        Serial.print("rssi: ");
        Serial.println(rssi); 
      } else if (action == 'i') {
        int16_t vssi = radio.readVSSI();
        Serial.print("vssi: ");
        Serial.println(vssi); 
      }
            
      Serial.println(action);
    }
    Serial.flush();
    print_menu();
  }
}

void turn_off(int dev) {
  switch (dev) {
  case RX_S:
    radio.setRX(0);
    break;
  case TX_S:
    radio.setTX(0);
    break;
  case UHF_S:
    radio.setGpioMode(3, 3); // set GPIO3 high (uhf is active low)
    break;
  case VHF_S:
    radio.setGpioMode(2, 3); // set GPIO2 high (vhf is active low)
    break;
  default:
    break; 
 } 
}

void turn_on(int dev) {
  switch (dev) {
  case RX_S:
    radio.setRX(1);
    break;
  case TX_S:
    radio.setTX(1);
    break;
  case UHF_S:
    radio.setGpioMode(3, 2); // set GPIO3 low (uhf is active low)
    break;
  case VHF_S:
    radio.setGpioMode(2, 2); // set GPIO2 low (uhf is active low)
    break;
  default:
    break; 
 } 
}

void print_menu() {
 Serial.println("MENU");
 switch (state) {
  case MAIN_S:
    Serial.println("select step: [r]x, [t]x, [f]req, [u]hf, [v]hf, [p]wr, [g]pio control, r[s]si, vss[i] ...");
    break;
  case RX_S:
    Serial.println("enter 1 to turn on rx, 0 to turn off rx");
    break;
  case TX_S:
    Serial.println("enter 1 to turn on tx, 0 to turn off tx");  
    break;
  case FREQ_S:
    Serial.println("enter frequency in kHz (ffffff)");
    break;
  case UHF_S:
    Serial.println("enter 1 to turn on uhf, 0 to turn off uhf");  
    break;
  case VHF_S:
    Serial.println("enter 1 to turn on vhf, 0 to turn off vhf");    
    break;
  case PWR_S:
    Serial.println("enter power (raw) (ppp)");
    break;
  case GPIO_S:
    Serial.println("enter GPIO pin and control (no spaces, eg pin 1 mode 3 is 13");
    Serial.println("modes 0 - HiZ, 1 - FCN, 2 - Low, 3 - Hi");
    break;
  default:
    state = MAIN_S;
    break; 
 } 
}
