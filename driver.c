#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <time.h>
#include <inttypes.h>
#include <errno.h>
//From wiringPi
#include <ads1115.h>

#define ADS_ADDRESS 0x48
#define ADS_BASE 2222


#define ERR_NO_JOYSTICK 1
#define MAX_FRAME_EVENTS 4

#define PI_GRRL_NOP		0b0000000000000000
#define PI_GRRL_A		0b0000000000000001
#define PI_GRRL_B		0b0000000000000010
#define PI_GRRL_X		0b0000000000000100
#define PI_GRRL_Y		0b0000000000001000
#define PI_GRRL_Start		0b0000000000010000
#define PI_GRRL_Select		0b0000000000100000
#define PI_GRRL_D_UP		0b0000000001000000
#define PI_GRRL_D_DOWN		0b0000000010000000
#define PI_GRRL_D_LEFT		0b0000000100000000
#define PI_GRRL_D_RIGHT		0b0000001000000000
//Triggers
#define PI_GRRL_L		0b0000010000000000
#define PI_GRRL_R		0b0000100000000000
//Screen buttons
#define PI_GRRL_S_1		0b0001000000000000
#define PI_GRRL_S_2		0b0010000000000000
#define PI_GRRL_S_3		0b0100000000000000
#define PI_GRRL_S_4		0b1000000000000000

typedef uint16_t buttonmask;


struct analog_axis_vector {
	int16_t x;
	int16_t y;
};

const struct analog_axis_vector ANALOG_AXIS_VECTOR_0 = {0, 0};

struct pi_grrl2_controller_state {
	buttonmask buttons;
	struct analog_axis_vector leftstick;
	struct analog_axis_vector rightstick;
	struct analog_axis_vector pitft;
};

struct possible_js_event {
	bool active;
	struct js_event event;
};


struct js_event_list {
	struct possible_js_event buttons;
	struct possible_js_event axis_x1;
	struct possible_js_event axis_x2;
	struct possible_js_event axis_y1;
	struct possible_js_event axis_y2;
};

void js_event_list_init(struct js_event_list *events) {
	events->buttons.active = false;
	events->axis_x1.active = false;
	events->axis_x2.active = false;
	events->axis_y1.active = false;
	events->axis_y2.active = false;
}

void pi_grrl2_controller_init(struct pi_grrl2_controller_state *state) {
	state->buttons = PI_GRRL_NOP;
	state->leftstick = ANALOG_AXIS_VECTOR_0;
	state->rightstick = ANALOG_AXIS_VECTOR_0;
	state->pitft = ANALOG_AXIS_VECTOR_0;
}

struct js_event_list states_to_events(struct pi_grrl2_controller_state state_prior, struct pi_grrl2_controller_state state_current) {
	struct js_event_list events;
	//initialize.
	js_event_list_init(&events);
	
	//Check buttons:
	return events;
}

int main() {
	int frame_event_number = 0;
	struct js_event frame_events[MAX_FRAME_EVENTS];
	struct pi_grrl2_controller_state current_controller_state;
	
	//Initialize state
	pi_grrl2_controller_init(&current_controller_state);
	//Initialize ADC:
	ads1115Setup(ADS_BASE, ADS_ADDRESS);

	int gamepad_out = open("/dev/input/js0", O_WRONLY | O_NONBLOCK);
	if (gamepad_out) {
		printf("Joystick created!");
		while (true) {
			printf("<0x%x, 0x%x> <0x%x, 0x%x>\n", analogRead(ADS_BASE), analogRead(ADS_BASE+1), analogRead(ADS_BASE+2), analogRead(ADS_BASE+3));
		}

	}
	else {
		printf("Failed to open joystick. Reason: %s\n", strerror(errno));
		return ERR_NO_JOYSTICK;
	}
	close(gamepad_out);
	return 0;
}
