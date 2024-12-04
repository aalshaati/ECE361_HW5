/**
 * iom361_r2.h - Header file for ECE 361 I/O module emulator
 *
 * @file:		iom361_r2.h
 * @author:		Roy Kravitz (roy.kravitz@pdx.edu)
 * @date:		05-Nov-2023
 * @version:	2.0
 *
 * This is the header file for the ECE 361 I/O module emulation.  The I/O module
 * emulates a memory-mapped I/O system with a number of "typical" peripheral registers.
 *
 */
 
 /**
 * Register formats:
 * -----------------
 *
 *	o switches[31:0]:	One bit per switch starting w/ bit[0] (rightmost, LSB).  Number
 *						of switches is specified in iom361_initialize().  Max of 32 switches.
 *						A switch is on for every bit that is 1
 *
 *	o leds[31:0]:		One bit per LED starting with bit[0] (rightmost, LSB. Number
 *						of LEDS is specified in iom361_initialize(). Max of 32 LEDS.
 *						An LED is on (lit) for every bit that is 1.  Contents of LED
 *						register is displayed on every write to the register.  Format is
 *						'o' for every lit LED.  '_' for every dark LED.
 *
 * o rgb_led[31:0]:		Control register for RGB LED.  Formatted as follows:
 * <pre>
 *	- bits[31:31]:  Enable - true if RGB outputs are enabled
 *	- bits[30:24]:	*reserved*
 *	- bits[23:16]:	8-bit duty cycle for Red segment
 *	- bits[15:8]:	8-bit duty cycle for Green segment
 *	- bits[7:0]:	8-bit duty cycle for Blue segment
 * </pre>
 *
 * o temperature[31:0]:	Temperature in degrees C.  iom361 emulates an AHT0 
 *						temperature/humidity sensor.  Temperature is 24-bit
 *						number that can be converted to a float with the following formula:
 *							Temp(degrees C) = (ST/2**20) * 200 - 50
 *								where ST is the value in the register.
 *
 * o humidity[31:0]:	Relative humidity in %.  iom361 emulates an AHT0 temperature/humidity 
 *						sensor.  Humidity is 24-bit number that can be converted to a float
 * 						with the following formula:
 *							Rel Humidity(%) = (SRH/2**20) * 100
 *								where SRH is the value in the register.
 *
 * o reserved_1[31:0]:	Reserved for future use.  Can be written and read
 *
 * o reserved_2[31:0]:	Reserved for future use.  Can be written and read
 *
 * o reserved_3[31:0]:	Reserved for future use.  Can be written and read	
 */
 
 #ifndef _IOM361_H
 #define _IOM361_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
 // define the I/O register map
 typedef struct {
	 uint32_t	switches;
	 uint32_t	leds;
	 uint32_t	rgbled;
	 uint32_t	temperature;
	 uint32_t	humidity;
	 uint32_t	reserved_1;
	 uint32_t	reserved_2;
	 uint32_t	reserved_3;
 } ioreg_t, *ioreg_ptr_t;
 
 // typedefs and enums
 enum {
	 SWITCHES_REG	= 0x00,
	 LEDS_REG		= 0x04,
	 RGB_LED_REG	= 0x08,
	 TEMP_REG		= 0x0C,
	 HUMID_REG		= 0x10,
	 RSVD1_REG		= 0x14,
	 RSVD2_REG		= 0x18,
	 RSVD3_REG		= 0x1C
 };
 
 // define constants
  #define NUM_IO_REGS	8		// There are 8 IO registers in the I/O map
 
 /*
  * API functions.  These are low level functions that read/write the
  * I/O registers directly.  You can use them to build higher level
  * functionality in your own code, but it doesn't get much more basic
  * than this.
  */
  
 /**
  * iom361_initialize() - initializes the ECE 361 I/O module
  *
  * Initializes the I/O module emulator. Function returns a pointer
  * to the base of the I/O register block.  Returns NULL if the function
  * fails. Updates rtn_code if the function succeeds (0) or fails (> 0)
  *
  * @param  num_switches: the number of switches (up to 32) in iom361
  * @param	num_leds: the number of leds (up to 32) in iom361
  * @param	*rtn_code: a pointer to the return code.  Will be 0 for success, a different
  *			number if the call fails. 
  *
  * @return	a pointer to the base of the I/O register block.  NULL if function fails
  */
