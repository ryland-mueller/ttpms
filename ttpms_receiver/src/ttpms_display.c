#include "ttpms_display.h"

uint32_t count = 0U;		// number value
char count_str[11] = {0};	// number string
const struct device *display_dev;

static lv_style_t style_1;
lv_obj_t *boot_label_1;
lv_obj_t *boot_label_2;
lv_obj_t *FL_label;
lv_obj_t *FL_connection_label;
lv_obj_t *FL_battery_label;
lv_obj_t *FR_label;
lv_obj_t *FR_connection_label;
lv_obj_t *FR_battery_label;
lv_obj_t *RL_label;
lv_obj_t *RL_connection_label;
lv_obj_t *RL_battery_label;
lv_obj_t *RR_label;
lv_obj_t *RR_connection_label;
lv_obj_t *RR_battery_label;
static lv_point_t *vertical_line_points;
static lv_point_t *horizontal_line_points;
lv_obj_t *vertical_line;
lv_obj_t *horizontal_line;
static lv_style_t style_line;

void ttpms_display_init(void)
{

	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		printk("device not found.  Aborting test.");
		return;
	}

	lv_style_init(&style_1);
	lv_style_set_text_letter_space(&style_1, LV_STATE_DEFAULT, -1);

	boot_label_1 = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(boot_label_1, "TTPMS v1.0");
	lv_label_set_align(boot_label_1, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(boot_label_1, NULL, LV_ALIGN_CENTER, 0, -16);

	boot_label_2 = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_text(boot_label_2, "Schulich Racing\n\nRyland Mueller");
	lv_label_set_align(boot_label_2, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(boot_label_2, NULL, LV_ALIGN_CENTER, 0, 14);

	lv_task_handler();

	k_sleep(K_MSEC(3000));

	lv_obj_set_hidden(boot_label_1, true);
	lv_obj_set_hidden(boot_label_2, true);

	FL_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(FL_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 6);
	lv_label_set_text(FL_label, "FL");

	FL_connection_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(FL_connection_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 18);
	lv_label_set_text(FL_connection_label, "Connected");
	lv_obj_add_style(FL_connection_label, LV_LABEL_PART_MAIN, &style_1);

	FL_battery_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(FL_battery_label, NULL, LV_ALIGN_IN_TOP_LEFT, 36, 6);
	lv_label_set_long_mode(FL_battery_label, LV_LABEL_LONG_CROP);
	lv_obj_set_width(FL_battery_label, 24);
	lv_label_set_text(FL_battery_label, "");
	lv_label_set_align(FL_battery_label, LV_LABEL_ALIGN_RIGHT);

	FR_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(FR_label, NULL, LV_ALIGN_IN_TOP_LEFT, 66, 6);
	lv_label_set_text(FR_label, "FR");

	FR_connection_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(FR_connection_label, NULL, LV_ALIGN_IN_TOP_LEFT, 66, 18);
	lv_label_set_text(FR_connection_label, "Scanning");
	lv_obj_add_style(FR_connection_label, LV_LABEL_PART_MAIN, &style_1);

	FR_battery_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(FR_battery_label, NULL, LV_ALIGN_IN_TOP_LEFT, 102, 6);
	lv_label_set_long_mode(FR_battery_label, LV_LABEL_LONG_CROP);
	lv_obj_set_width(FR_battery_label, 24);
	lv_label_set_text(FR_battery_label, "");
	lv_label_set_align(FR_battery_label, LV_LABEL_ALIGN_RIGHT);

	RL_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(RL_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 41);
	lv_label_set_text(RL_label, "RL");

	RL_connection_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(RL_connection_label, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 53);
	lv_label_set_text(RL_connection_label, "Connected");
	lv_obj_add_style(RL_connection_label, LV_LABEL_PART_MAIN, &style_1);

	RL_battery_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(RL_battery_label, NULL, LV_ALIGN_IN_TOP_LEFT, 36, 41);
	lv_label_set_long_mode(RL_battery_label, LV_LABEL_LONG_CROP);
	lv_obj_set_width(RL_battery_label, 24);
	lv_label_set_text(RL_battery_label, "");
	lv_label_set_align(RL_battery_label, LV_LABEL_ALIGN_RIGHT);

	RR_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(RR_label, NULL, LV_ALIGN_IN_TOP_LEFT, 66, 41);
	lv_label_set_text(RR_label, "RR");

	RR_connection_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(RR_connection_label, NULL, LV_ALIGN_IN_TOP_LEFT, 66, 53);
	lv_label_set_text(RR_connection_label, "Connected");
	lv_obj_add_style(RR_connection_label, LV_LABEL_PART_MAIN, &style_1);

	RR_battery_label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_align(RR_battery_label, NULL, LV_ALIGN_IN_TOP_LEFT, 102, 41);
	lv_label_set_long_mode(RR_battery_label, LV_LABEL_LONG_CROP);
	lv_obj_set_width(RR_battery_label, 24);
	lv_label_set_text(RR_battery_label, "");
	lv_label_set_align(RR_battery_label, LV_LABEL_ALIGN_RIGHT);
	
    vertical_line_points = (lv_point_t [2]) { {64, 0}, {64, 64} };
	vertical_line = lv_line_create(lv_scr_act(), NULL);
    lv_line_set_points(vertical_line, vertical_line_points, 2);

	horizontal_line_points = (lv_point_t [2]) { {0, 32}, {128, 32} };
	horizontal_line = lv_line_create(lv_scr_act(), NULL);
    lv_line_set_points(horizontal_line, horizontal_line_points, 2);

    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 1);
	lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_WHITE);

	lv_obj_add_style(vertical_line, LV_LINE_PART_MAIN, &style_line);
	lv_obj_add_style(horizontal_line, LV_LINE_PART_MAIN, &style_line);

	lv_task_handler();
}

//  actual functions should be update_temp, update_battey, update_connection, etc that are not blocking

void ttpms_display_update_test(void)    // this function runs forever to test/update display values
{
    while (1) {
		if ((count % 100) == 0U) {
			sprintf(count_str, "%d%%", count/100U);
			lv_label_set_text(FL_battery_label, count_str);
			lv_label_set_text(FR_battery_label, count_str);
			lv_label_set_text(RL_battery_label, count_str);
			lv_label_set_text(RR_battery_label, count_str);
		}
		lv_task_handler();
		k_sleep(K_MSEC(2));
		++count;
	}
}