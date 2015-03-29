#include <avr/io.h>
#include <util/delay.h>
#include "../lib/lcd.h"
#include "../lib/common.h"
#include "../lib/millis.h"
#include "states.h"
#include "stepper.h"

// TODO: Figure story for unit tests. cppUnit? uCNit? 
#define BUTTON1_PORT PORTB
#define BUTTON1_READ PINB
#define BUTTON1_PIN PB0
#define BUTTON2_PIN PB2
#define CAMERA_PIN PB3
#define CAMERA_DDB DDB2

// when button is pressed it will be either HIGH or LOW
// currently I am using built-in pull-up resistor and connected button to GND
// so low-level indicates button is pressed
#define BUTTON_PRESSED_LEVEL 0
#define BUTTON_NOT_PRESSED_LEVEL 1

/// Switch on the left and right. High indicates that slider is on this side.
/// This means, that platform is in the middle if both are "LOW"
/// (this way we can preserve energy - most of the time slider will be
/// driving from left to right, so having current flowing (even small current) is excessive)
#define LEFT_SWITCH PB3
#define RIGHT_SWITCH PB4

// Fill this in. This is number of motor rotations per entire slide from side to side
// This way we will know how many moves per picture to take
// To determine this, run "motor test" and it will return (on screen) a number to put in here
// TODO: persist data on atmega eprom
#define STEPS_PER_SLIDER 10000

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
    if(IS_SET(port, pin) == BUTTON_NOT_PRESSED_LEVEL) {
      return BUTTON_NOT_PRESSED_LEVEL;
    }
  }

  return BUTTON_PRESSED_LEVEL;
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

/// Prints a non-negative integer in current position of the screen
/// It will contain 4 digits, and will start with leading zeroes.
void print_uint16(uint16_t number) {
  // maximum supported number has 4 digits
  // put a divider as appropriate power of 10 to represent the longest supported number
  uint16_t divider = 1000L;
  while(divider != 0) {
    lcd_putc((number / divider) % 10 + '0');
    divider /= 10;
  }
}

/// Prints a non-negative integer in current position of the screen,
/// It will contain 3 digits
void print_uint8(uint8_t number) {
  // maximum supported number has 4 digits
  // put a divider as appropriate power of 10 to represent the longest supported number
  uint16_t divider = 100L;
  while(divider != 0) {
    lcd_putc((number / divider) % 10 + '0');
    divider /= 10;
  }
}

/// prints time of the slide
void print_total_time() {
  lcd_clrscr();

  lcd_puts("Sliding time:");
  lcd_set_cursor(1, 0);
  print_uint16(programming_state.total_time_in_minutes);
  lcd_puts(" [min]");
  // TODO: consider formatting as x h xx min (or x:xx)
}

void print_sliding_state() {
  lcd_clrscr();
  lcd_puts("Remaining picts:");
  lcd_set_cursor(1, 0);
  print_uint16(slider_state.remaining_steps);
  // TODO: maybe print remaining time?
}

void print_exposure_time() {
  lcd_clrscr();
  lcd_puts("Exposure time:");
  lcd_set_cursor(1, 0);
  print_uint8(programming_state.exposure_time_in_tens_of_second / 10);
  lcd_putc('.');
  lcd_putc(programming_state.exposure_time_in_tens_of_second % 10 + '0');

  lcd_puts(" [s]");
}

void print_pictures_count() {
  lcd_clrscr();

  lcd_puts("Pictures to take:");
  lcd_set_cursor(1, 0);
  print_uint16(programming_state.total_number_of_pictures);
}

void print_change_direction() {
	lcd_clrscr();
	lcd_puts("Change direction");
	lcd_set_cursor(1, 0);
	lcd_putc(programming_state.yes_no + '0');
}

void print_start_sliding() {
	lcd_clrscr();
	lcd_puts("Start sliding");
	lcd_set_cursor(1, 0);
	lcd_putc(programming_state.yes_no + '0');
}

