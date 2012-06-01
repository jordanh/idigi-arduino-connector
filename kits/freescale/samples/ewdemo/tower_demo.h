/*
* Copyright (c) 2012 Digi International Inc.,
* All rights not expressly granted are reserved.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/.
*
* Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
* =======================================================================
*/
#ifndef _tower_demo_h_
#define _tower_demo_h_

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include "DemoIO.h"

extern int idigi_malloc_failures, idigi_network_receive_failures, idigi_network_send_failures;
extern int idigi_connect_to_idigi_successes, idigi_connect_to_idigi_failures;

#define PUT_REQUEST_BUFFER_SIZE 512
#define DEVICE_REQUEST_BUFFER_SIZE 512

#define DEMO_DATA 0x0007E000
#define MAC_DATA1 0x0007E004
#define MAC_DATA2 0x0007E008

#define NUM_RUNNING_TASKS 9

#define PULSE_GENERATOR_VALUE_MIN 0
#define PULSE_GENERATOR_VALUE_MAX 100

#define FREESCALE_DEMO_CONTROL_ACCEL_FILE_FORMAT "%d,%d,%d"
#define FREESCALE_DEMO_CONTROL_AD_FILE_FORMAT "0,5,%d.%d"
#define FREESCALE_DEMO_CONTROL_POT_FILE_FORMAT "0,5,%d.%d"
#define FREESCALE_DEMO_CONTROL_BUTTON_FILE_FORMAT "%s"
#define FREESCALE_DEMO_CONTROL_TEMP_FILE_FORMAT "0,100,%d"

/* Sensor Struct */
typedef struct
{
  int_8 mma7660_x;
  int_8 mma7660_y;
  int_8 mma7660_z;
  uint_8 mma7660_status;
  int_8 pot_int;
  int_8 pot_dec;
  float pulse_gen;
} SENSOR_DATA;

/* Mode enum */
typedef volatile enum {
   TOUCH=0,
   TILT,
   GAME,
   MAX_MODES
} DEMO_MODE;

/* Global Variables */
extern SENSOR_DATA Sensor;
extern DEMO_MODE mode;
extern LWSEM_STRUCT touch_sem;
extern char print_accel;
extern unsigned char current_score;
extern unsigned char high_score;

/* Functions */
void write_demo_data();
void init_cpu_usage(void);

#endif
