
#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <linux/input-event-codes.h>

#define ADS_ADDRESS 0x48
#define ADS_BASE 2222
#define ADS_AXIS_COUNT 4
#define AXIS_X0 ADS_BASE
#define AXIS_Y0 ADS_BASE + 1
#define AXIS_X1 ADS_BASE + 2
#define AXIS_Y1 ADS_BASE + 3
static const int ADS_AXES[ADS_AXIS_COUNT] = { AXIS_X0, AXIS_Y0, AXIS_X1, AXIS_Y1 };

#define ERR_NO_JOYSTICK 	1
#define ERR_BAD_AXIS_ID 	2
#define ERR_FILE_OPEN_FAILURE 	3
#define ERR_FILE_READ_FAILURE 	4
#define ERR_FILE_WRITE_FAILURE 	5

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

#define PIGRRL_CONFIG_PATH "./calibration.dat"

#define BTN_COUNT 16
#define BTN_MAP_PIN 0
#define BTN_MAP_KEY 1

#define FRAME_PERIOD 8


//GPIO pin, buttonmask position, 
static const uint16_t PIGRRL_BTN_MAP[BTN_COUNT][3] = {
	{4, 	PIGRRL_D_LEFT	,	BTN_DPAD_LEFT	},
	{19, 	PIGRRL_D_RIGHT	,	BTN_DPAD_RIGHT	},
	{16,	PIGRRL_D_UP	,	BTN_DPAD_UP	},
	{26,	PIGRRL_D_DOWN	,	BTN_DPAD_DOWN	},
	{14,	PIGRRL_A	,	BTN_A		},
	{15,	PIGRRL_B	,	BTN_B		},
	{20,	PIGRRL_X	,	BTN_X		},
	{18,	PIGRRL_Y	,	BTN_Y		},
	{5,	PIGRRL_SELECT	,	BTN_SELECT	},
	{6,	PIGRRL_START	,	BTN_START	},
	{12,	PIGRRL_L	,	BTN_TL		},
	{13,	PIGRRL_R	,	BTN_TR		},
	{17,	PIGRRL_TFT_1	,	BTN_MODE	},
	{22,	PIGRRL_TFT_2	,	BTN_TL2		},
	{23,	PIGRRL_TFT_3	,	BTN_TR2		},
	{24,	PIGRRL_TFT_4	,	BTN_Z		}
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
bool pigrrl2_config_load(struct pigrrl2_controller_config *out);
void pigrrl2_controller_read(struct pigrrl2_controller_state *out, struct pigrrl2_controller_config *config);
void pigrrl2_controller_init(struct pigrrl2_controller_state *out);