void handle_programming() {
  // TODO: double buttons clicked - consider doing calibration
  // TODO: calculate number of pictures and exposure time and see if you can move fast enough between points
  // (for instance, 1 second for 10 steps might be enough, but 1 seconds for 100 steps won't)
  // to start with, ensure there is at least 2 seconds interval to move platform
  switch(programming_state.state) {
    case PROGRAMMING_STATE_TIME:
      if(debounce_read(BUTTON1_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
        // maximum 600 minutes, minimum 10
        programming_state.total_time_in_minutes = programming_state.total_time_in_minutes + 10;
        if(programming_state.total_time_in_minutes > 600) {
          programming_state.total_time_in_minutes = 10;
        }

        print_total_time();

        // read for the button to go up
        // alternatively, support hold in the future (60 clicks to loop through right now..)
        // or use potentiometer
        while(debounce_read(BUTTON1_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }
	  
      if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
        programming_state.state = PROGRAMMING_STATE_EXPOSURE_TIME;
		print_exposure_time();
		
		while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }
      break;

    case PROGRAMMING_STATE_EXPOSURE_TIME:
      if(debounce_read(BUTTON1_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
        if(programming_state.exposure_time_in_tens_of_second < 10)
          programming_state.exposure_time_in_tens_of_second += 2;
        else
          programming_state.exposure_time_in_tens_of_second += 10;

        if(programming_state.exposure_time_in_tens_of_second > 300)
          programming_state.exposure_time_in_tens_of_second = 0;

        print_exposure_time();

        // while for the button to go up
        while(debounce_read(BUTTON1_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }

      if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
        programming_state.state = PROGRAMMING_STATE_PICTURES;
		print_pictures_count();
        while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }

      break;
    case PROGRAMMING_STATE_PICTURES:
      if(debounce_read(BUTTON1_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
        programming_state.total_number_of_pictures += 50;
        if(programming_state.total_number_of_pictures > 1000) {
          programming_state.total_number_of_pictures = 50;
        }

        print_pictures_count();

        while(debounce_read(BUTTON1_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }

      if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
        programming_state.state = PROGRAMMING_STATE_DIRECTION;
		print_change_direction();
        while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }

      break;
   case PROGRAMMING_STATE_DIRECTION:
      // change YES/NO answer
      if(debounce_read(BUTTON1_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
        programming_state.yes_no ^= 1;
        // TODO: instead, we have two buttons ready.
        // Just use one as a yes, and other one as a no
        print_change_direction();

        while(debounce_read(BUTTON1_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }

      // "continue" button pressed, see if the answer is yes or no
      if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
        if(programming_state.yes_no == 0) {
          // the answer is no, go to the next question
          programming_state.state = PROGRAMMING_STATE_START;
		  print_start_sliding();
        } else {
          // the answer is "yes", change the direction (and move the platform), 
          // go back to question with state no
          programming_state.yes_no = 0;
          drive(slider_state.direction);
        }

        // wait for the button to go up
        while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }
      break;
   case PROGRAMMING_STATE_START:
      // change YES/NO answer
      if(debounce_read(BUTTON1_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
        programming_state.yes_no ^= 1;

		print_start_sliding();

        while(debounce_read(BUTTON1_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL);
      }

      if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
        if(programming_state.yes_no == 0) {
          // the answer is no, go to the first question
          programming_state.state = PROGRAMMING_STATE_TIME;
		  print_total_time();
        } else {
          // the answer is "yes", change the direction (and move the platform), 
          // go back to question with state no
          programming_state.yes_no = 0;

          // set up sliding state
          slider_state.remaining_steps = programming_state.total_number_of_pictures;
          slider_state.speed = STEPS_PER_SLIDER / programming_state.total_number_of_pictures;
          state = STATE_SLIDING;
        }

        // wait for the button to go up
        while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
      }
      break;
  }
  _delay_ms(100);
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
  uint8_t repeat_wait;
  print_sliding_state();

  while(done == 0) {
    take_picture();

    // yes, this could go to negative numbers
    // well, remaining steps is an uint, so it can go to very high numbers actually
    --slider_state.remaining_steps;

    millis_reset();

    print_sliding_state();

    step(slider_state.direction, slider_state.speed);

    unsigned long passed_time = millis_get();

    // this is constant in a run (rotation of specific degree will take always
    // same amount of time)
    // we must now wait remaining time
    repeat_wait = programming_state.exposure_time_in_tens_of_second - (passed_time / 100);
    for(; repeat_wait >= 0; --repeat_wait) _delay_ms(100);

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
    // TODO: camera requires two pins (focus and shutter)
	// DDRB |= _BV(BUTTON1_PIN);
	// DDRB |= _BV(BUTTON2_PIN);
	DDRB |= _BV(PB1);
	
	// enable pull-up resistor for button
	BUTTON1_PORT |= 1 << BUTTON1_PIN;
	BUTTON1_PORT |= 1 << BUTTON2_PIN;

    millis_init();
	lcd_init();
	lcd_on();
	lcd_clear();
	lcd_return_home();

	lcd_puts("Welcome...");
	print_total_time();
	
	// see position of the platform and if not on the side, go to the side
    // this operation is synchronous which means that it blocks UI / screen while the motor is moving
    //if(!update_direction_based_on_platform_position()) {
	  // I do not have motor just yet, comment it out
      // drive(DIRECTION_LEFT);
    //}

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
