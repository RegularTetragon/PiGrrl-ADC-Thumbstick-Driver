
//From standard library
#include <stdbool.h>

#ifndef __KERNEL__
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <inttypes.h>
	#include <unistd.h>
#endif
#ifdef __KERNEL__
	#include <linux/module.h>
	#include <linux/types.h>
	#include <linux/delay.h>
	#include <linux/string.h>
#endif

//From wiringPi
#include <ads1115.h>
#include <wiringPi.h>
//From project
#include "pigrrlread.h"



const struct analog_axis_vector ANALOG_AXIS_VECTOR_0 = {0, 0};

struct ads_axis_config* ads_axis_to_config_axis(int axis_id, struct pigrrl2_controller_config *config) {
	switch (axis_id) {
		case AXIS_X0:
			return &(config->x0);
		break;
		case AXIS_X1:
			return &(config->x1);
		break;
		case AXIS_Y0:
			return &(config->y0);
		break;
		case AXIS_Y1:
			return &(config->y1);
		break;
		default:
			#ifndef __KERNEL__
			fprintf(stderr, "Invalid axis ID\n");
			exit(ERR_BAD_AXIS_ID);
			#else
			printk(KERN_ERR "Invalid axis ID\n");
			#endif

		break;
	}
}


void pigrrl2_controller_init(struct pigrrl2_controller_state *out) {
	out->buttons = PIGRRL_NOP;
	out->leftstick = ANALOG_AXIS_VECTOR_0;
	out->rightstick = ANALOG_AXIS_VECTOR_0;
	out->pitft = ANALOG_AXIS_VECTOR_0;
}

void pigrrl2_controller_read_btns(struct pigrrl2_controller_state *out) {
	out->buttons = PIGRRL_NOP;
	for (int i=0; i < BTN_COUNT; i+=1) {
		//PiGrrl buttons are 0 hot
		if(!digitalRead(PIGRRL_BTN_MAP[i][BTN_MAP_PIN])) {
			out->buttons |= PIGRRL_BTN_MAP[i][BTN_MAP_KEY];
		}
	}
}
//Inputs:
//	uint16_t 			axis_value: 	The current read for the given channel 
//	int	 			axis_id:	AXIS_X0, AXIS_Y0, AXIS_X1, AXIS_Y1, the current axis being read from
//	pigrrl2_controller_config	*config:	The current configuration containing bounds information of each axis. Modified if axis_value is out of bounds.
//Returns:
//	int16_t				return value:	The input mapped from int16 min to int16 max
//Side effects:
//	Actively recalibrates the joystick if you go out of range. Does not account for drift.
int16_t ads_axis_to_controller_axis(uint16_t axis_value, int axis_id, struct pigrrl2_controller_config *config) {
	struct ads_axis_config *axis_config = ads_axis_to_config_axis(axis_id, config);
	int32_t out = ((int32_t)axis_value - axis_config->neutral) * (int32_t)axis_config->max / (int32_t)axis_config->min;
	if (out > INT16_MAX) {
		axis_config->max = axis_value;
		return INT16_MAX;
	}
	else if(out < INT16_MIN) {
		axis_config->min = axis_value;
		return INT16_MIN;
	}
	return (int16_t)out;
}

void pigrrl2_controller_read_analog(struct pigrrl2_controller_state *out, struct pigrrl2_controller_config *config) {
	out->leftstick.x  = ads_axis_to_controller_axis(analogRead(AXIS_X0), AXIS_X0, config);
	out->leftstick.y  = ads_axis_to_controller_axis(analogRead(AXIS_Y0), AXIS_Y0, config);
	out->rightstick.x = ads_axis_to_controller_axis(analogRead(AXIS_X1), AXIS_X1, config);
	out->rightstick.y = ads_axis_to_controller_axis(analogRead(AXIS_Y1), AXIS_Y1, config);
}

void pigrrl2_controller_read(struct pigrrl2_controller_state *out, struct pigrrl2_controller_config *config) {
	pigrrl2_controller_read_btns(out);
	pigrrl2_controller_read_analog(out, config);
}

void pigrrl2_config_default(struct pigrrl2_controller_config *out) {
	for (int axis = 0; axis < ADS_AXIS_COUNT; axis += 1) {
		int axis_id = ADS_AXES[axis];
		struct ads_axis_config *axis_config = ads_axis_to_config_axis(axis_id, out);
		axis_config->max = 0;
		axis_config->min = UINT16_MAX;
	}
}

//TODO: Add deadzone
void pigrrl2_config_calibrate_neutral(struct pigrrl2_controller_config *out) {
	printf("Please don't touch the thumbsticks...\n");
	for (int axis = 0; axis < ADS_AXIS_COUNT; axis += 1) {
		int axis_id = ADS_AXES[axis];
		struct ads_axis_config *axis_config = ads_axis_to_config_axis(axis_id, out);
		uint16_t axis_value = analogRead(axis_id);
		axis_config->neutral = axis_value;
	}
}
void pigrrl2_config_calibrate_bounds(struct pigrrl2_controller_config *out) {
	printf("Joystick calibration, move the thumbsticks in a circle. Press the A button when finished.\n");
	while (digitalRead(14)) {
		for (int axis = 0; axis < ADS_AXIS_COUNT; axis += 1) {
			int axis_id = ADS_AXES[axis];
			struct ads_axis_config *axis_config = ads_axis_to_config_axis(axis_id, out);
			uint16_t axis_value = analogRead(axis_id);
			axis_config->max = axis_value > axis_config->max ? axis_value : axis_config->max;
			axis_config->min = axis_value < axis_config->min ? axis_value : axis_config->min;
		}
		usleep(FRAME_PERIOD);
	}
	printf("Done.");
}
void pigrrl2_config_calibrate(struct pigrrl2_controller_config *out) {
	pigrrl2_config_default(out);
	pigrrl2_config_calibrate_neutral(out);
	pigrrl2_config_calibrate_bounds(out);
}

void wiringPiInit() {
	wiringPiSetupGpio();
	ads1115Setup(ADS_BASE, ADS_ADDRESS);
}

