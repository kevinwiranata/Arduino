#include "pitches.h"

#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>

struct Indicator {
  bool state;
  int port;
  Indicator(int port) {
    this->port = port;
    this->state = false;
  }
  void toggle() {
  if (this->state)
        {
          digitalWrite(this->port,LOW);
          this->state = false;
        }
        else
        {
          digitalWrite(this->port, HIGH);
          this->state = true;
        }
}
};

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

#define SPEAKER 8
Indicator GREEN_LED(12);
Indicator WHITE_LED(11);
Indicator MOTOR(3);

const char NO_IN = '\0';
const int MIN_DECEL = -5;
const int DEFAULT_NUM_SECS = 3;
const int TOGGLE_RATE = 2;  // 1/2 Seconds

char inputChar = NO_IN;
bool wState = false;
bool gState = false;
bool mState = false;

int daisy[] = {
  NOTE_C5, NOTE_A5, NOTE_F4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_F4, NOTE_C4
};

double noteDurations[] = {
  1.5, 1.5, 1.5, 1.5, 4, 4, 4, 2, 4, 1
};

void setup() {
  Serial.begin(9600);
  pinMode(GREEN_LED.port, OUTPUT);
  pinMode(WHITE_LED.port, OUTPUT);
  GREEN_LED.toggle();
  WHITE_LED.toggle();
  // sing daisy
  for (int thisNote = 0; thisNote < 10; thisNote++) {
    double noteDuration = 1000 / noteDurations[thisNote];
    tone(SPEAKER, daisy[thisNote], noteDuration);
    double pauseBetweenNotes = noteDuration * 1.3;
    delay(pauseBetweenNotes);
    noTone(SPEAKER);
  }
  GREEN_LED.toggle();
  WHITE_LED.toggle();
  accel.begin();
  Serial.println("Fall detector activated.");
}

void loop() {
  if (Serial.available() > 0)
  {
    inputChar = Serial.read();
    Serial.println(inputChar);
  }
  sensors_event_t event;
  accel.getEvent(&event);
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  if(event.acceleration.z < MIN_DECEL || inputChar != NO_IN) {
    tone(SPEAKER, NOTE_C4);
    MOTOR.toggle();
    GREEN_LED.toggle();

    if (inputChar == 'd') { // Daisy
      Serial.println("Playing \"Daisy\"");
      for (int thisNote = 0; thisNote < 10; thisNote++) {
        double noteDuration = 1000 / noteDurations[thisNote];
        tone(SPEAKER, daisy[thisNote], noteDuration);
        double pauseBetweenNotes = noteDuration * 1.3;
        delay(pauseBetweenNotes);
        noTone(SPEAKER);
        GREEN_LED.toggle();
        WHITE_LED.toggle();
        inputChar = NO_IN;
      }
      GREEN_LED.toggle();
      return;
    }
    
    // Get number of seconds to call: 1 digit number only
    int numToggles = TOGGLE_RATE * DEFAULT_NUM_SECS;
    if (isDigit(inputChar)) {
      numToggles = TOGGLE_RATE * (inputChar - '0');
      Serial.print("Pinging for "); Serial.print(numToggles/TOGGLE_RATE); Serial.println(" seconds");
    }
    Serial.println("Pinging...");
    for (int i = 0; i < numToggles; ++i) {
      GREEN_LED.toggle();
      WHITE_LED.toggle();
      delay(1000 / TOGGLE_RATE);
    }
    MOTOR.toggle();
    GREEN_LED.toggle();
    noTone(SPEAKER);
    inputChar = NO_IN;
  }
  delay(100);
}
