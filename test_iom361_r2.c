/**
 * test_iom361_r2 - verifies the functionality of the ECE 361 I/O emulator
 *
 * @file:		test_iom361_r2.c
 * @author:		Roy Kravitz (roy.kravitz@pdx.edu)
 * @date:		05-Nov-2033
 * @version:	2.0
 *
 * Test program for the ECE 361 I/O emulator.  Fairly basic:
 * <pre>
 *	- initializes the I/O emulator
 *	- reads all of the registers and display initial values
 *  - changes switches and writes them to LEDs
 *  - changes temp and humidity and displays new values
 * </pre>
 *
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

#include "iom361_r2.h"

// typedefs, enums and constants
#define TEMP_RANGE_LOW	42.0
#define TEMP_RANGE_HI	52.0
#define HUMID_RANGE_LOW	72.6
#define HUMID_RANGE_HI	87.3
  
 // global variables
 uint32_t* io_base;

 // main()
 int main() {
	int rtn_code;
	uint32_t reg_value;
	float temp, humid;
	 
	// greet the user and display the working directory for the application
    printf("ECE 361 - Test ECE 361 IO Module(roy.kravitz@pdx.edu)\n");
    errno = 0;
    char *buf = getcwd(NULL, 0);    // allocates a buffer large enough to hold the path
    
    if (buf == NULL) {
        perror("getcwd");
        printf("Could not display the path\n");
    }
    else {
        printf("Current working directory: %s\n", buf);
        free(buf);
    }
    printf("\n");
	 
	// initialize IO module
	printf("Test 1: Initialize and read the I/O registers\n");
	io_base = iom361_initialize(16, 16, &rtn_code);
	if (rtn_code != 0) {
		// initialization failed
		printf("FATAL(main): Could not initialize I/O module\n");
		return 1;
	}
	printf("\n");
	 
	// read/display all of the register values
	for (int i = 0; i < NUM_IO_REGS; i++) {
		// The I/O registers are all 32-bits wide, so 4 bytes apart
		reg_value = iom361_readReg(io_base, (i * 4), &rtn_code);
		if (rtn_code == 0) {
			printf("Reading I/O register %d, value: %08X\n",
				i, reg_value);
		}
		else {
			 printf("ERROR(main): Failed reading I/O register %d\n", i);
		}
	}
	 
	// put some values in switches and copy them to LEDs
	printf("\nTest 2: Test the switches and leds\n");
	
	reg_value = 0xA5A5;
	printf("\tsetting switches to %8X\n", reg_value); 
	_iom361_setSwitches(reg_value);
	reg_value = iom361_readReg(io_base, SWITCHES_REG, NULL);
	iom361_writeReg(io_base, LEDS_REG, reg_value, NULL);
 
	reg_value = 0x5A5A;
	printf("\tsetting switches to %8X\n", reg_value);
	_iom361_setSwitches(reg_value); 
	reg_value = iom361_readReg(io_base, SWITCHES_REG, NULL);
	iom361_writeReg(io_base, LEDS_REG, reg_value, NULL);

	reg_value = 0xFFFF;
	printf("\tsetting switches to %8X\n", reg_value); 	 
	_iom361_setSwitches(reg_value);
	reg_value = iom361_readReg(io_base, SWITCHES_REG, NULL);
	iom361_writeReg(io_base, LEDS_REG, reg_value, NULL);
	
	reg_value = 0x0000;
	printf("\tsetting switches to %8X\n", reg_value); 
	_iom361_setSwitches(reg_value);
	reg_value = iom361_readReg(io_base, SWITCHES_REG, NULL);
	iom361_writeReg(io_base, LEDS_REG, reg_value, NULL);

	// set the duty cycles of the RGB LED
	printf("\nTest 3: Test the RGB LED\n");	 
	reg_value = 0x80ABBAFC;  //red dc=67, green dc=73, blue dc= 99
	iom361_writeReg(io_base, RGB_LED_REG, reg_value, &rtn_code);
	if (rtn_code != 0) {
		printf("ERROR(main): Could not write RGB LED register\n");
	}
	reg_value = 0x00380378;	// disable RGB LED output, red dc=23, green dc=01, blue dc=47
	iom361_writeReg(io_base, RGB_LED_REG, reg_value, NULL);	
	
	// set a new temperature and humidity and display it
	uint32_t temp_value, humid_value;
	
	printf("\nTest 4: Test the sensor\n");	 
	_iom361_setSensor1(10.0, 63.0);
	temp_value = iom361_readReg(io_base, TEMP_REG, NULL);
	temp = (temp_value / powf(2,20)) * 200.0 - 50;
	humid_value = iom361_readReg(io_base, HUMID_REG, NULL);
	humid = (humid_value/ pow(2, 20)) * 100;
	printf("Temperature: %3.1fC(%08X), Relative Humidity: %3.1f%%(%08X)\n", 
		temp, temp_value, humid, humid_value);
	
	printf("\nTest 5: Test the random sensor value function\n");	 
	_iom361_setSensor1_rndm(TEMP_RANGE_LOW, TEMP_RANGE_HI, HUMID_RANGE_LOW,
		HUMID_RANGE_HI);
	temp_value = iom361_readReg(io_base, TEMP_REG, NULL);
	temp = (temp_value / powf(2,20)) * 200.0 - 50;
	humid_value = iom361_readReg(io_base, HUMID_REG, NULL);
	humid = (humid_value/ pow(2, 20)) * 100;
	printf("Temperature: %3.1fC(%08X), Relative Humidity: %3.1f%%(%08X)\n", 
		temp, temp_value, humid, humid_value);
		
	printf("\nTest 5: Try a 2nd set of random numbers\n");	 
	_iom361_setSensor1_rndm(TEMP_RANGE_LOW, TEMP_RANGE_HI, HUMID_RANGE_LOW,
		HUMID_RANGE_HI);
	temp_value = iom361_readReg(io_base, TEMP_REG, NULL);
	temp = (temp_value / powf(2,20)) * 200.0 - 50;
	humid_value = iom361_readReg(io_base, HUMID_REG, NULL);
	humid = (humid_value/ pow(2, 20)) * 100;
	printf("Temperature: %3.1fC(%08X), Relative Humidity: %3.1f%%(%08X)\n", 
		temp, temp_value, humid, humid_value);
	 
	return 0;
 }
	 
	 
	 
		
	 