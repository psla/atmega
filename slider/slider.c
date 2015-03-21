#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "../lib/common.h"

// TODO: Figure story for unit tests. cppUnit? uCNit? 

#define BUTTON_PIN PB0
#define LED_PIN PB1
#define LED_DDB DDB1

/// Switch on the left and right. High indicates that slider is on this side.
/// This means, that platform is in the middle if both are "LOW"
/// (this way we can preserve energy - most of the time slider will be
/// driving from left to right, so having current flowing (even small current) is excessive)
#define LEFT_SWITCH PB2
#define RIGHT_SWITCH PB3

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
  uint8_t remaining_steps;
} slider_state_t;

slider_state_t slider_state;

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
      slider_state.direction = DIRECTION_RIGHT;
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
