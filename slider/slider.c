#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "../lib/common.h"

// TODO: Figure story for unit tests. cppUnit? uCNit? 

#define BUTTON1_PIN PB0
#define BUTTON2_PIN PB1
#define LED_PIN PB2
#define LED_DDB DDB2

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
#define DIRECTION_RIGHT 1
#define DIRECTION_LEFT 0

#define STATE_SLIDING 0
#define STATE_PROGRAMMING 1

typedef struct slider_state_t {
  /// direction is 1 when slider is moving right
  /// and 0, when slider is moving left
  /// it should always reflect a desired direction of the next step
  /// (as opposed to the direction of the last step that took place)
  /// this means that at the end of slide, you can switch it to opposite
  uint8_t direction;

  /// Number of rotations to make per step.
  /// In other words, number of stepper motor steps to take between two pictures
  /// This must be positive number
  uint8_t speed;

  /// Contains number of remaining pictures to take
  /// This is also an estimate of remaining steps till the other end of the slider
  /// (note that this might not be accurate, you might finish too early
  /// or too late, depending on quality of the stepper motor)
  /// Code and hardware will prevent 'too late' stops.
  ///
  /// Remaining steps is initially set to ROTATIONS_PER_SLIDER / speed
  /// as long as speed is constant
  /// TODO: in future we might want to consider variable speed of the motor
  ///
  /// This number can be used to trigger buzzer (when it reaches zero)
  /// But you can continue going forward and taking pictures until you reach the other end of the slider
  uint16_t remaining_steps;
} slider_state_t;

/// Setting time of the slide
#define PROGRAMMING_STATE_TIME 0

/// Setting how long exposure should be
/// If using mirror lockup, it should at least be 2.5 seconds
/// Reasonable range from 0.5 - 30 second (though thirty means a long time :))
/// In some situations, using short stops is fine too.
#define PROGRAMMING_STATE_EXPOSURE_TIME 1

/// (or should I rather set time - but one defines another)
#define PROGRAMMING_STATE_PICTURES 2

/// Setting direction of the slide, "Change direction: YES/NO"
/// if no, go to next step, if yes, change direction and ask again
#define PROGRAMMING_STATE_DIRECTION 3

/// YES/NO "start timelapse", if no, go back to PROGRAMMING_STATE_TIME
#define PROGRAMMING_STATE_START 4

typedef struct programming_state_t {
  /// Defied in PROGRAMMING_STATE_...
  /// state of programming
  uint8_t state;

  /// how many pictures to take
  uint16_t total_number_of_pictures;

  /// how long a total timelapse time should be
  uint16_t total_time_in_minutes;

  /// what's the exposure time in tens of secons (how long platform should be stopped)
  /// 10 means 1 second, 250 means 25 seconds, 5 means 500 ms (0.5 seconds)
  uint8_t exposure_time_in_tens_of_second;

  /// current state of yes/no answer;
  /// 1 = yes
  /// 0 = no
  uint8_t yes_no;
} programming_state_t;

slider_state_t slider_state = {0};
programming_state_t programming_state = {0};

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
        programming_state.total_time_in_minutes = (programming_state.total_time_in_minutes + 10) % 600;
        if(programming_state.total_time_in_minutes == 0) {
          programming_state.total_time_in_minutes += 10;
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
          programming_state.exposure_time_in_tens_of_second += 0.2;
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
  }
}

void handle_sliding() {
}

int
main (void)
{
    DDRB |= _BV(LED_DDB);

    // see position of the platform and if not on the side, go to the side
    // this operation is synchronous which means that it blocks UI / screen while the motor is moving
    if(!update_direction_based_on_platform_position()) {
      drive(DIRECTION_LEFT);
    }

    uint8_t state = STATE_PROGRAMMING;

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
