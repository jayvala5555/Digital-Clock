#include <avr/io.h>
#include <util/delay.h>
uint8_t ss,mm,hh; 

#define F_CPU 8000000            
#define SCL_CLOCK  100000        //100 kHz

void i2c_start(void)
{
	TWBR = 72;
	TWSR = (0<<TWPS0) | (0<<TWPS1);   
	
	TWCR = 0x00;
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);     // Necessary conditions for Initiating Start
	
	while (!(TWCR & (1<<TWINT)));               // Bit Monitoring
	
	
	// Should a line confirming value of TWSR be added ??
	
	// Shouldn't we check for Repeated start condition ??
	
	while((TWSR & 0xF8) != 0x08);

	// Start Function Completed

}

void i2c__MasterTransmission(void)
{
	
	// put it in master transmission mode
	
	TWDR = 0xD0;   
	
	// Manipulate control Register
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	while((TWSR & 0xF8) != 0x18);
}


void data_transmission(uint8_t data)  // Used by the slave to give data
{
	TWDR = data;
	
	TWCR = (1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	
	while((TWSR & 0xF8) != 0x28);
	
}

void i2c_MasterRecieving(void)
{
	TWDR = 0xD1;               // Standard Value
	
	TWCR = (1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	
	while((TWSR & 0xF8) != 0x40);
	
	
}

uint8_t data_reception(void)
{
	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN);     // Necessary conditions for Initiating Start
	
	while (!(TWCR & (1<<TWINT)));               // Bit Monitoring
	
	while((TWSR & 0xF8) != 0x50);
	
	return (TWDR);
}

void i2c_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	// End of Stop function
}



uint8_t i2c_ack_recieved(void)
{
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!(TWCR&(1<<TWINT)))
	
	while ((TWSR & 0b11111000) != 0x50);
}


uint8_t i2c_no_ack(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	
	while ((TWSR & 0b11111000) != 0x58);
	return (TWDR);
	
}
void set_rtc(uint8_t ss,uint8_t mm,uint8_t hh)
{
	i2c_start();
	i2c__MasterTransmission();  
	data_transmission(0x00);
	data_transmission(ss);
	data_transmission(mm);
	data_transmission(hh);
	i2c_stop();
}

void rtc_time_data(void)
{
	i2c_start();
	i2c__MasterTransmission();          // should data inside Master Transmission be Slave Address ?? :- Ans : Return Type void
	data_transmission(0x00);
	i2c_stop();
	// Start bit should be send once more ?? :- Ans : Yes 
	i2c_start();
	i2c_MasterRecieving();
	ss=data_reception();                   // ss not defined for this block
	mm=data_reception();                   // mm not defined for this block
	hh=i2c_no_ack();                   // hh not defined for this block
	i2c_stop();
}

int main(void)
{
	DDRA = 0xFF;
	DDRB = 0xFF;
	PORTA = 0xF0;
	set_rtc(0x55,0x59,0x00);
	while(1)
	{
		rtc_time_data();
		PORTA &=~(1<<PA0);
		PORTB=ss;
		_delay_us(2000);
		PORTA|=1<<PA0;
		_delay_us(2000);
		PORTA &=~(1<<PA1);
		PORTB=ss>>4;
		_delay_us(2000);
		PORTA|=1<<PA1;
		_delay_us(2000);
		PORTA &=~(1<<PA2);
		PORTB=mm;
		_delay_us(2000);
		PORTA|=1<<PA2;
		_delay_us(2000);
		PORTA &=~(1<<PA3);
		PORTB= mm>>4;
		_delay_us(2000);
		PORTA|=1<<PA3;
		_delay_us(2000);/////
		PORTA &=~(1<<PA4);
		PORTB=hh;
		_delay_us(2000);
		PORTA|=1<<PA4;
		_delay_us(2000);
		PORTA &=~(1<<PA5);
		PORTB=hh>>4;
		_delay_us(2000);
		PORTA|=1<<PA5;
		_delay_us(2000);
	}
	
	return 0;
}