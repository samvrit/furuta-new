/*
 * core_controls.h
 *
 *  Created on: 20-Nov-2022
 *      Author: Samvrit
 */

#ifndef CONTROLS_CORE_CONTROLS_H_
#define CONTROLS_CORE_CONTROLS_H_

#include <stdbool.h>
#include <stdint.h>

__interrupt void epwm3ISR(void);

void controller_init(void);
void controller_get_measurements(float measurements_output[3]);
void controller_get_state_estimate(float x_hat_output[6]);
uint16_t controller_get_rls_error_bitfield(void);

#endif /* CONTROLS_CORE_CONTROLS_H_ */