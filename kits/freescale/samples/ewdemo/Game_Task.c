#include "tower_demo.h"
#include "DemoIO.h"
#include "TSI.h"
#include <event.h>

#define LOG_LENGTH  30
char LastKeyPressed();
char KeyPressed();
void AddTouch(char key);
void WaitForTouch();
void WaitForRelease();
void create_sequence();
void GetHighScore();

volatile unsigned char sequence[LOG_LENGTH];
volatile unsigned char keylog[LOG_LENGTH];
volatile unsigned char keypressed;
char length;
char player_error=0;
unsigned char current_score;
unsigned char high_score;


/*
 * Task for running the memory game
 *
 * TODO: Add support for keeping track of high score
 */
void Game_Task(uint_32 data)
{
#if 0
  int i;

  /* Initialize variables */
  char length=1;
  char lastkey;

  printf("Starting Game Task\n");

  GetHighScore();

  while(1)
  {
    //If mode is now touch, turn on all LED's to start with
    if(mode==TOUCH)
    {
      OutputAllOn();
    }

    //Wait for Game mode to be entered
    while(mode!=GAME)
    {
      _time_delay(500);
    }

    //Reset variables
    length=1;
    current_score=0;
    player_error=0;

    for(i=0;i<LOG_LENGTH;i++)
    {
      keylog[i]=0;
    }

    //Create random sequence
    create_sequence();

    //Toggle LED's to show entered game mode
    OutputAllOff();
    ToggleAllOutput(2);
    _time_delay(500);

    //While still in game mode and the player has not made a mistake...
    while(!player_error && mode==GAME)
    {
      /* Send out sequence to LED's */
      for(i=0;i<length;i++)
      {
        SetOutput((Output_t)sequence[i], ON);
        _time_delay(250);
        SetOutput((Output_t)sequence[i], OFF);
        _time_delay(200);
      }

      /* Now get user input */
      for(i=0;i<length;i++)
      {
        //Wait for TSI touch
        WaitForTouch();

        //Get the key pressed
        lastkey=LastKeyPressed();

        //Turn on that LED
        SetOutput((Output_t)lastkey, ON);

        //Wait for release of touch
        WaitForRelease();

        //Leave on LED slightly longer so user can see which LED they hit
        _time_delay(150);
        SetOutput((Output_t)lastkey, OFF);

        //If they hit the wrong key or no longer in Game mode, set player_error=1
        if(lastkey!=sequence[i] || mode!=GAME)
        {
          player_error=1;
          break;
        }
        else
        {
          //Updated score
          current_score=length;

          //Update high score if appropriate
          if(high_score<current_score)
          {
            high_score=current_score;
            write_demo_data();
          }
        }
      }

      //If the player was correct, toggle LED's
      if(!player_error)
      {
        ToggleAllOutput(2);
        _time_delay(200);
      }

      //Increase the number of LED lights to remember
      length++;

      //If reached maximum length, toggle 10 times and reset
      if(length==LOG_LENGTH)
      {
        ToggleAllOutput(10);
        length=1;
      }
    }

    //If player error occured and we are not now in touch mode, then toggle LED's 5 times
    if(mode!=TOUCH)
    {
      ToggleAllOutput(5);
    }
  }//End WHILE
#endif
}

/*
 * Wait for touch pad to be pressed or
 *  for game mode to be exited by push button
 */
void WaitForTouch()
{
  //Reset semaphore
  touch_sem.VALUE=0;

  //Wait for semaphore to post with a timeout of 1000 ticks (5 seconds)
  //Semaphore is posted if touch pad is touched, or demo put into Touch mode
  if(_lwsem_wait_ticks(&touch_sem,1000)!= MQX_OK)
  {
    printf("Waited too long for key press\n");
    player_error=1;
  }
  //If semaphore was posted because it is no longer game mode, also set player_error=1
  else if(mode!=GAME)
  {
    player_error=1;
  }
}

/*
 * Wait for touch pad to be un-pressed
 */
void WaitForRelease()
{
  while(keypressed!=0)
  {}
}

/*******************************************************************/
/*!
 * Return value of last key pressed
 * @return Number of the last button pressed
 */
char LastKeyPressed()
{
  return (char)keylog[0] & 0x0000000F;
}

/*******************************************************************/
/*!
 * Return 1 if key is currently being pressed. 0 if no key is being pressed
 */
char KeyPressed()
{
  return keypressed & 0x0000000F;
}

/*******************************************************************/
/*!
 *  Add Key Press to Key History
 *  @param key is the key to add to queue
 */
void AddTouch(char key)
{
  unsigned char i=LOG_LENGTH-1;

  /* Shift queue over by 1 */
  while(i>0)
  {
    keylog[i]=keylog[i-1];
    i--;
  }
  keylog[0]=key;
}

/*
 * Use RNG module to create a random sequence to light up
 */
void create_sequence()
{
  int i, value;

  //Turn on RNG module and configure RNG module
  SIM_SCGC3|=SIM_SCGC3_RNGB_MASK;
  RNG_CR|=RNG_CR_MASKDONE_MASK|RNG_CR_MASKERR_MASK|RNG_CR_AR_MASK;

  //Wait for random data to be generated
  while((RNG_SR & RNG_SR_SDN_MASK)==0)
  {}

  //Assigned random data to entire sequence
  for(i=0;i<LOG_LENGTH;i++)
  {
    //Wait for RNG FIFO to be full
    while((RNG_SR & RNG_SR_FIFO_LVL(0xF))==0)
    {}
    value=(RNG_OUT%4); //Get value
    sequence[i]=value;
  }
}

/*
 * Read flash memory to read off the previous high score
 */
void GetHighScore()
{
  unsigned int *DemoData = (unsigned int *)DEMO_DATA;
  int stored_data=*DemoData;

  //If no valid data, return
  if(stored_data == 0xFFFFFFFF)
  {
    high_score=0;
    return;
  }
  high_score=(stored_data&0xFF000000)>>24;

  /* Make sure high score is not higher than maximum possible high score */
  if(high_score>LOG_LENGTH)
    high_score=0;
}