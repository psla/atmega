#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
 
#define BUTTON_PIN PB0
#define LED_PIN PB1
#define LED_DDB DDB1

// Fill this in. This is number of motor rotations per entire slide from side to side
// This way we will know how many moves per picture to take
// To determine this, run "motor test" and it will return (on screen) a number to put in here
// TODO: persist data on arduino eprom
#define ROTATIONS_PER_SLIDER 1000


typedef struct slider_state_t {
  /// direction is +1 when slider is moving right
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


/// Drive as long as you can in direction
/// until you reach (any) side
void drive(uint8_t direction)
{
    if(direction) {
      /// TODO: move right
    } else {
      /// TODO: move left
    }
}

int
main (void)
{
    DDRB |= _BV(LED_DDB);

    

    while(1)
    {
        PORTB ^= _BV(LED_PIN);
        _delay_ms(500);
    }
}
