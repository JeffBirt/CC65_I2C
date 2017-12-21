/* PCA9596_Driver.c  */

#include "PCA9596_Driver.h"

// call first to initalize driver
void PCA9596_init(I2C *i2c)
{
	_i2c = i2c;
}

//bool (*writeByte)(bool send_start, bool send_stop, unsigned char byte);
unsigned char PCA9596_configAll(PCA9596 *target)
{
	unsigned char isOK = 0;
	unsigned char address = AddBase | (target->address<<1) | WR;
	unsigned char reg = (AutoInc | CfgIO);
	isOK |= _i2c->writeBytes(1, 0, &address, 1);
	isOK |= _i2c->writeBytes(0, 0, &reg, 1);
	isOK |= _i2c->writeBytes(0, 1, target->configIO, 5);

	reg = (AutoInc | CfgPol); 	//configure polarity
	isOK |= _i2c->writeBytes(1, 0, &address, 1);
	isOK |= _i2c->writeBytes(0, 0, &reg, 1);		
	isOK |= _i2c->writeBytes(0, 1, target->configPol, 5);

	reg = (AutoInc | CfgInt);	
	isOK |= _i2c->writeBytes(1, 0, &address, 1);//configure interrupts
	isOK |= _i2c->writeBytes(0, 0, &reg, 1);
	isOK |= _i2c->writeBytes(0, 1, target->configInt, 5);
		
	return isOK;
}

// Write data to a output port
unsigned char PCA9596_writePorts(PCA9596 *target)
{
	unsigned char isOK = 0;
	unsigned char address = AddBase | (target->address<<1) | WR;
	unsigned char reg = (AutoInc | Output | port0);
	isOK |= _i2c->writeBytes(1, 0, &address, 1);
	isOK |= _i2c->writeBytes(0, 0, &reg, 1);
	isOK |= _i2c->writeBytes(0, 1, target->outPort, 5);
	
	return isOK;
}

unsigned char PCA9596_readPorts(PCA9596 *target)
{
	unsigned char isOK = 0;
	unsigned char address = AddBase | (target->address<<1) | WR;
	unsigned char reg = (AutoInc | Input | port0);
	isOK |= _i2c->writeBytes(1, 0, &address, 1);
	isOK |= _i2c->writeBytes(0, 0, &reg, 1);
	address = AddBase | (target->address<<1) | RD;
	isOK |= _i2c->writeBytes(1, 0, &address, 1);
	isOK |= _i2c->readBytes(1, target->inPort, 5);

	return isOK;
}
