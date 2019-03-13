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

int main() {
	struct pigrrl2_controller_state current_controller_state;
	struct pigrrl2_controller_config controller_config;
	
	//Initialize GPIO and ads
	wiringPiInit();
	//Initialize state
	pigrrl2_controller_init(&current_controller_state);
	//Calibrate the controller
	pigrrl2_config_calibrate(&controller_config);
	int gamepad_out = open("/dev/input/js0", O_WRONLY | O_NONBLOCK);
	if (gamepad_out) {
		printf("Joystick created!");
		while (true) {
			pigrrl2_controller_read(&current_controller_state, &controller_config);
			pigrrl2_controller_state_print(&current_controller_state);
			usleep(FRAME_PERIOD);
		}

	}
	else {
		printf("Failed to open joystick. Reason: %s\n", strerror(errno));
		return ERR_NO_JOYSTICK;
	}
	close(gamepad_out);
	return 0;
}
