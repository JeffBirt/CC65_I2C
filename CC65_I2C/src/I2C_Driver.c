/* i2c_driver.c  */
// ****************************************************************************
// Based on C pseudo code from: https://en.wikipedia.org/wiki/I²C
// and published source by Dave Watson --- djwatson@u.washington.edu
// Jeffrey T. Birt 11/26/2017
//
// The idea is that an I2C 'object' is created which is a pointer to 
// a structure of the I2C external functions. This pointer can be passed
// to device drivers that need to talk in the I2C bus.
//
// Usage -	I2C *i2c = createI2C(); // create pointer to I2C 'object'
// 					i2c->init();						// initialize I2C driver
//					myDriverSetI2C(i2c);		// pass the pointer to your device driver
//					
//	Your driver would then use the pointer to access the I2C bus. You place 
//	your data in a char array and pass a poiner to it and #bytes to send.
//	Optionally you can choose to preface your data with the START condition
//	and suffix it with the stop condition as required by the device you are 
//	talking to on the I2 bus.
//
//				_i2c->writeBytes(send_start, send stop, &address_of_array,length);
//
//  			_i2c->writeBytes(TRUE, FALSE, &address, 1);		
// ****************************************************************************


#include "i2c_driver.h"
#include "stdlib.h"

#define I2CSPEED 1 // 100 == 2kHz, 200 == 1kHz
#define USR_DDR_DEFAULT 0x00
#define USR_DDR_I2C 0x03
#define USR_PORT_DEFAULT 0x00
#define USR_PORT_I2C 0x03
#define LINK_D1_IN 0x08 //D1 = SCL
#define LINK_D1_OUT 0x02
#define LINK_D0_IN 0x04 //D0 = SDA
#define LINK_D0_OUT 0x01
//#define ARBITRATION
//#define ACTIVE_LOW // output port is active low hardware, else invert


unsigned char started = 0; // global data
// Link Port, D0 == SDA, D1 == SCL
static char *const usr_DDR=(char *)0xDD03;
//static char *const link_status = (char *)0x6000D;
static char *const usr_port =(char *)0xDD01;
unsigned char bit, index, byte;
unsigned char nack = 0;

// ****************************************************************************
// External functions
// ****************************************************************************

// Creates an I2C 'object', a pointer that main can create and pass to 
// devices that need access to the I2C bus.
I2C *createI2C(void)
{
	I2C *myI2C = malloc (sizeof (I2C));
	
	if (myI2C != 0)
	{
		myI2C -> init = &init;
		myI2C -> portReset = &portReset;
		myI2C -> readBytes = &readBytes;
		myI2C -> writeBytes = &writeBytes;
	}
	
	return myI2C;
}

// Set calculator link port to I2C settings
// disable automatic connection detection and interrupts
void init(void)
{
	portReset();
	*usr_DDR = USR_DDR_I2C;
	*usr_port = USR_PORT_I2C;
}

// Reset calculator link port to default settings
// enable automatic connection detection and interrupts
void portReset(void)
{
	*usr_DDR = USR_DDR_DEFAULT;
	*usr_port = USR_PORT_DEFAULT;
}

// Read a bytes from I2C bus
unsigned char readBytes(unsigned char send_stop, unsigned char *rxArray,
								unsigned int length) 
{
  unsigned char index, bit, byte = 0;
  unsigned char nack = 0;

	for (index = 0; index < length; index++)
	{
	  for (bit = 0; bit < 8; ++bit) 
	  {
	    byte = (byte << 1) | read_bit(); 
	  }
		
		rxArray[index] = byte;
		// do not send nack after last byte
	  if (index < length) { write_bit(nack); }
  }

  if (send_stop) 
  {
    stop(); 
  }

  return byte;
}

// Write a byte to I2C bus. Return 0 if ack by the slave.
unsigned char writeBytes(unsigned char send_start, unsigned char send_stop,
									unsigned char *txArray, unsigned int length) 
{ 
  //unsigned char bit, index, byte;
  //unsigned char nack = 0;
	nack = 0;

  if (send_start) 
  {
    start();
  }
  
	for (index = 0; index < length; index++)
	{
		byte = txArray[index];
		//for (bit = 0; bit < 8; ++bit) 
		for (bit = 0x80; bit >= 1; bit>>1)
		{
			//write_bit((byte & 0x80) != 0);
			write_bit((byte & bit) != 0);
			//byte <<= 1;
		}
		nack |= read_bit();	// if any NACK comes back TRUE stays TRUE
  }

  if (send_stop) 
  {
    stop();
  } 
 
  return nack; 
}


// ****************************************************************************
// Internal functions
// ****************************************************************************

