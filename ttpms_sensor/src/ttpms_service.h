/*
    ttpms_service contains all the Bluetooth-related functions of the TTPMS system,
    such as the GATT TTPMS service
*/

#include <zephyr/types.h>

#include "sensors.h"

#define UUID_PART_1				    0x7f0413db		// randomly generated
#define TTPMS_LOCATION			    0xFF01 			// 0xFF01 = FL    0xFF02 = FR    0xFF03 = RL    0xFF04 = RR
#define BASE_UUID_PART			    0x00FF			// used for base service UUID
#define TEMPERATURE_UUID_PART	    0x01FF			// used for temperature characteristic UUID
#define PRESSURE_UUID_PART			0x02FF			// used for pressure characteristic UUID
#define BATTERY_UUID_PART		    0x03FF			// used for battery characteristic UUID
#define TEMP_FREQ_UUID_PART		    0x04FF			// used for temperature frequency characteristic UUID
#define PRESSURE_FREQ_UUID_PART		0x05FF			// used for pressure frequency characteristic UUID
#define UUID_PART_4				    0x9c11			// randomly generated
#define UUID_PART_5				    0x8f8275ff78c4	// randomly generated

#define TTPMS_BASE_UUID BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION, BASE_UUID_PART, UUID_PART_4, UUID_PART_5)

void ttpms_service_init(void);

void ttpms_temp_notify(void);

void ttpms_pressure_notify(void);