#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "ttpms_service.h"
#include "sensors.h"

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

void main(void)
{
	ttpms_service_init();
}

void temp_update(void)
{
	while(1)
	{
		k_sleep(K_MSEC(1000 / temp_freq));
		temp_read();
		printk("Attempting to notify temperature!\n");
		ttpms_temp_notify();
	}
}

void pressure_update(void)
{
	while(1)
	{
		k_sleep(K_MSEC(1000 / pressure_freq));
		pressure_read();
		printk("Attempting to notify pressure!\n");
		ttpms_pressure_notify();
	}
}

K_THREAD_DEFINE(temp_update_id, STACKSIZE, temp_update, NULL, NULL, NULL,
				PRIORITY, 0, 0);

K_THREAD_DEFINE(pressure_update_id, STACKSIZE, pressure_update, NULL, NULL, NULL,
				PRIORITY, 0, 0);