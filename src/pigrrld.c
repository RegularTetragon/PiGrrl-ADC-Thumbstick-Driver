//From standard
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/uinput.h>

//From project
#include "pigrrlread.h"
#include "pigrrlout.h"

#define PIGRRL_DEV_ID_VENDOR 0x0fff
#define PIGRRL_DEV_ID_PRODUCT 0x0001
#define PIGRRL_DEV_ID_VERSION 0x0001


void set_axis_data(int fd, int axis_code) {
	struct uinput_abs_setup setup;
	memset(&setup, 0, sizeof(setup));
	setup.code = axis_code;
	setup.absinfo.maximum = INT16_MAX;
	setup.absinfo.minimum = INT16_MIN;
	setup.absinfo.flat    = 0;
	setup.absinfo.fuzz    = 5;
	if (ioctl(fd, UI_ABS_SETUP, &setup) < 0) fprintf(stderr, "Failed to setup axis: %s\n", strerror(errno));
}

int init_usetup(struct uinput_setup *usetup) {
	int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "Failed to open uinput: %s\n", strerror(errno));
		exit(1);
	}

	//Initialize button outputs
	if(ioctl(fd, UI_SET_EVBIT, EV_KEY)) printf("Error setting evbit to key: %s\n", strerror(errno));
	for (int i=0;i<BTN_COUNT;i++) {
		if(ioctl(fd, UI_SET_KEYBIT, PIGRRL_BTN_MAP[i][2]) < 0) printf("Error Setting Button: %s", strerror(errno));
	}

	//Initialize analog outputs
	ioctl(fd, UI_SET_EVBIT, EV_ABS);
	if (ioctl(fd, UI_SET_RELBIT, REL_X) < 0) printf("Error Axis X: %s\n", strerror(errno));
	if (ioctl(fd, UI_SET_RELBIT, REL_Y) < 0) printf("Error Axis Y: %s\n", strerror(errno));
	if (ioctl(fd, UI_SET_RELBIT, REL_RX) < 0) printf("Error Axis RX: %s\n", strerror(errno));
	if (ioctl(fd, UI_SET_RELBIT, REL_RY) < 0) printf("Error Axis RY: %s\n", strerror(errno));

	//Blank the usetup and initialize it
	memset(usetup, 0, sizeof(*usetup));
	usetup->id.bustype = BUS_USB;
	usetup->id.vendor = PIGRRL_DEV_ID_VENDOR;
	usetup->id.product = PIGRRL_DEV_ID_PRODUCT;
	usetup->id.version = PIGRRL_DEV_ID_VERSION;

	set_axis_data(fd, ABS_X);
	set_axis_data(fd, ABS_Y);
	set_axis_data(fd, ABS_RX);
	set_axis_data(fd, ABS_RY);	

	strcpy(usetup->name, "ADS1115 GPIO Joystick");
	if (ioctl(fd, UI_DEV_SETUP, usetup) < 0) {
		fprintf(stderr, "Failed to write setup data to uinput: %s\n", strerror(errno));
		exit(2);
	}
	if (ioctl(fd, UI_DEV_CREATE) < 0) { 
		fprintf(stderr, "Failed to create device: %s\n", strerror(errno));
		exit(4);
	}
	return fd;
}


//A really simple helper function taken from Kernel.org's uinput example. Writes an input event to the stream.
void emit(int fd, int type, int code, int val) {
	struct input_event ie;
	ie.type = type;
	ie.code = code;
	ie.value = val;
	/* timestamp values below are ignored */
	ie.time.tv_sec = 0;
	ie.time.tv_usec = 0;

	if (write(fd, &ie, sizeof(ie)) < 0) {
		fprintf(stderr, "Failed to emit event with type %d, code %d, value %d. Reason: %s\n", type, code, val, strerror(errno));
		exit(3);
	}
}

void writeChanges(int fd, struct pigrrl2_controller_state *previous, struct pigrrl2_controller_state *current) {
	printf("Checking changes. Previous: \n");
	pigrrl2_controller_state_print(previous);
	pigrrl2_controller_state_print(current);
	buttonmask btn_release = previous->buttons & (~current->buttons);
	buttonmask btn_pressed = (~previous->buttons) & current->buttons;
	printf("Pressed: %d, Released: %d\nButtons:\n", btn_pressed, btn_release);
	for (int btn = 0; btn < BTN_COUNT; btn++) {
		printf(" Btn: %d ", btn);
		uint16_t mask_check = 1 << btn;
		if (btn_release & mask_check) {
			printf("Released ");
			emit(fd, EV_KEY, PIGRRL_BTN_MAP[btn][2], 0);
		}
		else if (btn_pressed & mask_check) {
			emit(fd, EV_KEY, PIGRRL_BTN_MAP[btn][2], 1);
			printf("Pressed ");
		}
	}
	printf("\n");
	emit(fd, EV_ABS, ABS_X, current->leftstick.x);
	emit(fd, EV_ABS, ABS_Y, current->leftstick.y);
	emit(fd, EV_ABS, ABS_RX, current->rightstick.x);
	emit(fd, EV_ABS, ABS_RY, current->rightstick.y);
	//Send the changes
	emit(fd, EV_SYN, SYN_REPORT, 0);
}



int main() {
	struct pigrrl2_controller_state current_controller_state;
	struct pigrrl2_controller_state previous_controller_state;

	struct pigrrl2_controller_config controller_config;
	struct uinput_setup usetup;

	int fd = init_usetup(&usetup);
	
	//Initialize GPIO and ads
	wiringPiInit();
	//Initialize state
	pigrrl2_controller_init(&current_controller_state);
	//Calibrate the controller
	if (pigrrl2_config_load(&controller_config)) {
		printf("Joystick created!");
		while (true) {
			previous_controller_state = current_controller_state;
			pigrrl2_controller_read(&current_controller_state, &controller_config);
			writeChanges(fd, &previous_controller_state, &current_controller_state);
			usleep(FRAME_PERIOD);
		}
		return 0;
	}
	else {
		fprintf(stderr, "Loading calibration failed. Did you forget to run the calibration program?\n");
	}
	ioctl(fd, UI_DEV_DESTROY);
	close(fd);
}
