//From standard
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

//From project
#include "pigrrlread.h"
#include "pigrrlout.h"
int main() {
	struct pigrrl2_controller_state current_controller_state;
	struct pigrrl2_controller_config controller_config;
	
	//Initialize GPIO and ads
	wiringPiInit();
	//Initialize state
	pigrrl2_controller_init(&current_controller_state);
	//Calibrate the controller
	if (pigrrl2_config_load(&controller_config)) {
		printf("Joystick created!");
		while (true) {
			pigrrl2_controller_read(&current_controller_state, &controller_config);
			pigrrl2_controller_state_print(&current_controller_state);
			usleep(FRAME_PERIOD);
		}
		return 0;
	}
	else {
		fprintf(stderr, "Loading calibration failed. Did you forget to run the calibration program?\n");
	}
}