void arbitration_lost(void)
{
	#ifdef ARBITRATION
	// do something to signal state change
	int i;
	int x=0;
	for (i=0; i<100; i++)
	{
		x++;
	}
	#endif
}

// Set SDA line HIGH, allow to be pulled up by internal resistor
void set_SDA(void)
{
#ifdef ACTIVE_LOW
	*usr_port &= ~LINK_D0_OUT;
#else
	*usr_port |= LINK_D0_OUT;
#endif
}

// Drive SDA line LOW, activly pull down
void clear_SDA(void)
{
#ifdef ACTIVE_LOW
	*usr_port |= LINK_D0_OUT;	
#else
	*usr_port &= ~LINK_D0_OUT;
#endif
}

// Set SCL line HIGH, allow to be pulled up by internal resistor
void set_SCL(void)
{
#ifdef ACTIVE_LOW
	*usr_port &= ~LINK_D1_OUT;	
#else
	*usr_port |= LINK_D1_OUT;
#endif
}

// Drive SCL line LOW, activly pull down
void clear_SCL(void)
{
#ifdef ACTIVE_LOW
	*usr_port |= LINK_D1_OUT;
#else
	*usr_port &= ~LINK_D1_OUT;
#endif
}

// Send start condition
void start(void) 
{
	int cs = 0;
	
	// if started, do a restart condition
  if (started) 
  {  
    set_SDA(); 
    //delay(); 
    set_SCL();
    
    // Clock stretching
    //while (read_SCL() == 0 && cs < 5) 
    //{  
    //  cs++; 
    //} 
 
    // Repeated start setup time, minimum 4.7us
    //delay();
  }

#ifdef ARBITRATION
  if (read_SDA() == 0) 
  {
    arbitration_lost();
  }
#endif
 
  // SCL is high, set SDA from 1 to 0. 
  clear_SDA(); 
  //delay(); 
  clear_SCL(); 
  started = 1; 
}

void stop(void) 
{
	int cs = 0;
  clear_SDA();
  //delay();

  set_SCL();
  // Clock stretching
  //while (read_SCL() == 0 && cs < 5) 
  //{
  //  cs++;
  //} 

  // Stop bit setup time, minimum 4us 
  //delay(); 
 
  // SCL is high, set SDA from 0 to 1 
  set_SDA();
  //delay(); 

#ifdef ARBITRATION
  if (read_SDA() == 0) 
  {
    arbitration_lost(); 
  } 
#endif
 
  started = 1;
} 

// Read state of SDA line
unsigned char read_SDA(void)
{
	return !((*usr_port&LINK_D0_IN)>>2);
}

// Read state of SCL line
unsigned char read_SCL(void)
{
	return !((*usr_port&LINK_D1_IN)>>3);
}
 
// Write a bit to I2C bus
void write_bit(unsigned char bit)
{ 
  //int cs = 0;
  
  if (bit) 
  { 
    set_SDA(); 
  } else 
  { 
    clear_SDA(); 
  } 
 
  // SDA change propagation delay 
  //delay(); 
 
  // Set SCL high to indicate a new valid SDA value is available
  set_SCL(); 

  // Wait for SDA value to be read by slave, minimum of 4us for standard mode 
  //delay(); 

	// Clock stretching
  //while (read_SCL() == 0  && cs < 5) 
  //{
  //  cs++; 
  //}
 
  // SCL is high, now data is valid
  // If SDA is high, check that nobody else is driving SDA
#ifdef ARBITRATION
  if (bit && (read_SDA() == 0)) 
  {
    arbitration_lost();
  }
#endif

  // Clear the SCL to low in preparation for next change
  clear_SCL(); 
}

// Read a bit from I2C bus
unsigned char read_bit(void)
{
	unsigned char bit;
	//int cs = 0;
 
	// Let the slave drive data 
	set_SDA(); 
 
	// Wait for SDA value to be written by slave, minimum of 4us for standard mode 
	//delay(); 

	// Set SCL high to indicate a new valid SDA value is available 
	set_SCL(); 
 
	// Clock stretching
	//while (read_SCL() == 0 && cs < 5) 
	//{ 
	//	cs++;
	//}
 
	// Wait for SDA value to be written by slave, minimum of 4us for standard mode
	//delay();
 
	// SCL is high, read out bit 
	bit = read_SDA();
 
	// Set SCL low in preparation for next operation 
	clear_SCL(); 

	return bit; 
} 

// Do nothing loop to act as SCK delay between bits
//void delay(void) 
//{ 
//  volatile int v;
//  int i;
// 
//  for (i = 0; i < I2CSPEED; ++i) 
//  {
//    v;
//  } 
//}
