#ifndef __DemoIO_h__
#define __DemoIO_h__

#define ON TRUE
#define OFF FALSE

extern LWGPIO_STRUCT led1, led2, led3, led4, btn1, btn2;

typedef enum {
   LED1=0,
   LED2,
   LED3,
   LED4,
   MAX_OUTPUTS
} Output_t;

typedef enum {
   SW1_INPUT=0,
   SW2_INPUT,
   MAX_INPUTS
} Input_t;

static FILE_PTR input_port=NULL, output_port=NULL;

boolean InitializeIO(void);
boolean GetInput(LWGPIO_STRUCT);
boolean GetOutput(Output_t);
void SetOutput(LWGPIO_STRUCT,boolean);
void TiltOutput(char signal);
void OutputAllOn();
void OutputAllOff();
void ToggleAllOutput(int toggles);

#define PULSE_GEN_GPIO (GPIO_PORT_C | GPIO_PIN18)
#define BSP_PULSE_GEN_MUX_GPIO (LWGPIO_MUX_C18_GPIO)

#endif
