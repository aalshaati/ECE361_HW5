#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "float_rndm.h"
#include "iom361_r2.h"

// constants
//#define _DEBUG_ 1

// global variables
static uint32_t IOSpace[sizeof(ioreg_t) / sizeof(uint32_t)];
static uint32_t* IOSpacePtr;
static int nsw;                        // number of switches
static int nleds;                      // number of LEDs
static bool isInitialized = false;     // true if IOM361 has been initialized
static uint32_t errValue = 0xDEADBEEF; // value returned on error

// Helper function prototypes
static void display_leds(uint32_t value, int num_leds);
static void display_rgb_leds(uint32_t value);

// API functions

/* iom361_initialize() */
uint32_t* iom361_initialize(int num_switches, int num_leds, int* rtn_code) {
    // initialize global variables
    nsw = num_switches;
    nleds = num_leds;
    IOSpacePtr = (uint32_t*)&IOSpace;

    #ifdef _DEBUG_
        printf("INFO [iom361_initialize()]: IOSpacePtr=%p, IOSpace Length=%d\n",
            IOSpacePtr, sizeof(IOSpace));
    #endif

    // initialize the I/O registers
    iom361_writeReg(IOSpacePtr, LEDS_REG, 0x00000000, NULL);
    iom361_writeReg(IOSpacePtr, RGB_LED_REG, 0x00000000, NULL);
    iom361_writeReg(IOSpacePtr, RSVD1_REG, 0x11111111, NULL);
    iom361_writeReg(IOSpacePtr, RSVD2_REG, 0x22222222, NULL);
    iom361_writeReg(IOSpacePtr, RSVD3_REG, 0x33333333, NULL);

    _iom361_setSwitches(0x00000000);
    _iom361_setSensor1(23.5, 75.0);

    // randomize rand()
    srand(time(NULL));

    if (rtn_code != NULL)
        *rtn_code = 0;
    isInitialized = true;
    return IOSpacePtr;
}

/* iom361_readReg() */
uint32_t iom361_readReg(uint32_t* base, uint32_t offset, int* rtn_code) {
    uint32_t value;
    uint32_t* ioreg_ptr;

    if (base != IOSpacePtr) {
        // not pointing to base of IO space
        if (rtn_code != NULL)
            *rtn_code = 1;
        return errValue;
    }

    if ((offset < 0) || (offset > (sizeof(IOSpace) - sizeof(uint32_t)))) {
        // offset is out of range
        if (rtn_code != NULL)
            *rtn_code = 2;
        return errValue;
    }

    // calculate address and get the value
    ioreg_ptr = base + (offset / sizeof(uint32_t));
    value = *ioreg_ptr;

    #ifdef _DEBUG_
        printf("INFO[iom361_readReg()]: base = %p, offset = %d, ioreg_ptr=%p, value=%08X\n",
            base, offset, ioreg_ptr, value);
    #endif

    if (rtn_code != NULL)
        *rtn_code = 0;
    return value;
}

/* iom361_writeReg() */
uint32_t iom361_writeReg(uint32_t* base, int offset, uint32_t value, int* rtn_code) {
    uint32_t* ioreg_ptr;

    if (base != IOSpacePtr) {
        // not pointing to base of IO space
        if (rtn_code != NULL)
            *rtn_code = 1;
        return errValue;
    }

    if ((offset < 0) || (offset > (sizeof(IOSpace) - sizeof(uint32_t)))) {
        // offset is out of range
        if (rtn_code != NULL)
            *rtn_code = 2;
        return errValue;
    }

    if ((offset % sizeof(uint32_t)) != 0) {
        // offset does not point to start of an I/O register
        if (rtn_code != NULL)
            *rtn_code = 3;
        return errValue;
    }

    // OK, we're in range and on a I/O register boundary
    if (rtn_code != NULL)
        *rtn_code = 0;

    ioreg_ptr = base + (offset / sizeof(uint32_t));

    #ifdef _DEBUG_
        printf("INFO[iom361_writeReg()]: base = %p, offset = %d, ioreg_ptr=%p, value=%08X\n",
            base, offset, ioreg_ptr, value);
    #endif

    switch (offset) {
        case SWITCHES_REG:
            break; // switches are a read-only input

        case LEDS_REG:
            *ioreg_ptr = value;
            display_leds(value, nleds);
            break;

        case RGB_LED_REG:
            *ioreg_ptr = value;
            display_rgb_leds(value);
            break;

        case TEMP_REG:
        case HUMID_REG:
            break; // temperature/humidity are read-only inputs

        case RSVD1_REG:
        case RSVD2_REG:
        case RSVD3_REG:
            *ioreg_ptr = value;
            break;

        default:
            if (rtn_code != NULL) // shouldn't get here
                *rtn_code = 4;
            value = errValue;
            break;
    }
    return value;
}

// Functions used for testing - set register values for read-only registers

/* _iom361_setSwitches() */
void _iom361_setSwitches(uint32_t value) {
    uint32_t* ioreg_ptr = IOSpacePtr + (SWITCHES_REG / sizeof(uint32_t));
    *ioreg_ptr = value;
    return;
}

/* _iom361_setSensor1() */
void _iom361_setSensor1(float new_temp, float new_humid) {
    float temp_float, humid_float;
    uint32_t temp_value, humid_value;

    uint32_t* ioreg_ptr;

    // Precomputed constants
    static const float temp_const = 1048576.0 / 200.0;
    static const float rh_const = 1048576.0 / 100.0;

    // per AHT20 data sheet, Temp(C) = (ST/2**20)* 200 - 50
    temp_float = temp_const * (new_temp + 50.0);
    temp_value = (uint32_t)temp_float;

    // per AHT20 data sheet, RH(%) = (SRH/2**20)* 100%
    humid_float = rh_const * new_humid;
    humid_value = (uint32_t)humid_float;

    // write the I/O registers
    ioreg_ptr = IOSpacePtr + (TEMP_REG / sizeof(uint32_t));
    *ioreg_ptr = temp_value;

    ioreg_ptr = IOSpacePtr + (HUMID_REG / sizeof(uint32_t));
    *ioreg_ptr = humid_value;
    return;
}

/* _iom361_setSensor1_rndm() */
void _iom361_setSensor1_rndm(float temp_low, float temp_hi,
                             float humid_low, float humid_hi) {
    float new_temp = float_rand_in_range(temp_low, temp_hi);
    float new_humid = float_rand_in_range(humid_low, humid_hi);
    _iom361_setSensor1(new_temp, new_humid);
}

// Helper Functions

static void display_leds(uint32_t value, int num_leds) {
    char leds[32];
    for (int i = 0; i < num_leds; i++) {
        leds[i] = (0x1 << i) & value ? 'o' : '_';
    }
    for (int i = num_leds - 1; i >= 0; i--) {
        if ((num_leds - 1 - i) % 4 == 0) {
            putchar(' ');
            putchar(' ');
        }
        putchar(leds[i]);
    }
    printf("\n");
    return;
}

static void display_rgb_leds(uint32_t value) {
    uint8_t red_dc = (value >> 16) & 0xFF;
    uint8_t green_dc = (value >> 8) & 0xFF;
    uint8_t blue_dc = (value >> 0) & 0xFF;
    uint8_t enable = (value >> 31) & 0x01;

    printf("RedDC=%2d%% (%3d), GrnDC=%2d%% (%3d), BluDC=%2d%% (%3d)\tEnable=%s\n",
           (red_dc * 100 / 255), red_dc,
           (green_dc * 100 / 255), green_dc,
           (blue_dc * 100 / 255), blue_dc,
           (enable ? "ON" : "OFF"));
}
