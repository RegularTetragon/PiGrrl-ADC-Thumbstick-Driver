#include "pigrrlread.h"
struct ads_axis_config* ads_axis_to_config_axis(int axis_id, struct pigrrl2_controller_config *config);
int16_t ads_axis_to_controller_axis(uint16_t axis_value, int axis_id, struct pigrrl2_controller_config *config);
