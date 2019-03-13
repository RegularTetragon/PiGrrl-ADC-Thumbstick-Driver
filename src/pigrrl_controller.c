#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <stdbool.h>
//From the project
#include "pigrrlread.h"

#define DRIVER_AUTHOR "Vincent Mattingly <rotflstar@gmail.com>"
#define DRIVER_DESC "PiGrrl 2 + ADS 1115 joystick driver"

int init_module(void) {
	struct pigrrl2_controller_state current_state;
	struct pigrrl2_controller_config config;

	wiringPiInit();
	pigrrl2_controller_init(&current_state);
	while (true) {
		pigrrl2_controller_read(&current_state, &config);
		msleep(FRAME_PERIOD);
	}
	return 0;
}

void cleanup_module(void) {

}
