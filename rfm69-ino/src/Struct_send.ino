#include "RFM69.h"
#include <SPI.h>
#include "dht11.h"

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
uint8_t data [4];

typedef struct {
	int           nodeId; //store this nodeId
	uint8_t       temperature;   
	uint8_t       humidity;   
	// these two are most likely going to be 0, as DHT11 does not pass this info
	uint8_t	temperature_decimal;
	uint8_t	humidity_decimal;
} Payload;
Payload dataToBeSent;

void setup() {
	initDHT();
	Blink(LED, 500);
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	Blink(LED, 500);
	// radio.setHighPower(); //uncomment only for RFM69HW!
	radio.encrypt(KEY);

	dataToBeSent.nodeId = NODEID;
}

void loop() {
	while(!fetchData(data)) { delay(RETRY_DELAY); }

	//fill in the struct with new values
	dataToBeSent.temperature = data[2];
	dataToBeSent.temperature_decimal = data[3];
	dataToBeSent.humidity = data[0]; //it's hot!
	dataToBeSent.humidity_decimal = data[1]; //it's hot!

	if (radio.sendWithRetry(GATEWAYID, (const void*)(&dataToBeSent), sizeof(dataToBeSent)))
		Blink(LED, 500);
	else
		Blink(LED, 1500);

	Blink(LED,3);

	// temperature is measured, send it to master
	delay(POLL_DELAY);
	digitalWrite(LED,LOW);
}

void Blink(byte PIN, int DELAY_MS)
{
	pinMode(PIN, OUTPUT);
	digitalWrite(PIN,HIGH);
	delay(DELAY_MS);
	digitalWrite(PIN,LOW);
}
