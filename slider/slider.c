#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "../lib/common.h"
#include "states.h"

// TODO: Figure story for unit tests. cppUnit? uCNit? 

#define BUTTON1_PIN PB0
#define BUTTON2_PIN PB1
#define CAMERA_PIN PB2
#define CAMERA_DDB DDB2

/// Switch on the left and right. High indicates that slider is on this side.
/// This means, that platform is in the middle if both are "LOW"
/// (this way we can preserve energy - most of the time slider will be
/// driving from left to right, so having current flowing (even small current) is excessive)
#define LEFT_SWITCH PB3
#define RIGHT_SWITCH PB4

// Fill this in. This is number of motor rotations per entire slide from side to side
// This way we will know how many moves per picture to take
// To determine this, run "motor test" and it will return (on screen) a number to put in here
// TODO: persist data on arduino eprom
#define ROTATIONS_PER_SLIDER 1000

slider_state_t slider_state = {0};
programming_state_t programming_state = {0};

// this is a global state of the slider.
// it can be STATE_PROGRAMMING
// or STATE_SLIDING
uint8_t state = STATE_PROGRAMMING;

/// Drive as long as you can in direction
/// until you reach (any) side
void drive(uint8_t direction)
{
    if(direction) {
      while(!IS_SET(PORTB, RIGHT_SWITCH))  {
        // move right
      }
    } else {
      while(!IS_SET(PORTB, LEFT_SWITCH)) {
        // move left
      }
    }

    if(direction == DIRECTION_LEFT) {
      slider_state.direction = DIRECTION_RIGHT;
    } else {
      slider_state.direction = DIRECTION_LEFT;
    }
}

/// Performs a debounced read of high
/// this means that if it ever sees 0, it will return 0
/// but to see 1, it will need to see 1 few times
/// This method is actively blocking
uint8_t debounce_read(uint8_t port, uint8_t pin) {
  uint8_t retry;
  for(retry = 0; retry < 8; retry++) {
    if(IS_SET(port, pin) == 0) {
      return 0;
    }
  }

  return 1;
}
/// Verifies the platform position
/// if platform is on the side, update a direction for the next sliding direction
/// and return 1
/// otherwise do nothing and return 0
uint8_t update_direction_based_on_platform_position()
{
  if(IS_SET(PORTB, LEFT_SWITCH)) {
    slider_state.direction = DIRECTION_RIGHT;
    return 1;
  }

  if(IS_SET(PORTB, RIGHT_SWITCH)) {
    slider_state.direction = DIRECTION_LEFT;
    return 1;
  }

  return 0;
}

