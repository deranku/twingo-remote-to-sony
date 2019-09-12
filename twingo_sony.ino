// https://www.reddit.com/r/diyelectronics/comments/413liz/renault_steering_wheel_remote_to_jvc/
// https://youtu.be/fk73SCOE8qU
// https://pastebin.com/fXbScxV4


// Renault Twingo / Clio steering wheel remote wire functions

// pin_cycle_current  1     0              2
//         OUTPUTS    BLUE  GREEN          YELLOW
// INPUTS  PIN#       3     5              6

// BLACK   2          MUTE  TOP RIGHT BTN  TOP LEFT BTN
// RED     4          VOL+  BOTTOM BTN     VOL-

//                    HIGH  HIGH           LOW    SCROLL UP (CCW)
// BROWN   7          HIGH  LOW            HIGH  SCROLLWHEEL
//                    LOW   HIGH           HIGH   SCROLL DN (CW)

// Outputs are set LOW one at a time (the other outputs will be HIGH). Inputs (with internal pull-up) are then evaluated.
//   If an input is being pulled LOW this means a button is being pressed. Taking into account which output is currently LOW
//   we know which button this is. For example, is output pin 3 (Blue wire) is currently LOW and we also read LOW on
//   input pin 2 (Black) we know the MUTE button is being pressed.
// For the scrollwheel we must take into account its last known position in order to determine if there has been a change.
// We can determine the direction based on which pins are being pulled LOW.

// Connect Renault Twingo / Clio steering wheel remote wires to these pins
#define BLACKPIN    2 // D2
#define BLUEPIN     3 // D3
#define REDPIN      4 // D4
#define GREENPIN    5 // D5
#define YELLOWPIN   6 // D6
#define BROWNPIN    7 // D7

// Connect reed relays to these pins

#define OFF_PIN         14
#define MUTE_PIN        15
#define SOURCE_PIN      16
#define VOL_UP_PIN      17
#define VOL_DN_PIN      18
#define SIRI_PIN        19
#define PRESET_UP_PIN   10
#define PRESET_DN_PIN   11

// On-board LED, useful for debugging
#define LEDPIN     13 // D13

// Set number of output pins and put those pins in an array to cycle through when polling the input pins
#define OUT_PINS 3
unsigned char out_pins[OUT_PINS] = {GREENPIN, BLUEPIN, YELLOWPIN};

// Duration in ms for sending input from the scrollwheel
const unsigned int wheel_send_dur = 50;

// While any button is active, the corresponding pins are not set LOW
bool button_active = false;


void setup() {

  Serial.begin(115200);

  // Set the proper pins as output
  pinMode(OFF_PIN, OUTPUT);
  pinMode(MUTE_PIN, OUTPUT);
  pinMode(SOURCE_PIN, OUTPUT);
  pinMode(VOL_UP_PIN, OUTPUT);
  pinMode(VOL_DN_PIN, OUTPUT);
  pinMode(SIRI_PIN, OUTPUT);
  pinMode(PRESET_UP_PIN, OUTPUT);
  pinMode(PRESET_DN_PIN, OUTPUT);

  // Output LOW to make sure optocouplers are off
  digitalWrite(OFF_PIN, LOW);
  digitalWrite(MUTE_PIN, LOW);
  digitalWrite(SOURCE_PIN, LOW);
  digitalWrite(VOL_UP_PIN, LOW);
  digitalWrite(VOL_DN_PIN, LOW);
  digitalWrite(SIRI_PIN, LOW);
  digitalWrite(PRESET_UP_PIN, LOW);
  digitalWrite(PRESET_DN_PIN, LOW);

  // Set the pins connected to the steering wheel remote as input / output
  pinMode(BLACKPIN, INPUT_PULLUP);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(REDPIN, INPUT_PULLUP);
  pinMode(GREENPIN, OUTPUT);
  pinMode(YELLOWPIN, OUTPUT);
  pinMode(BROWNPIN, INPUT_PULLUP);

  pinMode(LEDPIN, OUTPUT);                  // Set pin connected to on-board LED as output...
  digitalWrite(LEDPIN, LOW);                // ...and turn LED off
}




