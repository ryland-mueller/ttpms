#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include "sensors.h"

// Temp values can range from -20 C (0) to 300 C (32000). The last two digits are decimal places (ie 150.43 C = 15043)
// Signed ints are annoying so the range is offset to allow some negative temps (ie -20 C = 0, 0 C = 2000, 100 C = 12000).
uint16_t temp[12] = {18000, 18500, 19000, 19500,
					 20000, 20500, 21000, 21500,
					 22000, 22500, 23000, 23500};

uint16_t pressure = 4000;

uint16_t battery = 340;		    // battery voltage * 100, for example 340 = 3.40v

uint16_t temp_freq = 10;	    // frequency of temp sensor updates in Hertz (1000 / frequency) should be a whole number

uint16_t pressure_freq = 20;	// frequency of pressure sensor updates in Hertz (1000 / frequency) should be a whole number

void temp_read(void)
{
    temp[0] += 50;
	temp[1] += 40;
	temp[2] += 30;
}

void pressure_read(void)
{
    pressure += 5;
}