#include <avr/io.h>
#include <util/delay.h>
#include "../lib/lcd.h"
#include "../lib/common.h"
#include "../lib/millis.h"
#include "states.h"
#include "../lib/stepper.h"
#include "slider_memory.h"

// TODO: Figure story for unit tests. cppUnit? uCNit?
#define BUTTON1_PORT PORTB
#define BUTTON1_READ PINB
#define BUTTON1_PIN  PB4

#define BUTTON2_PORT PORTD
#define BUTTON2_READ PIND
#define BUTTON2_PIN  PD7

#define CAMERA_SHUTTER_PORT PORTB
#define CAMERA_SHUTTER_DDR  DDRB
#define CAMERA_SHUTTER_PIN  PB7

#define CAMERA_FOCUS_PORT PORTB
#define CAMERA_FOCUS_DDR DDRB
#define CAMERA_FOCUS_PIN PB6

// this is the first pin out of 4 to drive motor.
// pins must be connected to the same port and must be consecutive.
#define MOTOR_FIRST_PIN PIN0
#define MOTOR_DDR DDRD

// when button is pressed it will be either HIGH or LOW
// currently I am using built-in pull-up resistor and connected button to GND
// so low-level indicates button is pressed
#define BUTTON_PRESSED_LEVEL 0
#define BUTTON_NOT_PRESSED_LEVEL 1

// LEFT or RIGHT switch is active on following level
// this means that platform is on the side of the slider
#define SWITCH_ACTIVE 0

// inactive means that platform is in the middle of slide
#define SWITCH_INACTIVE 1

/// Switch on the left and right. High indicates that slider is on this side.
/// This means, that platform is in the middle if both are "LOW"
/// (this way we can preserve energy - most of the time slider will be
/// driving from left to right, so having current flowing (even small current) is excessive)
#define LEFT_SWITCH_PORT PORTD
#define LEFT_SWITCH_READ PIND
#define LEFT_SWITCH_PIN PD4

#define RIGHT_SWITCH_PORT PORTC
#define RIGHT_SWITCH_READ PINC
#define RIGHT_SWITCH_PIN PC0

slider_state_t slider_state = {0};
programming_state_t programming_state = {0};

// this is a global state of the slider.
// it can be STATE_PROGRAMMING
// or STATE_SLIDING
uint8_t state = STATE_PROGRAMMING;

uint16_t steps_per_slider = 0;

// declarations:
uint8_t debounce_read(uint8_t port, uint8_t pin);
uint16_t drive(uint8_t direction);
void print_change_direction();
void print_uint16(uint16_t number);
void print_uint16_custom(uint16_t number, uint16_t divider);


void print_yes_no() {
	lcd_set_cursor(1, 0);
	lcd_puts("NO          YES");
}

void calibrate() {
	lcd_clrscr();
	lcd_puts("Calibrating...");
	lcd_set_cursor(1, 0);
	lcd_puts("Going left");
	drive(DIRECTION_LEFT);
	lcd_clrscr();
	lcd_puts("Calibrating");
	lcd_set_cursor(1, 0);
	lcd_puts("Going right");
	_delay_ms(2000);
	uint16_t steps = drive(DIRECTION_RIGHT);
	lcd_clrscr();
	lcd_puts("Steps: ");
	print_uint16_custom(steps, 10000L);
	print_yes_no();
	
	// read update
	for(;;)
	{
		// no:
		if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
			while(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_NOT_PRESSED_LEVEL) ;
			return;
		}
		
		// yes:
		if(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
			while(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_NOT_PRESSED_LEVEL) ;
			set_steps_per_slide(steps);
			return;
		}
	}
}

