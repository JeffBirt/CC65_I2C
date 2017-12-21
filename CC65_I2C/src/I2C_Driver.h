/* i2c_driver.h  */
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

#ifndef I2C_DRIVER
#define I2C_DRIVER

// NOTE: CC65 does not use bools, use char instead val > 0 == true
typedef struct I2C I2C;
struct I2C
{
	void (*init)(void);
	void (*portReset)(void);
	unsigned char(*readBytes)(char send_stop, unsigned char *rxArray,
											unsigned int length);
	unsigned char(*writeBytes)(char send_start, char send_stop,
											unsigned char *txArray, unsigned int length);
};

// external functions
I2C *createI2C(void);
void init(void);      		// Set calculator link port to I2C settings
void linkReset(void);    	// Reset calculator link port to default settings
void portReset(void);
unsigned char readBytes(unsigned char send_stop, unsigned char *rxArray,
										unsigned int length);
unsigned char writeBytes(unsigned char send_start, unsigned char send_stop,
										unsigned char *txArray, unsigned int length);

//internal functions
void arbitration_lost(void);
void start(void);			// Send Start condition
void stop(void);			// Send Stop condition
void write_bit(unsigned char bit);
unsigned char read_bit(void);
void delay(void); 		// Busy loop to add delay
unsigned char read_SCL(void);  // Return current level of SCL line, 0 or 1
unsigned char read_SDA(void);  // Return current level of SDA line, 0 or 1
void set_SCL(void);   // Do not drive SCL (set pin high-impedance) 
void clear_SCL(void); // Actively drive SCL signal low
void set_SDA(void);   // Do not drive SDA (set pin high-impedance)
void clear_SDA(void); // Actively drive SDA signal low


#endif /* !I2C_DRIVER */
