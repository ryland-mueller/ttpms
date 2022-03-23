/*
    sensors contains the actual sensor data and related functions
*/

#include <zephyr/types.h>

#define bitset(byte,nbit)   ((byte) |=  (1<<(nbit)))
#define bitclear(byte,nbit) ((byte) &= ~(1<<(nbit)))
#define bitflip(byte,nbit)  ((byte) ^=  (1<<(nbit)))
#define bitcheck(byte,nbit) ((byte) &   (1<<(nbit)))

extern const struct device *i2c_dev;    // the Zephyr I2C device instance

//0th bit = connected   1th bit = connect procedure required    2th bit = disconnect procedure required    3th bit = temp_freq update required
extern uint8_t connection_status;

// Temp values can range from -20 C (0) to 300 C (32000). The last two digits are decimal places (ie 150.43 C = 15043)
// Signed ints are annoying so the range is offset to allow some negative temps (ie -20 C = 0, 0 C = 2000, 100 C = 12000).
extern uint16_t temp[12];

extern uint16_t pressure;

extern uint16_t battery;		// battery voltage * 100, for example 340 = 3.40v

// frequency of temp sensor updates (0x00 = 0.5Hz, 0x01 = 1Hz, 0x02 = 2Hz, 0x03 = 4Hz, 0x04 = 8Hz, 0x05 = 16Hz (default), 0x06 = 32Hz, 0x07 = 64Hz)
extern uint8_t temp_freq;

extern uint8_t pressure_freq;	// frequency of pressure sensor updates in Hertz

void i2c_init(void);

void MLX90641_init(void);

void temp_read(void);

void pressure_read(void);