/// Drive as long as you can in direction
/// until you reach (any) side
/// reports number of steps
uint16_t drive(uint8_t direction)
{
	uint16_t steps = 0;
	if(direction) {
		while(IS_SET(RIGHT_SWITCH_READ, RIGHT_SWITCH_PIN) == SWITCH_INACTIVE)  {
			// move right
			++steps;
			onestep(direction);
			
			_delay_ms(2);
		}
		} else {
		while(IS_SET(LEFT_SWITCH_READ, LEFT_SWITCH_PIN) == SWITCH_INACTIVE) {
			// move left
			++steps;
			
			onestep(direction);
			
			_delay_ms(2);
		}
	}

	if(direction == DIRECTION_LEFT) {
		slider_state.direction = DIRECTION_RIGHT;
		} else {
		slider_state.direction = DIRECTION_LEFT;
	}
	
	return steps;
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
	if(IS_SET(LEFT_SWITCH_READ, LEFT_SWITCH_PIN) == SWITCH_ACTIVE) {
		slider_state.direction = DIRECTION_RIGHT;
		return 1;
	}

	if(IS_SET(RIGHT_SWITCH_READ, RIGHT_SWITCH_PIN) == SWITCH_ACTIVE) {
		slider_state.direction = DIRECTION_LEFT;
		return 1;
	}

	return 0;
}

/// Prints a non-negative integer in current position of the screen
/// It will contain X digits, depending on divider.
/// If divider is set to 1000L, it will contain 4 digits
/// If divider is set to 10000L, it will contain 5 digits
void print_uint16_custom(uint16_t number, uint16_t divider) {
	// maximum supported number has 4 digits
	// put a divider as appropriate power of 10 to represent the longest supported number
	while(divider != 0) {
		lcd_putc((number / divider) % 10 + '0');
		divider /= 10;
	}
}

/// Prints a non-negative integer in current position of the screen
/// It will contain 4 digits, and will start with leading zeroes.
void print_uint16(uint16_t number) {
	print_uint16_custom(number, 1000L);
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
	if(slider_state.remaining_steps >= 0) {
		lcd_puts("Remaining picts:");
		lcd_set_cursor(1, 0);
		print_uint16(slider_state.remaining_steps);
		} else {
		lcd_puts("Over pictures:");
		lcd_set_cursor(1, 0);
		print_uint16(slider_state.over_pictures);
	}
	
	// TODO: maybe print remaining time?
	// (in timer interrupt routine?)
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
	print_yes_no();
}

void print_start_sliding() {
	lcd_clrscr();
	lcd_puts("Start sliding");
	print_yes_no();
}

void print_calibration() {
	lcd_clrscr();
	lcd_puts("Calibrate? ");
	print_uint16_custom(get_steps_per_slide(), 10000L);
	print_yes_no();
}

// performs an operation associated with each state change.
// for instance, updates LCD screen
void change_programming_state(uint8_t desired_state) {
	switch (desired_state)
	{
		case PROGRAMMING_CALIBRATION:
		print_calibration();
		break;
		case PROGRAMMING_STATE_TIME:
		print_total_time();
		break;
		case PROGRAMMING_STATE_EXPOSURE_TIME:
		print_exposure_time();
		break;
		case PROGRAMMING_STATE_PICTURES:
		print_pictures_count();
		break;
		case PROGRAMMING_STATE_DIRECTION:
		print_change_direction();
		break;
		case PROGRAMMING_STATE_START:
		print_start_sliding();
		break;
	}
	
	programming_state.state = desired_state;
}

