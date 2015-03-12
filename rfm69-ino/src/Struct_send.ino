#include "RFM69.h"
#include <SPI.h>

#define NODEID      99
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_915MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define KEY         0 //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         8
#define SERIAL_BAUD 115200
#define ACK_TIME    30  // # of ms to wait for an ack

int TRANSMITPERIOD = 300; //transmit a packet to gateway so often (in ms)
byte sendSize=0;
boolean requestACK = false;
RFM69 radio;

typedef struct {
  int           nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
  float         temp;   //temperature maybe?
} Payload;
Payload theData;

void setup() {
  Blink(LED, 500);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  Blink(LED, 500);
  radio.setHighPower(); //uncomment only for RFM69HW!
  radio.encrypt(KEY);
}

long lastPeriod = -1;
void loop() {
  //process any serial input

  //check for any received packets

  int currPeriod = millis()/TRANSMITPERIOD;
  if (currPeriod != lastPeriod)
  {
    //fill in the struct with new values
    theData.nodeId = NODEID;
    theData.uptime = millis();
    theData.temp = 91.23; //it's hot!
    
    if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData), sizeof(theData)))
	Blink(LED, 500);
    else
	Blink(LED, 1500);
	
    Blink(LED,3);
    lastPeriod=currPeriod;
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
