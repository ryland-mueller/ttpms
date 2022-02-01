#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include "ttpms_service.h"
#include "sensors.h"

// UUIDs values are in ttpms_service.h
// Base UUID for the TTPMS service
static struct bt_uuid_128 ttpms_uuid = BT_UUID_INIT_128(TTPMS_BASE_UUID);

// UUID for the Temperature characteristic
static struct bt_uuid_128 temp_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION, TEMP_UUID_PART, UUID_PART_4, UUID_PART_5));

// UUID for the Pressure characteristic
static struct bt_uuid_128 pressure_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION, PRESSURE_UUID_PART, UUID_PART_4, UUID_PART_5));

// UUID for the Battery characteristic
static struct bt_uuid_128 battery_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION, BATTERY_UUID_PART, UUID_PART_4, UUID_PART_5));

// UUID for the Temperature Frequency characteristic
static struct bt_uuid_128 temp_freq_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION, TEMP_FREQ_UUID_PART, UUID_PART_4, UUID_PART_5));

// UUID for the Pressure Frequency characteristic
static struct bt_uuid_128 pressure_freq_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(UUID_PART_1, TTPMS_LOCATION, PRESSURE_FREQ_UUID_PART, UUID_PART_4, UUID_PART_5));

static ssize_t read_temp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	printk("Reading temp\n");
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(temp));
}

static ssize_t read_pressure(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	printk("Reading pressure\n");
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(pressure));
}

static ssize_t read_battery(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	printk("Reading battery\n");
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(battery));
}

static ssize_t read_temp_freq(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	printk("Reading temperature frequency\n");
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(temp_freq));
}

static ssize_t write_temp_freq(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	printk("Writing temp frequency with length %d\n", len);
	printk("Offset = %d\n", offset);
	printk("Old actual temp frequency value: %d\n", temp_freq);
	uint8_t *value = attr->user_data;
	memcpy(value + offset, buf, len);
	value[offset + len] = 0;
	printk("New actual temp frequency value: %d\n", temp_freq);
	return len;
}

static ssize_t read_pressure_freq(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	printk("Reading frequency\n");
	printk("Actual frequency value: %d\n", pressure_freq);
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(pressure_freq));
}

static ssize_t write_pressure_freq(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	printk("Writing pressure frequency with length %d\n", len);
	printk("Offset = %d\n", offset);
	printk("Old actual pressure frequency value: %d\n", pressure_freq);
	uint8_t *value = attr->user_data;
	memcpy(value + offset, buf, len);
	value[offset + len] = 0;
	printk("New actual pressure frequency value: %d\n", pressure_freq);
	return len;
}

static void temp_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	printk("Temp Notifications %s\n", notif_enabled ? "enabled" : "disabled");
}

static void pressure_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	printk("Pressure Notifications %s\n", notif_enabled ? "enabled" : "disabled");
}

/* Vendor Primary Service Declaration */
BT_GATT_SERVICE_DEFINE(ttpms_svc,
	BT_GATT_PRIMARY_SERVICE(&ttpms_uuid),
	BT_GATT_CHARACTERISTIC(&temp_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       read_temp, NULL, temp),
	BT_GATT_CCC(temp_ccc_cfg_changed,				// This CCC allows notifications to be enabled for the above characteristic?
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(&pressure_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ,
			       read_pressure, NULL, &pressure),
	BT_GATT_CCC(pressure_ccc_cfg_changed,			// This CCC allows notifications to be enabled for the above characteristic?
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(&battery_uuid.uuid,
			       BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ,
			       read_battery, NULL, &battery),
	BT_GATT_CHARACTERISTIC(&temp_freq_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_temp_freq, write_temp_freq, &temp_freq),
    BT_GATT_CHARACTERISTIC(&pressure_freq_uuid.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_pressure_freq, write_pressure_freq, &pressure_freq),
);

static const struct bt_data ad[] = {										// Advertisement:
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),	// General Discoverable (no timeout), BR/EDR not supported
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, TTPMS_BASE_UUID),					// Our service
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

void ttpms_service_init(void)
{
    int err;

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	bt_ready();
	bt_gatt_cb_register(&gatt_callbacks);
}

void ttpms_temp_notify(void)
{
    bt_gatt_notify(NULL, &ttpms_svc.attrs[2], temp, sizeof(temp));
}

void ttpms_pressure_notify(void)
{
    bt_gatt_notify(NULL, &ttpms_svc.attrs[4], &pressure, sizeof(pressure));
}