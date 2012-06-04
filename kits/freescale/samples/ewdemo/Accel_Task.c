/**************************************************************************
*
* FileName: Accel_Task.c
*
* Comments: Uses I2C driver to initialize and read MMA7660 Accelerometer on the
*            the tower board
*
*
***************************************************************************/

#include "tower_demo.h"
#include <i2c.h>

#ifdef TWR_K53N512
#if !BSPCFG_ENABLE_I2C1
#error This application requires BSPCFG_ENABLE_I2C1 defined non-zero in user_config.h. Please recompile libraries with this option.
#endif
#else
#if !BSPCFG_ENABLE_I2C0
#error This application requires BSPCFG_ENABLE_I2C0 defined non-zero in user_config.h. Please recompile libraries with this option.
#endif
#endif

/* I2C Address for MMA7660 */
#define I2C_ACCELEROMETER_ADDRESS 0x4C
#define I2C_DATA_SIZE 5

void InitializeMMA7660();
void InitializeI2C();

void write_I2C(int i2c_device, uchar reg, uchar value);
void read_I2C(int i2c_device, int sensor, int length);
void print_sensor_data();
signed char convert_accel_data(signed char input);
void print_accelerometer_data();


void calibrate();
void update_accel_calibration();

/* Global Variables */
extern SENSOR_DATA Sensor;
FILE_PTR fd;
uchar recv_buffer[I2C_DATA_SIZE];
signed char x_offset,y_offset,z_offset;
char print_accel;
signed char xvalues[8];
signed char yvalues[8];
signed char zvalues[8];
signed long xsum;
signed long ysum;
signed long zsum;
signed char xread;
signed char yread;
signed char zread;
unsigned char current;

_enet_address  mac_address;

unsigned int *DemoData = (unsigned int *)DEMO_DATA;
unsigned long *MacData1 = (unsigned long *)MAC_DATA1;
unsigned long *MacData2 = (unsigned long *)MAC_DATA2;

SENSOR_DATA CurrentSensor;

// #define WRITE_MAC_ADDRESS

/*
 * Task for reading the MMA7660 accelerometer data
 */
void Accel_Task(uint_32 data)
{
  printf("Starting Accelerometer Task\n");

  /* Initialize global variables */
  print_accel=0;
  x_offset=0;
  y_offset=0;
  z_offset=0;
    
  for(current = 0; current < 8; current++) {
  	  xvalues[current] = yvalues[current] = zvalues[current] = 0;
  }
  xsum = ysum = zsum = 0;
  current = 0;

  //Initialize I2C driver
  InitializeI2C();

  //Configure MMA7660
  InitializeMMA7660();

  //Read accelerometer offset from memory
  update_accel_calibration();
  
#ifdef WRITE_MAC_ADDRESS
  calibrate();
#endif

  /*
   * Read the accelerometer data and store it in a global structure
   */
  while (TRUE)
  {
    /* If both push buttons held down, perform offset calibration */

	if(GetInput(btn1) && GetInput(btn2))
    {
      calibrate();
    }

    //Read first five registers on MMA7660
    read_I2C(I2C_ACCELEROMETER_ADDRESS,0x00,5);
    /* Parse out data */
   	
    xread = convert_accel_data((int_8)recv_buffer[0])+x_offset;
    yread = convert_accel_data((int_8)recv_buffer[1])+y_offset;
    zread = convert_accel_data((int_8)recv_buffer[2])+z_offset;
    Sensor.mma7660_status=recv_buffer[3];
    
    xsum = xsum - xvalues[current] + xread;
    ysum = ysum - yvalues[current] + yread;
    zsum = zsum - zvalues[current] + zread;
    	
    xvalues[current] = xread;
    yvalues[current] = yread;
    zvalues[current] = zread;
    	
    Sensor.mma7660_x = xsum / 8;
    Sensor.mma7660_y = ysum / 8;
    Sensor.mma7660_z = zsum / 8;
    	
    current = (current + 1) % 8;
    
    if(print_accel)
    {
        print_accelerometer_data();
    }

    //Time delay
     _time_delay(100);
   }
}

