#include "RFM69.h"
#include <SPI.h>
#include "dht.h"
#include "LowPower.h"

#define NODEID      3
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

RFM69 radio(RF69_SPI_CS, RF69_IRQ_PIN, true, RF69_IRQ_NUM);
dht22data sensorData;


typedef struct {
	uint8_t  nodeId; //store this nodeId
	int16_t  temperature;   
	int16_t  humidity;   
        uint8_t  checksum;
} Payload;
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

/** 
 * returns a checksum of payload
 *
 * checksum is computed by adding all bytes together as unsigned integer
 */
uint8_t get_checksum(const Payload* data) {
  // node Id
  checksum += *((const uint8_t*)data);
  
  // temperature
  checksum += *(((const uint8_t*)data) + 1);
  checksum += *(((const uint8_t*)data) + 2);

  // humidity
  checksum += *(((const uint8_t*)data) + 3);
  checksum += *(((const uint8_t*)data) + 4);

  return checksum;
}

void loop() {
	// measure temperature
        uint8_t retries_before_error = 3;
	while(!getDht22(&sensorData)) { 
                if(retries_before_error > 0) {
                  digitalWrite(LED, HIGH);
                  --retries_before_error;
                }

		delay(RETRY_DELAY);
		digitalWrite(LED, LOW);
	}

	// temperature is measured, send it to master
	// fill in the struct with new values
	dataToBeSent.nodeId = NODEID;
	dataToBeSent.temperature = sensorData.temperature;
	dataToBeSent.humidity = sensorData.humidity;
        dataToBeSent.checksum = get_checksum((const Payload *) &dataToBeSent);

	if (radio.sendWithRetry(GATEWAYID, (const void*)(&dataToBeSent), sizeof(dataToBeSent)))
        {
                // for testing you may uncomment line below
		// blink_repeat(1);
        }
	else
        {
		blink_repeat(8);
        }

	digitalWrite(LED,LOW);

	radio.sleep();

	// send data roughly once every minute
	for(int i = 0; i < 6; i++) {
		LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
	}
}

void blink_repeat(uint8_t n) {
	for(uint8_t i = 0; i < n; i++) {
		digitalWrite(LED, LOW);
		delay(350);
		digitalWrite(LED, HIGH);
		delay(150);
	}
}

void Blink(byte PIN, int DELAY_MS)
{
	pinMode(PIN, OUTPUT);
	digitalWrite(PIN,HIGH);
	delay(DELAY_MS);
	digitalWrite(PIN,LOW);
}
