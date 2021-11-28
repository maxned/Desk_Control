// Max Nedorezov
// Originally developed on November 27, 2021

// Arduino button pins
#define BUTTON_1_IN     2
#define BUTTON_1_OUT    3
#define BUTTON_2_IN     4
#define BUTTON_2_OUT    5
#define BUTTON_3_IN     6
#define BUTTON_3_OUT    7
#define BUTTON_4_IN     8
#define BUTTON_4_OUT    9
#define BUTTON_UP_IN    10
#define BUTTON_UP_OUT   11
#define BUTTON_DOWN_IN  12
#define BUTTON_DOWN_OUT 13

// Number of buttons supported
#define NUM_BUTTONS 6

// Maximum move time of the desk in milliseconds
#define MAX_MOVE_TIME 17 * 1000

// All buttons
enum buttons {
  BUTTON_1,
  BUTTON_2,
  BUTTON_3,
  BUTTON_4,
  BUTTON_UP,
  BUTTON_DOWN,
  NONE
};

// Arduino input pins in the order of enum buttons
uint8_t input_pins[NUM_BUTTONS] = {
  BUTTON_1_IN,
  BUTTON_2_IN,
  BUTTON_3_IN,
  BUTTON_4_IN,
  BUTTON_UP_IN,
  BUTTON_DOWN_IN
};

// Arduino output pins in the order of enum buttons
uint8_t output_pins[NUM_BUTTONS] = {
  BUTTON_1_OUT,
  BUTTON_2_OUT,
  BUTTON_3_OUT,
  BUTTON_4_OUT,
  BUTTON_UP_OUT,
  BUTTON_DOWN_OUT };

// Know when the Arduino is holding a button
bool arduino_holding_button = false;

// Milliseconds at which the desk started to move, used to know when to timeout
// the Arduino hold
unsigned long start_move_time;

void setup()
{
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(input_pins[i], INPUT);
    pinMode(output_pins[i], OUTPUT);
    digitalWrite(output_pins[i], LOW);
  }

  Serial.begin(115200);
}

void loop()
{
  // Read all current buttons
  uint8_t button_bitmap_state = 0;
  for (int i = 0; i < NUM_BUTTONS; i++) {
    button_bitmap_state |= digitalRead(input_pins[i]) << (NUM_BUTTONS - 1 - i);
  }

  // Figure out which button is pressed
  // If another button was pressed while the current one is being held, the
  // state will be set to NONE
  // When buttons 3 and 4 are pressed, signals show up at other buttons too
  // Don't hijack the up/down buttons because they're used for human adjustments
  enum buttons pressed_button = NONE;
  switch (button_bitmap_state) {
    case 0b100000: pressed_button = BUTTON_1; break;
    case 0b010000: pressed_button = BUTTON_2; break;
    case 0b011001: pressed_button = BUTTON_3; break;
    case 0b100101: pressed_button = BUTTON_4; break;
    // case 0b000010: pressed_button = BUTTON_UP; break;
    // case 0b000001: pressed_button = BUTTON_DOWN; break;
    default: pressed_button = NONE; break;
  }

  // If no button was pressed and now one became pressed, begin holding it
  if (!arduino_holding_button && pressed_button != NONE) {
    arduino_holding_button = true;
    start_move_time = millis();

    // When Buttons 3 and 4 are pressed, signals show up at other buttons too
    if (pressed_button == BUTTON_3) {
      digitalWrite(BUTTON_2_OUT, HIGH);
      digitalWrite(BUTTON_3_OUT, HIGH);
      digitalWrite(BUTTON_DOWN_OUT, HIGH);
    } else if (pressed_button == BUTTON_4) {
      digitalWrite(BUTTON_1_OUT, HIGH);
      digitalWrite(BUTTON_4_OUT, HIGH);
      digitalWrite(BUTTON_DOWN_OUT, HIGH);
    } else {
      digitalWrite(output_pins[pressed_button], HIGH);
    }

    Serial.print("Started holding button: ");
    Serial.println(pressed_button);
  }

  // If Arduino is currently holding a button and the timer runs out or the
  // state changes to NONE (if another button was pressed), then stop holding it
  if (arduino_holding_button &&
      (millis() - start_move_time > MAX_MOVE_TIME || pressed_button == NONE)) {
    arduino_holding_button = false;

    for (int i = 0; i < NUM_BUTTONS; i++) {
      digitalWrite(output_pins[i], LOW);
    }

    Serial.println("Ended holding any button presses");

    // This delay allows stopping the desk movement without the Arduino starting
    // to hold the new button press.
    // For example, say I press button 1 and the Arduino starts holding it. Then
    // while the Arduino is holding it I press button 2.
    // If I hold it < 1 second then the Arduino won't start holding button 2.
    // If I hold it > 1 second then the Arduino will start holding button 2.
    delay(1000);
  }

  // Don't read values too quickly, otherwise I've seen it get iffy
  delay(10);
}
