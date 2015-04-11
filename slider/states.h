#ifndef _STATES_H_
#define _STATES_H_

/// There are two master states: either we are programming, or sliding
#define STATE_SLIDING 0
#define STATE_PROGRAMMING 1

/// Perform on the new slider - this will
/// allow you to compute how many steps per slide are required
#define PROGRAMMING_CALIBRATION 0

/// Setting time of the slide
#define PROGRAMMING_STATE_TIME 1

/// Setting how long exposure should be
/// If using mirror lockup, it should at least be 2.5 seconds
/// Reasonable range from 0.5 - 30 second (though thirty means a long time :))
/// In some situations, using short stops is fine too.
#define PROGRAMMING_STATE_EXPOSURE_TIME 2

/// (or should I rather set time - but one defines another)
#define PROGRAMMING_STATE_PICTURES 3

/// Setting direction of the slide, "Change direction: YES/NO"
/// if no, go to next step, if yes, change direction and ask again
#define PROGRAMMING_STATE_DIRECTION 4

/// YES/NO "start timelapse", if no, go back to PROGRAMMING_STATE_TIME
#define PROGRAMMING_STATE_START 5

// Direction of the slide
// one of those two needs to be 0, and the other 1, (1 - x) is ussed to reverse direction
#define DIRECTION_RIGHT 1
#define DIRECTION_LEFT 0

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
  /// it must be lower than 200! (TODO: add cheks)
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
  
  /// When remaining steps reach 0, but platform does not reach
  /// the side of slider, it continues taking pictures
  /// and reports additional pictures count.
  uint16_t over_pictures;
  
  /// How many seconds between two pictures
  /// ('exposure_time_int_tens_of_seconds' represents how long the shutter button will be pressed,
  /// while this field represents how much time between two pictures has to pass)
  uint16_t tens_of_seconds_between_pictures;
} slider_state_t;

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
} programming_state_t;

#endif
