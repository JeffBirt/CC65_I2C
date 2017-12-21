/* main.c  */
// ****************************************************************************
// Test routines for I2C driver and PCA9596 driver
//
// Create a test suite with a menu system
//	
// ****************************************************************************

#include <stdio.h>
#include "conio.h"
#include "main.h"
#include "i2c_driver.h"
#include "PCA9596_Driver.h"

// create a PCA9596 'object' to hold config data for chip
PCA9596 IOBoard_0;
PCA9596 IOBoard_1;

void main(void)
{
	unsigned int s = 0;
	int i = 0;
	int key = 0;
	char OK = 1;

	I2C *i2c = createI2C(); // create pointer to I2C 'object'
	i2c->init();			// initialize I2C driver

	IOBoard_0.address = 0x00;
	IOBoard_0.configIO[0] = 0x00; IOBoard_0.configIO[1] = 0xFF;
	IOBoard_0.configIO[2] = 0x00; IOBoard_0.configIO[3] = 0xFF;
	IOBoard_0.configIO[4] = 0x00;
	IOBoard_0.configPol[0] = 0x00; IOBoard_0.configPol[1] = 0x00;
	IOBoard_0.configPol[2] = 0x00; IOBoard_0.configPol[3] = 0x00;
	IOBoard_0.configPol[4] = 0x00;
	IOBoard_0.configInt[0] = 0xFF; IOBoard_0.configInt[1] = 0xFF;
	IOBoard_0.configInt[2] = 0xFF; IOBoard_0.configInt[2] = 0xFF;
	IOBoard_0.configInt[4] = 0xFF;

	IOBoard_1.address = 0x01;
	IOBoard_1.configIO[0] = 0xFF; IOBoard_1.configIO[1] = 0x00;
	IOBoard_1.configIO[2] = 0xFF; IOBoard_1.configIO[3] = 0x00;
	IOBoard_1.configIO[4] = 0xFF;
	IOBoard_1.configPol[0] = 0x00; IOBoard_1.configPol[1] = 0x00;
	IOBoard_1.configPol[2] = 0x00; IOBoard_1.configPol[3] = 0x00;
	IOBoard_1.configPol[4] = 0x00;
	IOBoard_1.configInt[0] = 0xFF; IOBoard_1.configInt[1] = 0xFF;
	IOBoard_1.configInt[2] = 0xFF; IOBoard_1.configInt[2] = 0xFF;
	IOBoard_1.configInt[4] = 0xFF;

	//// pass pointer to I2C bus driver to use
	PCA9596_init(i2c);

	mainMenu();

	/* Check for a key */
	while (1)
	{
		if (kbhit())
		{
			key = cgetc();
			if (key == 'a') //configure
			{
				configBoards();
				mainMenu();
			}
			else if (key == 'b') //output test
			{
				outputTest();
				mainMenu();
			}
			else if (key == 'c') //input test
			{
				inputTest();
				mainMenu();
			}
			else if (key == 'd') //nothing
			{
				cprintf("Size Of short = %d\r\n", sizeof(short));
			}
		}
	}

}

void mainMenu(void)
{
	clrscr();
	cprintf("6502-C Compiler.\r\n");

	cprintf("A - Config Boards 0,1\r\n");
	cprintf("B - Ouput Test\r\n");
	cprintf("C - Input Test\r\n");
	cprintf("D - nothing\r\n");
}

unsigned char configBoards(void)
{
	unsigned char OK = 1;
	int key;

	OK = PCA9596_configAll(&IOBoard_0); // configure this PCA9596 chip
	cprintf("Config Board 0=%d\r\n", OK);
	OK = PCA9596_configAll(&IOBoard_1); // configure this PCA9596 chip
	cprintf("Config Board 1=%s\r\n", (OK>0) ? "Error" : "OK");
	cprintf("Press any key to continue.\r\n");

	while (!kbhit())
	{
		// nothing
	}
	cgetc();

	return OK;
}

void inputTest(void)
{
	int key = 0;
	char OK = 1;

	clrscr();
	cprintf("Any key to read Inputs\r\n");
	cprintf("D - when done\r\n");
	while (key != 'd')
	{
		if (kbhit())
		{
			key = cgetc();
			//OK = PCA9596_readPorts(&IOBoard_0);
			//OK |= PCA9596_readPorts(&IOBoard_1);

			cprintf("B0, P0: %d\r\n", IOBoard_0.inPort[1]);
			cprintf("B1, P1: %d\r\n", IOBoard_1.inPort[0]);
		}
	}
}

void outputTest(void)
{
	int key = 0;
	char OK = 1;

	IOBoard_0.outPort[0] = 0xAA;
	IOBoard_1.outPort[1] = 0xAA;

	clrscr();
	cprintf("A - Toggle Board 0, Port 0\r\n");
	cprintf("B - Toggle Board 1, Port 1\r\n");
	cprintf("D - when done\r\n");

	while (key != 'd')
	{
		if (kbhit())
		{
			key = cgetc();
			if (key == 'a')
			{
				IOBoard_0.outPort[0] ^= 0xFF; // toggle all bits
				//OK = PCA9596_writePorts(&IOBoard_0);
				cprintf("IOBoard_0.outPort[0]=%d\r\n", IOBoard_0.outPort[0]);

			}
			else if (key == 'b')
			{
				IOBoard_1.outPort[1] ^= 0xFF; // toggle all bits
				//OK = PCA9596_writePorts(&IOBoard_1);
				cprintf("IOBoard_1.outPort[1]=%d\r\n", IOBoard_1.outPort[1]);
			}
		}
	}
	
}