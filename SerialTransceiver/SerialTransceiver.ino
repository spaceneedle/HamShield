/*

SerialTransceiver is TTL Serial port "glue" to allow desktop or laptop control of the HAMShield

Commands:

Mode           ASCII       Description                                                                                                                                  Implemented
-------------- ----------- -------------------------------------------------------------------------------------------------------------------------------------------- -----------------
Transmit       space       Space must be received at least every 500 mS                                                                                                 Yes
Receive        not space   If space is not received and/or 500 mS timeout of space occurs, unit will go into receive mode                                               Yes
CTCSS In       A<tone>;    <tone> must be a numerical ascii value with decimal point indicating CTCSS receive tone required to unsquelch                                No
CTCSS Out      B<tone>;    <tone> must be a numerical ascii value with decimal point indicating CTCSS transmit tone                                                     No
CTCSS Enable   C<state>;   Turns on CTCSS mode (analog tone) with 1, off with 0.                                                                                        No
CDCSS Enable   D<state>;   Turns on CDCSS mode (digital tone) with 1, off with 0.                                                                                       No
Bandwidth      E<mode>;    for 12.5KHz mode is 0, for 25KHz, mode is 1                                                                                                  No
Frequency      F<freq>;    Set the receive frequency in KHz, if offset is disabled, this is the transmit frequency                                                      No
CDCSS In       G<code>;    <code> must be a valid CDCSS code                                                                                                            No
CDCSS Out      H<code>;    <code> must be a valid CDCSS code                                                                                                            No
Print tones    I           Prints out all configured tones and codes, coma delimited in format: CTCSS In, CTCSS Out, CDCSS In, CDCSS Out                                No
Power level    P<level>;   Set the power amp level, 0 = lowest, 255 = highest                                                                                           No
Enable Offset  R<state>;   1 turns on repeater offset mode, 0 turns off repeater offset mode                                                                            No
Squelch        S<level>;   Set the squelch level                                                                                                                        No
TX Offset      T<freq>;    The absolute frequency of the repeater offset to transmit on in KHz                                                                          No
Volume         V<level>;   Set the volume level of the receiver                                                                                                         No
Reset          X           Reset all settings to default                                                                                                                No
Sleep          Z           Sleep radio                                                                                                                                  No
Filters        @<state>;   Set bit to enable, clear bit to disable: 0 = pre/de-emphasis, 1 = high pass filter, 2 = low pass filter (default:  ascii 7, all enabled)     No
Vox mode       $<state>;   0 = vox off, >= 1 audio sensitivity. lower value more sensitive                                                                              No
Mic Channel    *<state>;   Set the voice channel. 0 = signal from mic or arduino, 1 = internal tone generator                                                           No
RSSI           ?           Respond with the current receive level in - dBm (no sign provided on numerical response)                                                     No
Tone Gen       % (notes)   To send a tone, use the following format: Single tone: %1,<freq>,<length>; Dual tone: %2,<freq>,<freq>,<length>; DTMF: %3,<key>,<length>;    No



Responses:

Condition    ASCII      Description
------------ ---------- -----------------------------------------------------------------
Startup      *<code>;   Startup and shield connection status
Success      !;         Generic success message for command that returns no value
Error        X<code>;   Indicates an error code. The numerical value is the type of error
Value        :<value>;  In response to a query
Status       #<value>;  Unsolicited status message

*/

#include "Wire.h"
#include "RDA.h"

int state;
int txcount = 0;

RDA1846 radio;



void setup() {
  Serial.begin(115200);
  Serial.print(";;;;;;;;;;;;;;;;;;;;;;;;;;");
  Wire.begin();
  int result = radio.testConnection();
  Serial.print("*");
  Serial.print(result,DEC);
  Serial.print(";");
  radio.initialize(); // initializes automatically for UHF 12.5kHz channel
  Serial.print("*START;");  
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
    switch (text) { 
       case 10:
         if(text == 32) { txcount = 0; }
         break;
       case 0:
         if(text == 32) { 
           radio.setRX(0);
           radio.setTX(1);
           state = 10;
           Serial.print("#TX,ON;");
         }
     }
    txcount++;
    if(state == 10) { 
    if(txcount > 30000) { Serial.print("#TX,OFF;");radio.setRX(1); radio.setTX(0); state = 0; txcount = 0; }
    }
  }
}


        
    
