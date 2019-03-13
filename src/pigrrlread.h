
#pragma once

#ifdef __KERNEL__
#include <linux/types.h>
#endif
#ifndef __KERNEL__
#include <inttypes.h>
#endif

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

void wiringPiInit(void);
void pigrrl2_config_calibrate(struct pigrrl2_controller_config *out);
void pigrrl2_controller_read(struct pigrrl2_controller_state *out, struct pigrrl2_controller_config *config);
void pigrrl2_controller_init(struct pigrrl2_controller_state *out);
