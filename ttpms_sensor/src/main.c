#include <zephyr.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(TTPMS);

#include "ttpms_service.h"
#include "sensors.h"

/* size of stack area used by threads */
#define TEMP_STACKSIZE 4096
#define PRESSURE_STACKSIZE 1024

/* scheduling priority used by threads */
#define TEMP_PRIORITY 4			// should these be negative?
#define PRESSURE_PRIORITY 4

void main(void)
{
	LOG_INF("Running TTPMS Sensor on %s", CONFIG_BOARD);

	ttpms_service_init();	// start bluetooth
	i2c_init();				// set up I2C

}

void temp_update(void)
{
	while(1)
	{
		switch (temp_freq)
		{
			case 0x00: 	k_sleep(K_USEC(1000000 * 2)); 	break;	// 0.5Hz
			case 0x01: 	k_sleep(K_USEC(1000000)); 		break;	// 1Hz
			case 0x02: 	k_sleep(K_USEC(1000000 / 2)); 	break;	// 2Hz
			case 0x03: 	k_sleep(K_USEC(1000000 / 4)); 	break;	// 4Hz
			case 0x04: 	k_sleep(K_USEC(1000000 / 8)); 	break;	// 8Hz
			case 0x05: 	k_sleep(K_USEC(1000000 / 16)); 	break;	// 16Hz
			case 0x06: 	k_sleep(K_USEC(1000000 / 32)); 	break;	// 32Hz
			case 0x07:	k_sleep(K_USEC(1000000 / 64)); 	break;	// 64Hz
		}
		if (bitcheck(connection_status, 1)) {	// if we just connected, do connect procedure

			//LOG_INF("Running connect procedure");
			// set VREG enable pin HIGH
			k_sleep(K_MSEC(10));	// let MLX90641 start up, supposedly needs 5ms?
			MLX90641_init();
			bitclear(connection_status, 1);		// clear connect procedure required bit

		} else if (bitcheck(connection_status, 0)) {	// if connected, take reading and update over bluetooth

			//LOG_INF("Running active connection procedure");
			temp_read();
			ttpms_temp_notify();

		} else if (bitcheck(connection_status, 2)) {	// if we just disconnected, do disconnect procedure

			//LOG_INF("Running disconnect procedure");
			// set VREG enable pin LOW
			bitclear(connection_status, 2);		// clear disconnect procedure required bit

		}
		
	}
}

void pressure_update(void)
{
	while(1)
	{
		k_sleep(K_USEC(1000000 / pressure_freq));
		if (bitcheck(connection_status, 0))
		{
			pressure_read();
			ttpms_pressure_notify();
		}
	}
}

K_THREAD_DEFINE(temp_update_id, TEMP_STACKSIZE, temp_update, NULL, NULL, NULL,
				TEMP_PRIORITY, 0, 0);

K_THREAD_DEFINE(pressure_update_id, PRESSURE_STACKSIZE, pressure_update, NULL, NULL, NULL,
				PRESSURE_PRIORITY, 0, 0);
