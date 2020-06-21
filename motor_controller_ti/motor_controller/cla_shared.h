#ifndef _CLA_SHARED_H_
#define _CLA_SHARED_H_
//#############################################################################
// $TI Release: F2837xD Support Library v3.09.00.00 $
// $Release Date: Thu Mar 19 07:35:24 IST 2020 $
// $Copyright:
// Copyright (C) 2013-2020 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

/*==================INCLUDES==================*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==================DEFINES==================*/
#define WAITSTEP     asm(" RPT #255 || NOP")
#define CONTROL_CYCLE_TIME_US   20U // microseconds
#define USE_CLA     1
#define TEST_MODE   1

#define ADC_RESOLUTION          12
#define EPWM1_TIMER_TBPRD       1000U
#define TIMESTEP                (2e-8F * EPWM1_TIMER_TBPRD)
#define PI                      3.1415F

#define MOTOR_DRIVER_DIRECTION_PIN  1U
#define MOTOR_DRIVER_SLEEP_PIN      2U
#define MOTOR_DRIVER_FAULT_IN       3U
#define DEBUG_PIN                   131U

#define MOTOR_SUPPLY_VOLTAGE    12.0F
#define PWM_DUTY_CYCLE_SCALER   (EPWM1_TIMER_TBPRD / MOTOR_SUPPLY_VOLTAGE)

#define MOTOR_SPEED_THRESHOLD_HZ            10000U  // Hz (encoder pulses)

#define CURR_SENSE_LPF_CONST                    2.0F*PI*(10.0F/20000.0F)
#define CURR_SENSE_OFFSET                       2165U       // determined by observing the ADC value after applying a very heavy LPF
#define CURR_SENSE_POS_SCALING_FACTOR_INVERSE   170U        // determined by observing the change in ADC value for +1A of current after applying a very heavy LPF
#define CURR_SENSE_NEG_SCALING_FACTOR_INVERSE   158U        // determined by observing the change in ADC value for -1A of current after applying a very heavy LPF
#define CURR_SENSE_POS_SCALING_FACTOR           (1.0F / CURR_SENSE_POS_SCALING_FACTOR_INVERSE)
#define CURR_SENSE_NEG_SCALING_FACTOR           (1.0F / CURR_SENSE_NEG_SCALING_FACTOR_INVERSE)

#define MOTOR_CONSTANT_KT 0.2525F           // Nm/A

#define KP 91.6F                          // proportional gain for PI controller
#define KI 110090.0F                      // integral gain for PI controller

#define CAN_RX_MSG_OBJ_ID               2
#define COMM_MSG_RECV_DATA_LENGTH       4

#define SATURATE(input, lower_limit, upper_limit) ((input) > (upper_limit) ? (upper_limit) : ((input) < (lower_limit) ? (lower_limit) : (input)))
#define LOW_PASS_FILTER(output, input, lpf_constant) ((output) += (((input) - (output)) * (lpf_constant)))
#define ABS(input) ((input) < 0 ? -(input) : (input))

/*==================CLA VARIABLES==================*/
typedef struct {
    uint16_t enable;
    float currentAmperes;
    float torqueCommand;
} claInputs_S;
typedef struct {
    uint16_t CMPA;
    uint16_t direction;
} claOutputs_S;

extern claInputs_S claInputs;
extern claOutputs_S claOutputs;
extern float errorIntegral; // This variable will have to retain its value between iterations, and since CLA cannot declare static variables, this is declared as a shared variable

/*==================FUNCTION PROTOTYPES==================*/
__interrupt void Cla1Task1();

#ifdef __cplusplus
}
#endif // extern "C"

#endif //end of _CLA_ASIN_SHARED_H_ definition

//
// End of file
//
