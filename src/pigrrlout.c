#include <stdio.h>
#include "pigrrlread.h"

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
