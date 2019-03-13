#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <time.h>
#include <inttypes.h>
#include <errno.h>
//From wiringPi
#include <ads1115.h>
#include <wiringPi.h>

#define ADS_ADDRESS 0x48
#define ADS_BASE 2222
#define ADS_AXIS_COUNT 4
#define AXIS_X0 ADS_BASE
#define AXIS_Y0 ADS_BASE + 1
#define AXIS_X1 ADS_BASE + 2
#define AXIS_Y1 ADS_BASE + 3
static const int ADS_AXES[ADS_AXIS_COUNT] = { AXIS_X0, AXIS_Y0, AXIS_X1, AXIS_Y1 };

#define ERR_NO_JOYSTICK 1
#define ERR_BAD_AXIS_ID 2
#define MAX_FRAME_EVENTS 4


#define PIGRRL_NOP 	0
#define PIGRRL_A 	1 << 0
#define PIGRRL_B 	1 << 1
#define PIGRRL_X 	1 << 2
#define PIGRRL_Y 	1 << 3
#define PIGRRL_START 	1 << 4
#define PIGRRL_SELECT 	1 << 5
#define PIGRRL_D_UP 	1 << 6
#define PIGRRL_D_DOWN 	1 << 7
#define PIGRRL_D_LEFT 	1 << 8
#define PIGRRL_D_RIGHT  1 << 9
#define PIGRRL_L 	1 << 10
#define PIGRRL_R 	1 << 11
#define PIGRRL_TFT_1 	1 << 12
#define PIGRRL_TFT_2 	1 << 13
#define PIGRRL_TFT_3 	1 << 14
#define PIGRRL_TFT_4 	1 << 15

#define BTN_COUNT 16
#define BTN_MAP_PIN 0
#define BTN_MAP_KEY 1

#define FRAME_PERIOD 8
static const uint16_t PIGRRL_BTN_MAP[BTN_COUNT][2] = {
	{4, 	PIGRRL_D_LEFT	},
	{19, 	PIGRRL_D_RIGHT	},
	{16,	PIGRRL_D_UP	},
	{26,	PIGRRL_D_DOWN	},
	{14,	PIGRRL_A	},
	{15,	PIGRRL_B	},
	{20,	PIGRRL_X	},
	{18,	PIGRRL_Y	},
	{5,	PIGRRL_SELECT  },
	{6,	PIGRRL_START	},
	{12,	PIGRRL_L	},
	{13,	PIGRRL_R	},
	{17,	PIGRRL_TFT_1	},
	{22,	PIGRRL_TFT_2	},
	{23,	PIGRRL_TFT_3	},
	{24,	PIGRRL_TFT_4	}
};


typedef uint16_t buttonmask;


struct analog_axis_vector {
	int16_t x;
	int16_t y;
};

const struct analog_axis_vector ANALOG_AXIS_VECTOR_0 = {0, 0};

struct pigrrl2_controller_state {
	buttonmask buttons;
	struct analog_axis_vector leftstick;
	struct analog_axis_vector rightstick;
	struct analog_axis_vector pitft;
};

struct ads_axis_config {
	uint16_t neutral;
	uint16_t max;
	uint16_t min;
};

struct pigrrl2_controller_config {
	struct ads_axis_config x0;
	struct ads_axis_config x1;
	struct ads_axis_config y0;
	struct ads_axis_config y1;
};

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
			fprintf(stderr, "Invalid axis ID\n");
			exit(ERR_BAD_AXIS_ID);
		break;
	}
}

void pigrrl2_controller_state_print(const struct pigrrl2_controller_state *state) {
	printf(
		"Buttons: %x Left stick: <%f, %f> Right stick: <%f, %f>\n",
		state->buttons,
		(float)state->leftstick.x / INT16_MAX,
		(float)state->leftstick.y / INT16_MAX,
		(float)state->rightstick.x / INT16_MAX,
		(float)state->rightstick.y / INT16_MAX
	);
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

int16_t ads_axis_to_controller_axis(uint16_t axis_value, int axis_id, struct pigrrl2_controller_config *config) {
	struct ads_axis_config *axis_config = ads_axis_to_config_axis(axis_id, config);
	assert(axis_config->max != axis_config->min);
	int32_t out = ((int32_t)axis_value - axis_config->neutral) * (int32_t)axis_config->max / (int32_t)axis_config->min;
	if (out > INT16_MAX) {
		return INT16_MAX;
	}
	else if(out < INT16_MIN) {
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
	//for (int i=0; i<500;i++) {
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

int main() {
	int frame_event_number = 0;
	struct js_event frame_events[MAX_FRAME_EVENTS];
	struct pigrrl2_controller_state current_controller_state;
	struct pigrrl2_controller_config controller_config;
	
	//Initialize GPIO and ads
	wiringPiSetupGpio();
	ads1115Setup(ADS_BASE, ADS_ADDRESS);
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
