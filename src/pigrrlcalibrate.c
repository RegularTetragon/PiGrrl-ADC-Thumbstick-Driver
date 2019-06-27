#include "pigrrlread.h"
#include "pigrrlads.h"
#include "wiringPi.h"
#include "ads1115.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <inttypes.h>
#include <stdbool.h>
#include <sys/stat.h>


void pigrrl2_config_default(struct pigrrl2_controller_config *out) {
	for (int axis = 0; axis < ADS_AXIS_COUNT; axis += 1) {
		int axis_id = ADS_AXES[axis];
		struct ads_axis_config *axis_config = ads_axis_to_config_axis(axis_id, out);
		axis_config->max = 0;
		axis_config->min = UINT16_MAX;
	}
}


bool pigrrl2_config_save(struct pigrrl2_controller_config *config) {
	mkdir(PIGRRL_CONFIG_PATH, 0777);
	int f = open(PIGRRL_CONFIG_FILE, O_WRONLY | O_TRUNC | O_CREAT, 0777);
	if (f) {
		write(f, config, sizeof(struct pigrrl2_controller_config));
		if (errno) {
			fprintf(stderr, "Failed to write file %s. Error: %s.\n", PIGRRL_CONFIG_FILE, strerror(errno));
			return false;
		}
	}
	else {
		fprintf(stderr, "Failed to open file for writing %s. Error: %s.\n", PIGRRL_CONFIG_FILE, strerror(errno));
		return false;
	}
	printf("Saved configuration to %s\n",PIGRRL_CONFIG_PATH);
	return true;
}


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
	printf("Joystick calibration, move the thumbsticks in a circle. Press the B button when finished.\n");
	while (digitalRead(6)) {
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

int main(void) {
	struct pigrrl2_controller_config config;
	wiringPiInit();
	pigrrl2_config_calibrate(&config);
	if (pigrrl2_config_save(&config)) {
		return 0;
	}
	else {
		return ERR_FILE_WRITE_FAILURE;
	}
}