void InitializeI2C()
{
  /* Open the I2C driver, and assign a I2C device handler*/
#ifndef TWR_K53N512
  fd = fopen ("i2c0:", NULL);
#else
  fd = fopen ("i2c1:", NULL);
#endif
  if (fd == NULL)
  {
    printf ("Failed to open the I2C driver!\n");
    _time_delay (200L);
    _mqx_exit (1L);
  }

  /* Set I2C into Master mode */
  ioctl (fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL);
  
#ifdef TWR_K53N512
  //PORTC_PCR10  @ address 0x4004B028
  PORTC_PCR10 &= ~PORT_PCR_MUX_MASK ; 
  PORTC_PCR10 |=  PORT_PCR_MUX(2); 
  PORTC_PCR10 |= PORT_PCR_ODE_MASK;

  //PORTC_PCR11 @ address 4004B02C
  PORTC_PCR11 &= ~PORT_PCR_MUX_MASK ; 
  PORTC_PCR11|=  PORT_PCR_MUX(2); 
  PORTC_PCR11 |= PORT_PCR_ODE_MASK;
#endif
}

/*
 * Configure MMA7660
 */
void InitializeMMA7660()
{
  /* Configure MMA7660 */
  write_I2C(I2C_ACCELEROMETER_ADDRESS,0x09,0xE0);
  write_I2C(I2C_ACCELEROMETER_ADDRESS,0x07,0x19);
}

/* Function for writing data to I2C Device */
void write_I2C(int i2c_device_address, uchar reg, uchar value)
{
  uchar data[2];

  data[0]=reg;   //Sensor register
  data[1]=value; //Byte of data to write to register

  /* Set the destination address */
  ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &i2c_device_address);

  /* Write 2 bytes of data: the desired register and then the data */
  fwrite (&data, 1, 2, fd);  //data to write, size of unit, # of bytes to write, I2C device handler
  fflush (fd);

  /* Send out stop */
  ioctl (fd, IO_IOCTL_I2C_STOP, NULL);
}

/* Function for reading data from I2C Device. Data placed in recv_buffer[] */
void read_I2C(int i2c_device_address, int sensor, int length)
{
  int n=length;

  //The starting register for the particular sensor requested
  uchar reg=sensor;

  //Set the I2C destination address
  ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &i2c_device_address);

  //Tell the I2C device which register to read data from
  fwrite (&reg, 1, 1, fd); //data to write, size of unit, # of bytes to write, I2C device handler

  //Wait for completion
  fflush (fd);

  //Do a repeated start to avoid giving up control
  ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL);

  //Set how many bytes to read
  ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &n);

  //Read n bytes of data and put it into the recv_buffer
  fread (&recv_buffer, 1, n, fd); //buffer to read data into, size of unit, # of bytes to read, I2C device handler

  //Wait for completion
  fflush (fd);

  //Send out stop
  ioctl (fd, IO_IOCTL_I2C_STOP, NULL);
}

/*
 * Function for printing out accelerometer data to terminal
 */
void print_accelerometer_data()
{
	if (CurrentSensor.mma7660_x != Sensor.mma7660_x ||
		CurrentSensor.mma7660_y != Sensor.mma7660_y ||
		CurrentSensor.mma7660_z != Sensor.mma7660_z)
    {
    	CurrentSensor.mma7660_x = Sensor.mma7660_x;
    	CurrentSensor.mma7660_y = Sensor.mma7660_y;
    	CurrentSensor.mma7660_z = Sensor.mma7660_z;
    	printf("%d   %d   %d\n",Sensor.mma7660_x,Sensor.mma7660_y,Sensor.mma7660_z);	
    }
}

/*
 * Convert 6-bit result into an 8-bit signed char
 */
signed char convert_accel_data(signed char input)
{
      input &=~0x40; //Turn off alert bit
      if(input & 0x20)    // update 6-bit signed format to 8-bit signed format
	input |= 0xE0;
    return (signed char) input;
}

/*
 * Calibrate accelerometer offset
 */