uint32_t* iom361_initialize(int num_switches, int num_leds, int* rtn_code);
 
 
 /** iom361_readReg() - returns the value of an I/O register
  *
  * reads/returns the value of the I/O register at base + offset.  Updates
  * rtn_code if the function succeeds (0) or fails (> 0)
  *
  * @param	base: address of the base of the I/O memory block
  * @param	offset: offset into I/O memory block.  All registers are 32-bits wide
  * @param	*rtn_code: a pointer to the return code.  Will be 0 for success, a different
  *			number if the call fails. 
  *
  * @return the contents of the specified I/O register
  */
uint32_t iom361_readReg(uint32_t* base, uint32_t offset, int* rtn_code);
 
 
 /**
  * iom361_writeReg() - writes a 32-bit value to an I/O register
  *
  * writes a new value into the I/O register at base + offset. Updates
  * rtn_code if the function succeeds (0) or fails (> 0)
  *
  * @param	base: address of the base of the I/O memory block
  * @param	offset: offset into I/O memory block.  All registers are 32-bits wide 
  * @param	*rtn_code: a pointer to the return code.  Will be 0 for success, a different
  *			number if the call fails. 
  *
  * @return the contents of the specified I/O register (does a read)
  */
uint32_t iom361_writeReg(uint32_t* base, int offset, uint32_t value, int* rtn_code);


/* These functions are used for testing.  They set a specific register to a value.  For
 * example, there is a function to write a new value to the switch register.  The same
 * for the temp/humidity sensor.  I added these functions because we are emulating
 * memory-mapped I/O...there is no "real" hardware at the other end.
 *
 * The function names start w/ a _ to differentiate them from what would normally be
 * the API.
 */
 
 /**
  * _iom361_setSwitches () - sets the value of the switch register
  *
  * Used to set the value of the switch I/O register location.  The driver for the
  * emulator keeps track of the base address so it doesn't need to be a parameter
  *
  * @param	value: value for the switch register.  Not all 32-bits may be switches.  The
  * number of switches is set when iom361 is initialized.
  *
  */
void _iom361_setSwitches(uint32_t value);
 
 
 /**
  * _iom361_setSensor1 () - sets the temperature and humidity for Sensor 1
  *
  * Used to set the temperature and humidity for the emulated AHT20 sensor. The
  * sensor returns 20-bit unsigned values for the temperature and humidity. Fortunately
  * you can set temp to 0 - 100 degrees C and humidity to 0 - 99% RH as floats
  * and the function will calculate the value written to the register
  *
  * @param	new_temp: new temperature value in degrees C.  Specified as a float.
  *			conversion to register value is done in the function
  * @param	new_humid: new humidity value  Specified as float. conversion to a register
  *			value is done in the function
  */
void _iom361_setSensor1(float new_temp, float new_humid);

/**
  * _iom361_setSensor1_rndm () - sets the temperature and humidity for Sensor 1
  *
  * Used to set the temperature and humidity for the emulated AHT20 sensor. The
  * sensor returns 20-bit unsigned values for the temperature and humidity. This function is able
  * to set the temperature and humidity values to random numbers within the specified range
  *
  * @param	temp_low: low temperature for range  in degrees C.  Specified as a float.
  *			conversion to register value is done in the function
  * @param	temp_hi: high temperature for range  in degrees C.  Specified as a float.
  *			conversion to register value is done in the function
  * @param	humid_low: low relative humidity in range   Specified as float. 
  *			conversion to a register value is done in the function
  * @param	humid_hi: high relative humidity in range   Specified as float. 
  *			conversion to a register value is done in the function
  *
  * @note	Uses srand(time(NULL) to initialize rand().  This is done when
  *			iom361 is initialized.
  */
void _iom361_setSensor1_rndm(float temp_low, float temp_hi,
	float humid_low, float humid_hi);

#endif
  
  
  
  

  