/*=======================INCLUDES========================*/
#include "mbed.h"
#include "PwmIn.h"

/*=======================DEFINES========================*/
#define PI 3.14F
#define MICROSECOND 0.000001F

#define GEARBOX_RATIO 30.0F
#define ROTATION_PER_PULSE (2.0F * PI / (16.0F * GEARBOX_RATIO))  // radians per pulse
#define MOTOR_SPEED_THRESHOLD_RAD_S 50000.0F

#define CURRENT_LPF_CUTOFF_FREQ_HZ 3500U    // cutoff frequency for low pass filter
#define CURRENT_SENSE_OFFSET 0.091F         // ADC value when 0 current is present
#define CURRENT_SENSE_SCALING_FACTOR 13.73F // calculated by considering rate of change of voltage w.r.t. current, as well as voltage divider circuit (5V -> 3V)
#define CURRENT_SENSE_LOWER_BOUND 0.0F
#define CURRENT_SENSE_UPPER_BOUND 10.0F
#define MOTOR_CONSTANT_KT 0.2525F           // Nm/A

#define KP 166.08F                          // proportional gain for PI controller
#define KI 26161.30F                        // integral gain for PI controller

#define DUTY_CYCLE_LOWER_BOUND 0.0F
#define DUTY_CYCLE_UPPER_BOUND 1.0F
#define MOTOR_PWM_FREQUENCY_US 50.0F

#define SATURATE(input, lower_limit, upper_limit) ((input) > (upper_limit) ? (upper_limit) : ((input) < (lower_limit) ? (lower_limit) : (input)))
#define LOW_PASS_FILTER(output, input, dt, cutoff_freq) ((output) += (((input) - (output)) * 2 * PI * (cutoff_freq) * (dt) * MICROSECOND))
#define ABS(input) ((input) < 0 ? -(input) : (input))

/*=======================PERIPHERALS========================*/
DigitalOut motorEnable(PA_8);               // 3.3V power source for gate driver IC
PwmOut motorPWM(PA_5);                      // PWM for gate driver IC
DigitalOut inA(PA_6);                       // direction pin A
DigitalOut inB(PA_7);                       // direction pin B
AnalogIn currentSenseExternal(PA_4);        // current sense analog input
AnalogIn currentSenseDriver(PB_0);          // current sense from the motor driver
PwmIn encoder(PB_6);                        // encoder input

InterruptIn buttonPress(BUTTON1, PullDown);                // button for motor enable/disable

Timer t;                                    // timer for calculating code execution rate

RawSerial torqueInput(PA_0, PA_1);         // serial communication for getting torque commands

/*=======================STRUCTS & UNIONS========================*/
// use a union to decode float packet received through UART
typedef union {
    float value;
    char buffer[sizeof(float)];
} uartPacket_t;

typedef struct {
    bool motorEnabled = false;              // flag for when motor is enabled using button
    bool torqueCommandAvailable = false;    // flag for when new torque command is available
} flags_t;

/*=======================VOLATILES========================*/
volatile char rx_buffer[sizeof(float) + 1]; // receive buffer for UART packet containing torque command
volatile int rx_count;                      // index for receive buffer byte array
volatile flags_t flags;
volatile uartPacket_t uartPacket;

/*=======================INTERRUPT HANDLERS========================*/
// Interrupt service routine for when user button is pressed for enabling/disabling motor driver
void flip(void)
{
    flags.motorEnabled = !flags.motorEnabled;
}

// Interrupt service routine for UART RX
void rx_irq(void)
{
    while(torqueInput.readable())
    {
        rx_buffer[rx_count] = torqueInput.getc();  // fill available byte into buffer
        if(rx_buffer[rx_count] == '\r')             // look for carriage return
        {
            flags.torqueCommandAvailable = true;
            rx_count = 0;                           // reset buffer index
            break;
        }
        else
        {
            rx_count++;                             // increment index counter
        }
    }
}

/*=======================MAIN========================*/
int main()
{
    torqueInput.baud(115200);                          // set UART baud rate
    torqueInput.attach(&rx_irq, RawSerial::RxIrq);

    torqueInput.printf("Hello World!\n");

    buttonPress.rise(&flip);

    inA = 1;
    inB = 0;

    float dutyCycle = 0.0;
    float currentSenseLPF = 0.0;
    float currentSenseRaw = 0.0;
    float torqueCommand = 0.02;
    float torqueFeedback = 0.0;
    float torqueError = 0.0;
    float torqueErrorIntegral = 0.0;
    int executionRate_ms = 0;

    float motorSpeed = 0.0;
    float dt = 1;

    motorPWM.period_us(MOTOR_PWM_FREQUENCY_US); // set PWM frequency to 100 microseconds (10 kHz)
    
    while(true)
    {
        if(flags.torqueCommandAvailable)
        {
            NVIC_DisableIRQ(UART4_IRQn);    // disable UART interrupt while processing new information
            memcpy((void *)&uartPacket.buffer, (void *)&rx_buffer, sizeof(float));  // write buffer contents into union variable
            memset((void *)&rx_buffer, 0, sizeof(float) + 1);   // clear rx buffer
            NVIC_EnableIRQ(UART4_IRQn);     // enable UART interrupt after processing new torque command
            torqueInput.printf("New torque command: %f, dt: %d, motor speed: %f\n", uartPacket.value, executionRate_ms, motorSpeed);
            torqueCommand = uartPacket.value;
            flags.torqueCommandAvailable = false;
        }
        
        if(flags.motorEnabled && (motorSpeed < MOTOR_SPEED_THRESHOLD_RAD_S))
        {
            t.start();
            motorEnable = 1;   // 3.3V power source for gate driver IC
            
            currentSenseRaw = ABS(currentSenseExternal.read() - CURRENT_SENSE_OFFSET) * CURRENT_SENSE_SCALING_FACTOR; // current sense in amperes
            currentSenseRaw = SATURATE(currentSenseRaw, CURRENT_SENSE_LOWER_BOUND, CURRENT_SENSE_UPPER_BOUND);
            LOW_PASS_FILTER(currentSenseLPF, currentSenseRaw, executionRate_ms, CURRENT_LPF_CUTOFF_FREQ_HZ); // apply low pass filter

            torqueFeedback = MOTOR_CONSTANT_KT * currentSenseLPF;   // calculate output torque (tau = Kt * i)
            torqueError = ABS(torqueCommand) - torqueFeedback;   // compute error signal
            torqueErrorIntegral += torqueError; // compute integral of error signal

            dutyCycle = (KP * torqueError) + (KI * torqueErrorIntegral);   // PI controller
            dutyCycle = SATURATE(dutyCycle, DUTY_CYCLE_LOWER_BOUND, DUTY_CYCLE_UPPER_BOUND);  // saturate duty cycle

            inA = torqueCommand < 0 ? 0 : 1;    // set direction
            inB = !inA;
            motorPWM.write(dutyCycle);    // set duty cycle

            dt = encoder.period();                  // find time difference between two encoder pulses
            motorSpeed = (dt == 0.0) ? 0.0 : (ROTATION_PER_PULSE / dt); // don't divide by zero
            
            executionRate_ms = t.read_us();
            t.reset();
        }
        else
        {
            motorEnable = 0;    // disable 3.3V source to motor driver
        }
        
        
    }

    return 0; 
}