void handle_programming() {
	// TODO: double buttons clicked - consider doing calibration
	// TODO: calculate number of pictures and exposure time and see if you can move fast enough between points
	// (for instance, 1 second for 10 steps might be enough, but 1 seconds for 100 steps won't)
	// to start with, ensure there is at least 2 seconds interval to move platform
	switch(programming_state.state) {
		case PROGRAMMING_STATE_TIME:
		if(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
			// maximum 600 minutes, minimum 10
			programming_state.total_time_in_minutes = programming_state.total_time_in_minutes + 10;
			if(programming_state.total_time_in_minutes > 600) {
				programming_state.total_time_in_minutes = 10;
			}

			print_total_time();

			// read for the button to go up
			// alternatively, support hold in the future (60 clicks to loop through right now..)
			// or use potentiometer
			while(debounce_read(BUTTON2_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}
		
		if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
			change_programming_state(PROGRAMMING_STATE_EXPOSURE_TIME);
			
			while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}
		break;

		case PROGRAMMING_STATE_EXPOSURE_TIME:
		if(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
			if(programming_state.exposure_time_in_tens_of_second < 10)
			programming_state.exposure_time_in_tens_of_second += 2;
			else
			programming_state.exposure_time_in_tens_of_second += 10;

			if(programming_state.exposure_time_in_tens_of_second > 300)
			programming_state.exposure_time_in_tens_of_second = 0;

			print_exposure_time();

			// while for the button to go up
			while(debounce_read(BUTTON2_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}

		if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
			change_programming_state(PROGRAMMING_STATE_PICTURES);
			
			while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}

		break;
		case PROGRAMMING_STATE_PICTURES:
		if(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
			programming_state.total_number_of_pictures += 50;
			if(programming_state.total_number_of_pictures > 1000) {
				programming_state.total_number_of_pictures = 50;
			}

			print_pictures_count();

			while(debounce_read(BUTTON2_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}

		if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
			change_programming_state(PROGRAMMING_STATE_DIRECTION);
			
			while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}

		break;
		case PROGRAMMING_STATE_DIRECTION:
		// No button?
		if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
			// the answer is no, go to the next question
			change_programming_state(PROGRAMMING_STATE_START);
			
			// and wait for the button to go up
			while(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_NOT_PRESSED_LEVEL) ;
		}
		
		// Yes button?
		if(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
			// the answer is "yes", change the direction (and move the platform),
			// go back to question with state no
			lcd_clrscr();
			lcd_set_cursor(1, 0);
			lcd_puts("Going ");
			
			if(slider_state.direction == DIRECTION_LEFT) {
				lcd_puts("LEFT");
				} else {
				lcd_puts("RIGHT");
			}
			
			drive(slider_state.direction);
			print_change_direction();

			// wait for the button to go up
			while(debounce_read(BUTTON2_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}
		break;
		case PROGRAMMING_STATE_START:
		// Button 2 = YES
		if(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
			lcd_clrscr();
			lcd_puts("Starting...");
			
			// wait for button to go up
			while(debounce_read(BUTTON2_READ, BUTTON2_PIN) != BUTTON_NOT_PRESSED_LEVEL);
			
			// set up sliding state
			// this is a rough estimate of seconds between pictures
			slider_state.tens_of_seconds_between_pictures = (programming_state.total_time_in_minutes *10L * 60L / programming_state.total_number_of_pictures)
			- programming_state.exposure_time_in_tens_of_second;
			slider_state.remaining_steps = programming_state.total_number_of_pictures;
			slider_state.over_pictures = 0;
			slider_state.speed = steps_per_slider / programming_state.total_number_of_pictures;
			
			lcd_clrscr();
			lcd_puts("Stabilizing...");
			for (uint8_t countdown = 20; countdown >0; --countdown)
			{
				lcd_set_cursor(1, 0);
				print_uint8(countdown / 10);
				lcd_putc('.');
				lcd_putc((countdown % 10) + '0');
				lcd_puts(" [s]");
				_delay_ms(100);
			}
			
			state = STATE_SLIDING;
		}

		if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
			// the answer is no, go to the first question
			change_programming_state(PROGRAMMING_CALIBRATION);

			// wait for the button to go up
			while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}
		break;
		case PROGRAMMING_CALIBRATION:
		// YES
		if(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_PRESSED_LEVEL) {
			lcd_clrscr();
			lcd_puts("Calibrating...");
			
			while(debounce_read(BUTTON2_READ, BUTTON2_PIN) == BUTTON_NOT_PRESSED_LEVEL) ;
			
			calibrate();
			
			change_programming_state(PROGRAMMING_STATE_TIME);
		}
		
		// NO
		if(debounce_read(BUTTON1_READ, BUTTON1_PIN) == BUTTON_PRESSED_LEVEL) {
			change_programming_state(PROGRAMMING_STATE_TIME);
			while(debounce_read(BUTTON1_READ, BUTTON1_PIN) != BUTTON_NOT_PRESSED_LEVEL) ;
		}
		break;
	}
	_delay_ms(100);
}

void take_picture() {
	uint8_t counter = programming_state.exposure_time_in_tens_of_second;

	// set focus
	SET_BIT(CAMERA_FOCUS_PORT, CAMERA_FOCUS_PIN);
	
	// trigger shutter after 30ms since focus (even on manual focus,
	// they cannot be pressed together)
	_delay_ms(30);
	
	SET_BIT(CAMERA_SHUTTER_PORT, CAMERA_SHUTTER_PIN);
	
	// keep shutter pressed for exposure time time.
	// this usually would not matter, unless running in manual mode
	// or time priority
	for(; counter > 0; --counter) {
		_delay_ms(100);
	}

	CLEAR_BIT(CAMERA_FOCUS_PORT, CAMERA_FOCUS_PIN);
	CLEAR_BIT(CAMERA_SHUTTER_PORT, CAMERA_SHUTTER_PIN);
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
		if(slider_state.remaining_steps > 0) {
			--slider_state.remaining_steps;
			} else {
			++slider_state.over_pictures;
		}

		millis_reset();

		print_sliding_state();

		// TODO: takes a step in given direction
		step(slider_state.speed, 3, slider_state.direction);

		unsigned long passed_time = millis_get();

		// this is constant in a run (rotation of specific degree will take always
		// same amount of time)
		// we must now wait remaining time
		repeat_wait = slider_state.tens_of_seconds_between_pictures - (passed_time / 100);
		for(; repeat_wait > 0; --repeat_wait) {
			if(repeat_wait % 10 == 0) {
				lcd_set_cursor(1, 9);
				print_uint8((repeat_wait / 10));
				lcd_puts(" [s]");
			}
			
			_delay_ms(100);
		}
		
		lcd_set_cursor(1, 9);
		print_uint8(0);
		lcd_puts(" [s]");
		
		// otherwise go and take pictures!!
		done =
		(slider_state.direction == DIRECTION_RIGHT && (IS_SET(RIGHT_SWITCH_READ, RIGHT_SWITCH_PIN) == SWITCH_ACTIVE))
		|| (slider_state.direction == DIRECTION_LEFT && (IS_SET(LEFT_SWITCH_READ, LEFT_SWITCH_PIN) == SWITCH_ACTIVE));
	}

	state = STATE_PROGRAMMING;
	print_total_time();
}

int
main (void)
{
	// TODO: camera requires two pins (focus and shutter) to be output
	CAMERA_SHUTTER_DDR |= _BV(CAMERA_SHUTTER_PIN);
	CAMERA_FOCUS_DDR   |= _BV(CAMERA_FOCUS_PIN);
	
	// enable pull-up resistor for button. DDRs should be input by default.
	BUTTON1_PORT |= 1 << BUTTON1_PIN;
	BUTTON2_PORT |= 1 << BUTTON2_PIN;
	
	// pull-up resistor for LEFT | RIGHT reed switch (connect it to GND to close circuit)
	LEFT_SWITCH_PORT  |= 1 << LEFT_SWITCH_PIN;
	RIGHT_SWITCH_PORT |= 1 << RIGHT_SWITCH_PIN;

	// set up motor pins as output
	MOTOR_DDR |= _BV(MOTOR_FIRST_PIN);
	MOTOR_DDR |= _BV(MOTOR_FIRST_PIN + 1);
	MOTOR_DDR |= _BV(MOTOR_FIRST_PIN + 2);
	MOTOR_DDR |= _BV(MOTOR_FIRST_PIN + 3);


	millis_init();
	lcd_init();
	lcd_on();
	lcd_clear();
	lcd_return_home();
	
	lcd_puts("Welcome...");
		
	// see position of the platform and if not on the side, go to the side
	// this operation is synchronous which means that it blocks UI / screen while the motor is moving
	if(!update_direction_based_on_platform_position()) {
		// I do not have motor just yet, comment it out
		lcd_set_cursor(1, 0);
		lcd_puts("Going LEFT");
		drive(DIRECTION_LEFT);
	}

	// set initial values
	programming_state.exposure_time_in_tens_of_second = 10;
	programming_state.total_time_in_minutes = 30;
	programming_state.total_number_of_pictures = 300;

	steps_per_slider = get_steps_per_slide();
	if(steps_per_slider == 0xFFFF || steps_per_slider == 0) {
		lcd_set_cursor(1, 0);
		lcd_puts("Not calibrated..");
		calibrate();
	}
	
	change_programming_state(PROGRAMMING_STATE_TIME);
	
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
