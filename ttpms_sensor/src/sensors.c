#include <zephyr.h>
#include <drivers/i2c.h>

#include <math.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(TTPMS);

#include "sensors.h"
#include "MLX90641_API.h"
#include "MLX90641_I2C_Driver.h"

const struct device *i2c_dev;    // the Zephyr I2C device instance		

static float mlx90641To[192];
paramsMLX90641 mlx90641;
float vdd;
float Ta;

// The below floats should be added as BLE characteristics for tuning

float ta_shift = 8;			// Default shift for MLX90641 in open air
float emissivity = 0.95;	// Tune this with testing

//0th bit = connected   1th bit = connect procedure required    2th bit = disconnect procedure required    3th bit = temp_freq update required
uint8_t connection_status;

// Temp values can range from -20 C (0) to 300 C (32000). The last two digits are decimal places (ie 150.43 C = 15043)
// Signed ints are annoying so the range is offset to allow some negative temps (ie -20 C = 0, 0 C = 2000, 100 C = 12000).
uint16_t temp[12] = {18000, 18500, 19000, 19500,
					 20000, 20500, 21000, 21500,
					 22000, 22500, 23000, 23500};

uint16_t pressure = 100;

uint16_t battery = 340;		// battery voltage * 100, for example 340 = 3.40v

// frequency of temp sensor updates (0x00 = 0.5Hz, 0x01 = 1Hz, 0x02 = 2Hz, 0x03 = 4Hz, 0x04 = 8Hz, 0x05 = 16Hz (default), 0x06 = 32Hz, 0x07 = 64Hz)
uint8_t temp_freq = 0x05;

uint8_t pressure_freq = 20;	// frequency of pressure sensor updates in Hertz

void i2c_init(void)
{
	i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));
    i2c_configure(i2c_dev, I2C_SPEED_SET(I2C_SPEED_FAST));
}

void MLX90641_init(void)
{

	int status;
	uint16_t eeMLX90641[832];

    status = MLX90641_DumpEE(MLX90641_ADDR, eeMLX90641);
    if (status != 0) {
        LOG_INF("Failed to load system parameters, MLX90641_DumpEE() returned %d", status);
    }
	
    status = MLX90641_ExtractParameters(eeMLX90641, &mlx90641);
    if (status != 0) {
        LOG_INF("Parameter extraction failed, MLX90641_ExtractParameters() returned %d", status);
    }

	status = MLX90641_SetRefreshRate(MLX90641_ADDR, temp_freq);
	if (status != 0) {
        LOG_INF("Setting refresh rate failed, MLX90641_SetRefreshRate() returned %d", status);
    }
}

void temp_read(void)
{
	int status;

	uint16_t mlx90641Frame[242];
	status = MLX90641_GetFrameData(MLX90641_ADDR, mlx90641Frame);
	if (status < 0) {
		LOG_INF("GetFrame Error: %d", status);
	}

	vdd = MLX90641_GetVdd(mlx90641Frame, &mlx90641);
	battery = round(vdd)*100;

	Ta = MLX90641_GetTa(mlx90641Frame, &mlx90641);

	float tr = Ta - ta_shift; //Reflected temperature based on the sensor ambient temperature

	MLX90641_CalculateTo(mlx90641Frame, &mlx90641, emissivity, tr, mlx90641To);

	// Average nearby pixels to get the 12 measurement points
	// Only middle four rows are used. Rightmost column is not used.
	// Also converts float to offset int system (see temp[] comment)
	for (int i = 0; i < 12; i++)
	{
		temp[i] = round(((	mlx90641To[64 + i] + mlx90641To[65 + i] +
			   				mlx90641To[80 + i] + mlx90641To[81 + i] +
			   		 		mlx90641To[96 + i] + mlx90641To[97 + i] +
			   		 		mlx90641To[80 + i] + mlx90641To[81 + i]) / 8 + 20) * 100);
	}
	//LOG_INF("temp[12]: %d %d %d %d %d %d %d %d %d %d %d %d", temp[0], temp[1], temp[2], temp[3], temp[4], temp[5],
	//														 temp[6], temp[7], temp[8], temp[9], temp[10], temp[11]);

	if (bitcheck(connection_status, 3))	// check if MLX90641 refresh rate needs to be updated
	{
		status = MLX90641_SetRefreshRate(MLX90641_ADDR, temp_freq);
		if (status != 0) {
        	LOG_INF("Setting refresh rate failed, MLX90641_SetRefreshRate() returned %d", status);
    	} else {
			bitclear(connection_status, 3);
		}
	}
    
}

void pressure_read(void)
{
    pressure++;
}