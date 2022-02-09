/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

#include <usb/usb_device.h>
#include <drivers/uart.h>

#include "ttpms_display.h"
#include "ttpms_client.h"

void main(void)
{
	const struct device *dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
	uint32_t dtr = 0;
	
	if (usb_enable(NULL)) {
		return;
	}
	
	// Poll if the DTR flag was set
	while (!dtr) {
		uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
		// Give CPU resources to low priority threads.
		k_sleep(K_MSEC(100));
	}

	ttpms_display_init();

	ttpms_client_init();

	ttpms_display_update_test();
}
