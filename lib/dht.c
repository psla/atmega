#include "dht.h"

#include <avr/io.h>
#include <util/delay.h>

void initDHT(void)
{
    /* Set LED as output */
    SET_BIT(DDRB,LED);
    
    /* According to the DHT11's datasheet, the sensor needs about
       1-2 seconds to get ready when first getting power, so we
       wait
     */
    _delay_ms(3000);
}


uint8_t fetchData(uint8_t* arr)
{
    uint8_t data [5];
    uint8_t cnt, check;
    int8_t i,j;
    
    /******************* Sensor communication start *******************/
    
    /* Set data pin as output first */
    SET_BIT(DDRB,DHT_PIN);
    
    /* First we need milliseconds delay, so set clk/1024 prescaler */
    TCCR0B = 0x05;
    
    TCNT0 = 0;
    
    /* Clear bit for 20 ms */
    CLEAR_BIT(DHT_PORT_OUT,DHT_PIN);
    
    /* Wait about 20 ms */
    while(TCNT0 < 160);
    
    /* Now set Timer0 with clk/8 prescaling.
     Gives 1탎 per cycle @8Mhz */
    TCCR0B = 0x02;
    
    TCNT0 = 0;
    
    /* Pull high again */
    SET_BIT(DHT_PORT_OUT,DHT_PIN);
    
    /* And set data pin as input */
    CLEAR_BIT(DDRB,DHT_PIN);
    
    /* Wait for response from sensor, 20-40탎 according to datasheet */
    while(IS_SET(DHT_PORT_IN,DHT_PIN))
    { if (TCNT0 >= 60) return 0; }
    
    /************************* Sensor preamble *************************/
    
    TCNT0 = 0;
    
    /* Now wait for the first response to finish, low ~80탎 */
    while(!IS_SET(DHT_PORT_IN,DHT_PIN))
    { if (TCNT0 >= 100) return 0; }
    
    TCNT0 = 0;
    
    /* Then wait for the second response to finish, high ~80탎 */
    while(IS_SET(DHT_PORT_IN,DHT_PIN))
    { if (TCNT0 >= 100) return 0; }
    
    /********************* Data transmission start **********************/
    
    for (i = 0; i < 5; ++i)
    {
        for(j = 7; j >= 0; --j)
        {
            TCNT0 = 0;
            
            /* First there is always a 50탎 low period */
            while(!IS_SET(DHT_PORT_IN,DHT_PIN))
            { if (TCNT0 >= 70) return 0; }
            
            TCNT0 = 0;
            
            /* Then the data signal is sent. 26 to 28탎 (ideally)
             indicate a low bit, and around 70탎 a high bit */
            while(IS_SET(DHT_PORT_IN,DHT_PIN))
            { if (TCNT0 >= 90) return 0; }
            
            /* Store the value now so that the whole checking doesn't
             move the TCNT0 forward by too much to make the data look
             bad */
            cnt = TCNT0;
            
            if (cnt >= 20 && cnt <= 35)
            { CLEAR_BIT(data[i],j); }
            
            else if (cnt >= 60 && cnt <= 80)
            { SET_BIT(data[i],j); }
            
            else return 0;
        }
    }
    
    /********************* Sensor communication end *********************/
    
    check = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    
    if (check != data[4]) return 0;
    
    for(i = 0; i < 4; ++i)
    { arr[i] = data[i]; }
    
    return 1;
}

uint8_t data [4];

uint8_t getDht11(dht11data * sensorData) {
	if(!fetchData(data)) {
		return 0;
	}
	
	sensorData->temperature = data[3];
	sensorData->humidity = data[0];	
	return 1;
}

uint8_t getDht22(dht22data * sensorData) {
	if(!fetchData(data)) {
		return 0;
	}
	
	uint16_t temperature;
	uint8_t is_negative = (data[2] & (1 << 7)) != 0;

	// zero sign (so that it is always positive)
	temperature = data[2] & ~(1 << 7);
	temperature <<= 8;
	temperature += data[3];
	sensorData->temperature = (int16_t) temperature;
	if(is_negative) {
		sensorData->temperature *= -1;
	}

	uint16_t humidity;
	humidity = data[0];
	humidity <<= 8;
	humidity += data[1];
	sensorData->humidity = humidity;
	return 1;
}
