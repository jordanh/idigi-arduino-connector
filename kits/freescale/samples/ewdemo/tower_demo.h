#ifndef _tower_demo_h_
#define _tower_demo_h_
/**HEADER********************************************************************
*
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
***************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: tower_demo.h$
* $Version : 3.5.21.0$
* $Date    : Dec-2-2009$
*
* Comments:
*   The main configuration file for WEB_HVAC demo
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include "DemoIO.h"

extern int idigi_malloc_failures, idigi_network_receive_failures, idigi_network_send_failures;
extern int idigi_connect_to_idigi_successes, idigi_connect_to_idigi_failures;

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
