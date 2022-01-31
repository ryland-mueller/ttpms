#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "ttpms_service.h"

#define TTPMS_BASE_UUID \
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)

static struct bt_uuid_128 ttpms_uuid = BT_UUID_INIT_128(
	TTPMS_BASE_UUID);

static struct bt_uuid_128 temp_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1));

static struct bt_uuid_128 battery_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2));

static struct bt_uuid_128 frequency_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef3));

#define TEMP_MAX_LEN 12

// Temp values can range from -20 C (0) to 300 C (32000). The last two digits are decimal places (ie 150.43 C = 15043)
// Signed ints are annoying so the range is offset to allow some negative temps (ie -20 C = 0, 0 C = 2000, 100 C = 12000).
uint16_t temp[TEMP_MAX_LEN] = {18000, 18500, 19000, 19500,
							   20000, 20500, 21000, 21500,
							   22000, 22500, 23000, 23500};

//uint16_t temp[TEMP_MAX_LEN] = {18000, 18500, 19000, 19500, 20000, 20500};

uint16_t battery = 340;	// battery voltage * 100, for example 340 = 3.40v

uint16_t frequency = 80;		// frequency in hertz? Haven't thought this through yet

static ssize_t read_temp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	printk("Reading temp\n");
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,	// I think this function actually reads the value from temp?
				 sizeof(temp));
}

static ssize_t read_battery(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	printk("Reading battery\n");
	printk("Actual frequency value: %d\n", battery);
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(battery));
}

static ssize_t read_frequency(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	printk("Reading frequency\n");
	printk("Actual frequency value: %d\n", frequency);
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(frequency));
}

static ssize_t write_frequency(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset,
			 uint8_t flags)
{
	printk("Writing frequency with length %d\n", len);
	printk("Offset = %d\n", offset);
	printk("Old actual frequency value: %d\n", frequency);
	uint8_t *value = attr->user_data;
	memcpy(value + offset, buf, len);
	value[offset + len] = 0;
	printk("New actual frequency value: %d\n", frequency);
	return len;
}

static void temp_ccc_cfg_changed(const struct bt_gatt_attr *attr,
				       uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	printk("Temp Notifications %s\n", notif_enabled ? "enabled" : "disabled");
}

/* Vendor Primary Service Declaration */
BT_GATT_SERVICE_DEFINE(ttpms_svc,
	BT_GATT_PRIMARY_SERVICE(&ttpms_uuid),
	BT_GATT_CHARACTERISTIC(&temp_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       read_temp, NULL, temp),
	BT_GATT_CCC(temp_ccc_cfg_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(&battery_uuid.uuid,
			       BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ,
			       read_battery, NULL, &battery),
	BT_GATT_CHARACTERISTIC(&frequency_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_frequency, write_frequency, &frequency),
);

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),	// Limited Discoverable, BR/EDR not supported
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, TTPMS_BASE_UUID),			// Our service
};

void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
	printk("Updated MTU: TX: %d RX: %d bytes\n", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = {
	.att_mtu_updated = mtu_updated
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(void)
{
	int err;

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

void main(void)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	bt_ready();
	bt_gatt_cb_register(&gatt_callbacks);

	while(1)
	{
		k_sleep(K_SECONDS(1));
		printk("Attempting to notify!\n");
		bt_gatt_notify(NULL, &ttpms_svc.attrs[2], temp, sizeof(temp));
	}
}