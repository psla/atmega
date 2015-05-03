#include "dht22.hpp"
#include "RFM69.h"
#include <SPI.h>
#include "dht.h"
#include "LowPower.h"

#define NODEID      2
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_915MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
// TODO - deifne encryption
#define KEY         0 //has to be same 16 characters/bytes on all nodes, not more not less!
#define LED         8
// in case of failure of reading temperature from DHT11, how often should it retry reading
#define RETRY_DELAY 2000

// how often new value of temperature should be measured
#define POLL_DELAY 3000L

RFM69 radio;
uint8_t dhtdata [4];

Payload dataToBeSent;

void setup() {
	blink_repeat(4);
	initDHT();
	Blink(LED, 500);
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	Blink(LED, 500);

	radio.setHighPower(); //uncomment only for RFM69HW! but only if you want to use full power
	radio.setPowerLevel(25);
	radio.encrypt(KEY);

	dataToBeSent.nodeId = NODEID;
       
        blink_repeat(15);
}

void loop() {
	// measure temperature
	digitalWrite(LED, HIGH);
	while(!fetchData(dhtdata)) { delay(RETRY_DELAY); }
	digitalWrite(LED, LOW);
	// temperature is measured, send it to master

	//fill in the struct with new values
	dataToBeSent.nodeId = NODEID;
	dataToBeSent.temperature = dhtdata[2] * ((uint16_t) 10);
	dataToBeSent.humidity = dhtdata[0] * ((uint16_t) 10);
        dataToBeSent.checksum = get_checksum((const Payload *) &dataToBeSent);

	if (radio.sendWithRetry(GATEWAYID, (const void*)(&dataToBeSent), sizeof(dataToBeSent)))
        {
                // for testing you may uncomment line below
		// blink_repeat(4);
        }
	else
	{
		blink_repeat(8);
	}
	digitalWrite(LED,LOW);

	radio.sleep();

	for(int i = 0; i < 6; i++) {
		LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
	}
}

void blink_repeat(uint8_t n) {
	for(uint8_t i = 0; i < n; i++) {
		digitalWrite(LED,LOW);
		delay(70);
		digitalWrite(LED,HIGH);
		delay(70);
	}
}

void Blink(byte PIN, int DELAY_MS)
{
	pinMode(PIN, OUTPUT);
	digitalWrite(PIN,HIGH);
	delay(DELAY_MS);
	digitalWrite(PIN,LOW);
}