void handle_programming() {
  // TODO: double buttons clicked - consider doing calibration
  // TODO: calculate number of pictures and exposure time and see if you can move fast enough between points
  // (for instance, 1 second for 10 steps might be enough, but 1 seconds for 100 steps won't)
  // to start with, ensure there is at least 2 seconds interval to move platform

  switch(programming_state.state) {
    case PROGRAMMING_STATE_TIME:
      if(debounce_read(PORTB, BUTTON2_PIN)) {
        // maximum 600 minutes, minimum 10
        programming_state.total_time_in_minutes = programming_state.total_time_in_minutes + 10;
        if(programming_state.total_time_in_minutes > 600) {
          programming_state.total_time_in_minutes = 10;
        }

        // read for the button to go up
        // alternatively, support hold in the future (60 clicks to loop through right now..)
        // or use potentiometer
        while(debounce_read(PORTB, BUTTON2_PIN) != 0) ;
      }
      if(debounce_read(PORTB, BUTTON1_PIN)) {
        programming_state.state = PROGRAMMING_STATE_EXPOSURE_TIME;
        while(debounce_read(PORTB, BUTTON1_PIN) != 0) ;
      }
      break;

    case PROGRAMMING_STATE_EXPOSURE_TIME:
      if(debounce_read(PORTB, BUTTON2_PIN)) {
        if(programming_state.exposure_time_in_tens_of_second < 10)
          programming_state.exposure_time_in_tens_of_second += 2;
        else
          programming_state.exposure_time_in_tens_of_second += 10;

        if(programming_state.exposure_time_in_tens_of_second > 300)
          programming_state.exposure_time_in_tens_of_second = 0;

        // while for the button to go up
        while(debounce_read(PORTB, BUTTON2_PIN) != 0) ;
      }

      if(debounce_read(PORTB, BUTTON1_PIN)) {
        programming_state.state = PROGRAMMING_STATE_PICTURES;
        while(debounce_read(PORTB, BUTTON1_PIN) != 0) ;
      }

      break;
    case PROGRAMMING_STATE_PICTURES:
      if(debounce_read(PORTB, BUTTON2_PIN)) {
        programming_state.total_number_of_pictures += 50;
        if(programming_state.total_number_of_pictures > 1000) {
          programming_state.total_number_of_pictures = 50;
        }

        while(debounce_read(PORTB, BUTTON2_PIN) != 0) ;
      }

      if(debounce_read(PORTB, BUTTON1_PIN)) {
        programming_state.state = PROGRAMMING_STATE_DIRECTION;
        while(debounce_read(PORTB, BUTTON1_PIN) != 0) ;
      }

      break;
   case PROGRAMMING_STATE_DIRECTION:
      // change YES/NO answer
      if(debounce_read(PORTB, BUTTON1_PIN)) {
        programming_state.yes_no ^= 1;
        while(debounce_read(PORTB, BUTTON2_PIN) != 0) ;
      }

      // "continue" button pressed, see if the answer is yes or no
      if(debounce_read(PORTB, BUTTON1_PIN)) {
        if(programming_state.yes_no == 0) {
          // the answer is no, go to the next question
          programming_state.state = PROGRAMMING_STATE_START;
        } else {
          // the answer is "yes", change the direction (and move the platform), 
          // go back to question with state no
          programming_state.yes_no = 0;
          drive(slider_state.direction);
        }

        // wait for the button to go up
        while(debounce_read(PORTB, BUTTON1_PIN) != 0) ;
      }
      break;
   case PROGRAMMING_STATE_START:
      // change YES/NO answer
      if(debounce_read(PORTB, BUTTON1_PIN)) {
        programming_state.yes_no ^= 1;
        while(debounce_read(PORTB, BUTTON2_PIN) != 0) ;
      }

      if(debounce_read(PORTB, BUTTON1_PIN)) {
        if(programming_state.yes_no == 0) {
          // the answer is no, go to the first question
          programming_state.state = PROGRAMMING_STATE_TIME;
        } else {
          // the answer is "yes", change the direction (and move the platform), 
          // go back to question with state no
          programming_state.yes_no = 0;
          state = STATE_SLIDING;
        }

        // wait for the button to go up
        while(debounce_read(PORTB, BUTTON1_PIN) != 0) ;
      }
      break;
  }
}

void take_picture() {
    uint8_t counter = programming_state.exposure_time_in_tens_of_second;

    SET_BIT(PORTB, CAMERA_PIN);

    for(; counter > 0; --counter) {
      _delay_ms(100);
    }

    CLEAR_BIT(PORTB, CAMERA_PIN);
}

void handle_sliding() {
  // TODO: one of the buttons should be interrupt button that would stop sliding
  uint8_t done = 0;
  while(done == 0) {

    take_picture();

    // move to new position
    // but also count time (as the slider moves, it is part of the interval
    // between pictures)


    // otherwise go and take pictures!!
    done = 
      (slider_state.direction == DIRECTION_RIGHT && IS_SET(PORTB, RIGHT_SWITCH))
      || (slider_state.direction == DIRECTION_LEFT && IS_SET(PORTB, LEFT_SWITCH));
  }

  state = STATE_PROGRAMMING;
}

int
main (void)
{
    DDRB |= _BV(CAMERA_DDB);

    // see position of the platform and if not on the side, go to the side
    // this operation is synchronous which means that it blocks UI / screen while the motor is moving
    if(!update_direction_based_on_platform_position()) {
      drive(DIRECTION_LEFT);
    }


    while(1)
    {
      // we have just started, go to programming mode
      switch(state) {
        case STATE_SLIDING:
          handle_sliding();
          break;
        case STATE_PROGRAMMING:
          handle_programming();
          break;
      }
    }
}
