/*
    sensors contains the actual sensor data and related functions
*/

#include <zephyr/types.h>

// Temp values can range from -20 C (0) to 300 C (32000). The last two digits are decimal places (ie 150.43 C = 15043)
// Signed ints are annoying so the range is offset to allow some negative temps (ie -20 C = 0, 0 C = 2000, 100 C = 12000).
extern uint16_t temp[12];

extern uint16_t pressure;

extern uint16_t battery;		    // battery voltage * 100, for example 340 = 3.40v

extern uint16_t temp_freq;	        // frequency of temp sensor updates in Hertz (1000 / frequency) should be a whole number

extern uint16_t pressure_freq;	    // frequency of pressure sensor updates in Hertz (1000 / frequency) should be a whole number

void temp_read(void);

void pressure_read(void);