#ifndef __main_h_
#define __main_h_
#include <mqx.h>
#include <bsp.h>

#include <rtcs.h>
#ifdef APPLICATION_HAS_SHELL
#include <sh_rtcs.h>
#endif
#include <ipcfg.h>

#define MAIN_TASK 1
#define IDIGI_CONNECTOR_TASK 2
#define IDIGI_FLASH_TASK 3
#define ADC_TASK 4
#define ACCEL_TASK 5
#define IDIGI_LED_TASK 6
#define IDIGI_TOUCH_TASK 7
#define IDIGI_BUTTON_TASK 8
#define IDIGI_UTILITY_1 9
#define IDIGI_UTILITY_2 10
#define IDIGI_APP_TASK 11
#define IDIGI_GPIO_TASK 12
#define IDIGI_CPU_USAGE_TASK 13

#define ENET_DEVICE 0

extern _enet_address device_mac_addr;
extern void Main_task(uint_32);
extern void idigi_connector_thread(uint_32);
extern void idigi_flash_task(uint_32);
extern void Accel_Task(uint_32);
extern void ADC_Task();
extern void idigi_led_task(uint_32);
extern void idigi_touch_pad_task(uint_32);
extern void idigi_button_task(uint_32);
extern void idigi_utility_task1(uint_32);
extern void idigi_utility_task2(uint_32);
extern void idigi_app_run_task(uint_32);
extern void idigi_gpio_pulse_task(uint_32);
extern void idigi_cpu_usage(uint_32);

#endif /* __main_h_ */