void calibrate()
{
  int i;
  int x_avg=0;
  int y_avg=0;
  int z_avg=0;

  /* Calculate average from multiple readings */
  for(i=0;i<64;i++)
  {
    read_I2C(I2C_ACCELEROMETER_ADDRESS,0x00,3);
    x_avg+=convert_accel_data((int_8)recv_buffer[0]);
    y_avg+=convert_accel_data((int_8)recv_buffer[1]);
    z_avg+=convert_accel_data((int_8)recv_buffer[2]);
  }
  x_avg=(int)x_avg/64;
  y_avg=(int)y_avg/64;
  z_avg=(int)z_avg/64;

  /* Calculate offset */
  x_offset=(signed char)x_avg*-1;
  y_offset=(signed char)y_avg*-1;
  z_offset=(signed char)(z_avg-21)*-1;

 // printf("Offset is x=%d and y=%d and z=%d\n",x_offset,y_offset,z_offset);

  /* Write offset to flash memory */
  write_demo_data();

  /* Read accelerometer offset from memory */
  update_accel_calibration();
}

/*
 * Write offset data and high score to flash memory
 *
 * Will first erase sector, then perform write
 */
void write_demo_data()
{
  unsigned int destination=DEMO_DATA;

  //Turn off interrupts so don't get interrupted
  asm("cpsid.n i");

  //Make sure Flash commmand ok
  while((FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK )==0)
  {}	
   		
  while((FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK )==0)
  {}	

  //Write to FSTAT
  FTFL_FSTAT=FTFL_FSTAT_RDCOLERR_MASK| FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_FPVIOL_MASK;

  //Configure for sector erase
  FTFL_FCCOB0=   (uint_8)0x09; //FTFL_ERASE_SECTOR
  FTFL_FCCOB1 = (uint_8)(destination >> 16);
  FTFL_FCCOB2 = (uint_8)((destination >> 8) & 0xFF);
  FTFL_FCCOB3 = (uint_8)(destination & 0xFF);
  while((FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK )==0)
  {}

  //Execute command
  FTFL_FSTAT|= FTFL_FSTAT_CCIF_MASK;

  //Wait for completion
  while((FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK )==0)
  {}

  //Setup for write command
  while((FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK )==0)
  {}
  FTFL_FSTAT=FTFL_FSTAT_RDCOLERR_MASK| FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_FPVIOL_MASK;
  FTFL_FCCOB0=   (uint_8)0x06; //FTFL_PROGRAM_LONGWORD
  FTFL_FCCOB1 = (uint_8)(destination >> 16);
  FTFL_FCCOB2 = (uint_8)((destination >> 8) & 0xFF);
  FTFL_FCCOB3 = (uint_8)(destination & 0xFF);
  FTFL_FCCOB4 =(int_8)high_score;
  FTFL_FCCOB5=(int_8)x_offset;
  FTFL_FCCOB6=(int_8)y_offset;
  FTFL_FCCOB7=(int_8)z_offset;

  //Execute command
  FTFL_FSTAT|= FTFL_FSTAT_CCIF_MASK;

  //Wait for completion
  while((FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK )==0)
  {}

  //Enable interrupts again
  asm("cpsie.n i  ");
}

void update_mac_address(_enet_address  mac_address)
{
  unsigned long stored_data=*MacData1;

  mac_address[0] = (stored_data&0xFF000000)>>32;
  mac_address[1] = (stored_data&0x00FF0000)>>16;
  mac_address[2] = (stored_data&0x0000FF00)>>8;
  mac_address[3] = (stored_data&0x000000FF);
  
  stored_data=*MacData2;

  mac_address[4] = (stored_data&0xFF000000)>>32;
  mac_address[5] = (stored_data&0x00FF0000)>>16;
}

/*
 * Read accelerometer offset from memory
 */
void update_accel_calibration()
{
  int stored_data=*DemoData;

  //If no valid data, return
  if(stored_data == 0xFFFFFFFF)
  {
    x_offset=0;
    y_offset=0;
    z_offset=0;
    return;
  }

  x_offset=(stored_data&0x00FF0000)>>16;
  y_offset=(stored_data&0x0000FF00)>>8;
  z_offset=(stored_data&0x000000FF);

 // printf("Offset in memory is x=%d and y=%d and z=%d\n",x_offset,y_offset,z_offset);
}
