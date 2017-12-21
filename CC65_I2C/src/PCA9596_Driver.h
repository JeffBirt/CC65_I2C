/* PCA9596_Driver.h  */
// ****************************************************************************
// Driver for PCA9596 I2C IO POrt chip
// Jeffrey T. Birt 11/26/2017
//
// The idea is that a PCA9596 'object' is created which is a pointer to 
// a structure of configuration for one ship. This pointer can be passed
// to driver functions to indicate which chip you want to talk to.
//
// Usage -	PCA9596 IOBoard = {	.address   = 0x00,
//					.configIO  = {0x00, 0xFF, 0x00, 0xFF, 0x00},
//					.polarity  = {0x00, 0x00, 0x00, 0x00, 0x00},
//					.interrupt = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
//	
//					PCA9596_init(i2c);			// pass pointer to I2C bus driver to use
//					PCA9596_configAll(&IOBoard);    // configure this PCA9596 schip
//					
//	You first initialize a PCA9596 'object' describing how your chip should 
//	configured. Then you call PCA9596_init(*I2C) and pass the pointer to the
//	I2C bus driver to use. Then you call PCA9596_configAll(&device) and pass
//	the address the PCA9596 'object'. 
//
//	How to set ports/pins
//
//  How to read inputs
// ****************************************************************************

#ifndef PCA9596_DRIVER
#define PCA9596_DRIVER

#define AddBase 0x40
#define AutoInc 0x80
#define NoAutoInc 0x00
#define CfgPol 0x10
#define CfgIO 0x18
#define CfgInt 0x20
#define RD 0x01
#define WR 0x00
#define Input 0x00
#define Output 0x08
#define MaxPorts 5

#include "I2C_Driver.h"

enum Port
{
	port0,
	port1,
	port2,
	port3,
	port4
};

typedef struct PCA9596 PCA9596;
struct PCA9596
{
	unsigned char address;
	unsigned char configIO[5];
	unsigned char configPol[5];
	unsigned char configInt[5];
	unsigned char outPort[5];
	unsigned char inPort[5];
};

I2C *_i2c; // pointer to I2C driver to use

// external functions
void PCA9596_init(I2C *i2c); // initial driver
unsigned char PCA9596_configAll(PCA9596 *device); // configure all ports w/current settings
unsigned char PCA9596_writePorts(PCA9596 *device);
unsigned char PCA9596_readPorts(PCA9596 *device);

#endif /* !PCA9596_DRIVER */