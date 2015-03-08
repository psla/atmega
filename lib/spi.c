void SPI_init(void){
	// set up SPI output for Atmega 328P
	// this means MOSI, SCK and SS!! (otherwise it won't work)
	DDRB = ((1<<DDB2)|(1<<DDB3)|(1<<DDB5)); //spi pins on port b MOSI SCK,SS outputs

	// enable SPI, f/16
	// SPR0 = f/16
	// SPR0 SPI2X = f/8
	// not sure if CPOL and CPHA are needed
	// (clock polarity & clock phase)
	// according to RFM69HW manual
	// http://www.hoperf.com/upload/rf/RFM69HW-V1.3.pdf
	// The SPI interface gives access to the configuration register via a synchronous full-duplex protocol corresponding to CPOL
	// = 0 and CPHA = 0 in Motorola/Freescale nomenclature. Only the slave side is implemented. 
	// SPCR = ((1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<CPOL)|(1<<CPHA));
	
	// enable SPI, f/8
	// this is for 8MHz atmega
	SPCR = ((1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPI2X));
}

char SPI_transmit(char cData){
	SPDR = cData;
	while(!(SPSR & (1<<SPIF)))
		;
	return SPDR;
}
