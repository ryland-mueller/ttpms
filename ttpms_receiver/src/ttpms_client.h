#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/scan.h>

#define UUID_PART_1				    0x7f0413db		// randomly generated
#define TTPMS_LOCATION_FL			0xFF01
#define TTPMS_LOCATION_FR			0xFF02
#define TTPMS_LOCATION_RL			0xFF03
#define TTPMS_LOCATION_RR			0xFF04
#define BASE_UUID_PART			    0x00FF			// used for base service UUID
#define UUID_PART_4				    0x9c11			// randomly generated
#define UUID_PART_5				    0x8f8275ff78c4	// randomly generated

#define TTPMS_BASE_UUID_FL BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION_FL, BASE_UUID_PART, UUID_PART_4, UUID_PART_5)
#define TTPMS_BASE_UUID_FR BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION_FR, BASE_UUID_PART, UUID_PART_4, UUID_PART_5)
#define TTPMS_BASE_UUID_RL BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION_RL, BASE_UUID_PART, UUID_PART_4, UUID_PART_5)
#define TTPMS_BASE_UUID_RR BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION_RR, BASE_UUID_PART, UUID_PART_4, UUID_PART_5)

void ttpms_client_init(void);