// The steering wheel remote has 6 buttons and a scrollwheel, interfaced via 6 wires.
// This function will cycle through the output pins, setting one pin LOW at a time.
// It will then poll the input pins to see which input pins - if any - are pulled LOW.
void loop() {

  // To keep track of which output pin is currently LOW
  static unsigned char pin_cycle_current = 0;
  // To store the last known scrollwheel position
  static unsigned char pin_cycle_stored;

  // Only cycle through pins for scrollwheel, if no button is currently pressed
  if(!button_active){
  
    // After booting, there is no known last position for the scrollwheel
    // So on the first poll of the scrollwheel just store the current position and don't send a command
    static boolean first_run = true;
    unsigned char i;
  
    // Reset pin_cycle_current counter after last pin
    if (++pin_cycle_current > (OUT_PINS - 1)) pin_cycle_current = 0;     
  
    // Cycle through the output pins, setting one of them LOW and the rest HIGH
    for (i = 0; i < OUT_PINS; i++) {                                     
      if (i == pin_cycle_current)
        digitalWrite(out_pins[i], LOW);
      else
        digitalWrite(out_pins[i], HIGH);
    }
  
  
    // Scrollwheel
    // We're only interested if this pin is being pulled LOW
    if (!digitalRead(BROWNPIN)) {

      if (pin_cycle_current != pin_cycle_stored) {
      // If the output that's currently LOW is different from the one that was LOW the last time
      //  we came through here, then the scrollwheel has changed position
  
        // Result of this calculation can range from -2 to 2
        signed char scrollwheel_current = pin_cycle_current - pin_cycle_stored; 
        // Store which output pin is currently LOW
        pin_cycle_stored = pin_cycle_current;                            
        // If this is the first run, don't send a command
        if (first_run) {                                                 
          //   (since there was no previously known scrollwheel position)
          first_run = false;                                             
          return;
        }
        if ((scrollwheel_current == 1) || (scrollwheel_current == -2)) { 
          // If above calculation resulted in 1 or -2 the scrollwheel was rotated down (cw)
  
          Serial.println("                        Scroll wheel DOWN");
          digitalWrite(PRESET_UP_PIN, HIGH);
          digitalWrite(LEDPIN, HIGH);
          delay(wheel_send_dur);
          digitalWrite(PRESET_UP_PIN, LOW);
          digitalWrite(LEDPIN, LOW);
        }else {                                                          
          // If above calculation resulted in anything else the scrollwheel was rotated up (ccw)
  
          Serial.println("                        Scroll wheel UP");
          digitalWrite(PRESET_DN_PIN, HIGH);
          digitalWrite(LEDPIN, HIGH);
          delay(wheel_send_dur);
          digitalWrite(PRESET_DN_PIN, LOW);
          digitalWrite(LEDPIN, LOW);
        }
      }
    }
  }

  // We're only interested if this pin is being pulled LOW
  if (!digitalRead(REDPIN)) {   

    if(!digitalRead(GREENPIN)){
      // RED (input) is LOW while GREEN (output) is LOW
      // BOTTOM button pressed
      Serial.println("                        Bottom button pressed");
      digitalWrite(SOURCE_PIN, HIGH);
      digitalWrite(LEDPIN, HIGH);
      button_active = true;
    }
    else if(!digitalRead(BLUEPIN)){
      // RED (input) is LOW while BLUE (output) is LOW
      // VOLUME + button pressed
      Serial.println("                        Vol + button pressed");
      digitalWrite(VOL_UP_PIN, HIGH);
      digitalWrite(LEDPIN, HIGH);
      button_active = true;
    }
    else if(!digitalRead(YELLOWPIN)){
        // RED (input) is LOW while YELLOW (output) is LOW:
        // VOLUME - button pressed
      Serial.println("                        Vol - button pressed");
      digitalWrite(VOL_DN_PIN, HIGH);
      digitalWrite(LEDPIN, HIGH);
      button_active = true;
    }
  }

  // We're only interested if this pin is being pulled LOW
  else if (!digitalRead(BLACKPIN)) { 

    if(!digitalRead(GREENPIN)){
      // BLACK (input) is LOW while GREEN (output) is LOW
      // TOP RIGHT button is pressed
      Serial.println("                        Top right button pressed");
      digitalWrite(SIRI_PIN, HIGH);
      digitalWrite(LEDPIN, HIGH);
      button_active = true;
    }
    else if(!digitalRead(BLUEPIN)){
      // BLACK (input) is LOW while BLUE (output) is LOW
      // MUTE button is pressed
      Serial.println("                        Mute button pressed");
      digitalWrite(MUTE_PIN, HIGH);
      digitalWrite(LEDPIN, HIGH);
      button_active = true;
    }
    else if(!digitalRead(YELLOWPIN)){
      // BLACK (input) is LOW while YELLOW (output) is LOW
      // TOP LEFT button is pressed
      Serial.println("                        Top left button pressed");
      digitalWrite(OFF_PIN, HIGH);
      digitalWrite(LEDPIN, HIGH);
      button_active = true;
    }
  }
  else{
    // No button pressed
    // Serial.println("NO buttons pressed");
    button_active = false;
  }


  if(!button_active) {
    digitalWrite(OFF_PIN, LOW);
    digitalWrite(MUTE_PIN, LOW);
    digitalWrite(SOURCE_PIN, LOW);
    digitalWrite(VOL_UP_PIN, LOW);
    digitalWrite(VOL_DN_PIN, LOW);
    digitalWrite(SIRI_PIN, LOW);
    digitalWrite(PRESET_UP_PIN, LOW);
    digitalWrite(PRESET_DN_PIN, LOW);
    digitalWrite(LEDPIN, LOW);
  }

}