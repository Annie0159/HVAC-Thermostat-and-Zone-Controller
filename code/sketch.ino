// Pin definitions
const int LED_GREEN = 2;
const int LED_BLUE = 3;
const int LED_RED = 4;
const int SWITCH_ON_OFF = 5;
const int SWITCH_HEAT_COOL = 6;
const int BTN_TEMP_DOWN = 7;
const int BTN_TEMP_UP = 8;
const int PWM_SERVO_Z1 = 9;
const int PWM_SERVO_Z2 = 10;
const int ADC_TEMP_Z1 = A0;
const int ADC_TEMP_Z2 = A1;

// Global variables for system state
int desiredTemp = 70;
int currentTempZ1, currentTempZ2;
bool systemOn = false;
bool systemHeating = false;
bool zone1Open = false;
bool zone2Open = false;
bool errorState = false;

// Timers for non-blocking operations
unsigned long lastSampleTime = 0;
unsigned long lastSerialTime = 0;

void setup() {
  // Set pin modes
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(PWM_SERVO_Z1, OUTPUT);
  pinMode(PWM_SERVO_Z2, OUTPUT);
  pinMode(SWITCH_ON_OFF, INPUT_PULLUP);
  pinMode(SWITCH_HEAT_COOL, INPUT_PULLUP);
  pinMode(BTN_TEMP_DOWN, INPUT_PULLUP);
  pinMode(BTN_TEMP_UP, INPUT_PULLUP);
  pinMode(ADC_TEMP_Z1, INPUT);
  pinMode(ADC_TEMP_Z2, INPUT);
  
  // Initialize Serial communication
  Serial.begin(9600);
  
  // Initial state display
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_RED, LOW);
  
  // Setup PWM timers for both servos
  setupTimer(); 
}

void loop() {
  // Read switches for system state
  systemOn = digitalRead(SWITCH_ON_OFF) == LOW;
  systemHeating = digitalRead(SWITCH_HEAT_COOL) == LOW;

  
  // Check for error state 
  errorState = !systemOn;
  
  if (errorState) {
    // Determine which LED to light based on time
    unsigned long t = (millis() / 200) % 3; // change LED every 500 ms

    // Turn off all LEDs first
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_RED, LOW);

    if (t == 0) digitalWrite(LED_RED, HIGH);
    else if (t == 1) digitalWrite(LED_BLUE, HIGH);
    else digitalWrite(LED_GREEN, HIGH);

    // Optionally, force zones closed while in error
    zone1Open = false;
    zone2Open = false;
  }

  // Read buttons for desired temperature
  if (digitalRead(BTN_TEMP_UP) == LOW) {
    desiredTemp++;
    delay(200); 
  }
  if (digitalRead(BTN_TEMP_DOWN) == LOW) {
    desiredTemp--;
    delay(200); 
  }
  
  // Sample temperature every 1 second
  if (millis() - lastSampleTime >= 1000) {
    lastSampleTime = millis();
    currentTempZ1 = map(analogRead(ADC_TEMP_Z1), 1023, 0, 0, 100);
    currentTempZ2 = map(analogRead(ADC_TEMP_Z2), 1023, 0, 0, 100);
  }
  
  // Main state machine logic
  if (systemOn) {
    digitalWrite(LED_GREEN, HIGH);
    
    // Determine heating/cooling mode and set LEDs
    if(zone1Open || zone2Open)
    {
      if (systemHeating) {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_BLUE, LOW);
      } else {
        digitalWrite(LED_BLUE, HIGH);
        digitalWrite(LED_RED, LOW);
      }
    }else{
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_RED, LOW);
    }
    
    // Zone 1 logic
    if ((systemHeating && currentTempZ1 < desiredTemp) || (!systemHeating && currentTempZ1 > desiredTemp)) {
      zone1Open = true;
    } else {
      zone1Open = false;
    }
    
    // Zone 2 logic
    if ((systemHeating && currentTempZ2 < desiredTemp) || (!systemHeating && currentTempZ2 > desiredTemp)) {
      zone2Open = true;
    } else {
      zone2Open = false;
    }
    
  } else {
    // System is off
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_BLUE, LOW);
    zone1Open = false;
    zone2Open = false;
  }
  
  // Update servos based on zone state
  updateServos();
  
  // Send serial message every 10 seconds
  if (millis() - lastSerialTime >= 10000) {
    lastSerialTime = millis();
    Serial.print("Set Point: ");
    Serial.print(desiredTemp);
    Serial.print("F, Temperature Z1: ");
    Serial.print(currentTempZ1);
    Serial.print("F, Temperature Z2: ");
    Serial.print(currentTempZ2);
    Serial.print("F, System: ");
    Serial.print(systemOn ? "On" : "Off");
    Serial.print(", System Mode: ");
    Serial.print(systemHeating ? "Heat" : "Cool");
    Serial.print(", Zone 1: ");
    Serial.print(zone1Open ? "Open" : "Closed");
    Serial.print(", Zone 2: ");
    Serial.println(zone2Open ? "Open" : "Closed");
  }
}

// --- PWM Timer Functions ---
void setupTimer() {

  cli(); // disable interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1A |= (1 << COM1A1) | (1 << COM1B1);

  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << WGM12);

  TCCR1B |= (1 << CS11);

  ICR1 = 40000;

  OCR1A = 2000; // pin 9 (OC1A)
  OCR1B = 2000; // pin 10 (OC1B)

  sei(); // enable interrupts
}

void updateServos() {

  if (zone1Open) {
    OCR1A = 4000; // open = 2ms pulse
  } else {
    OCR1A = 2000; // closed = 1ms pulse
  }

  if (zone2Open) {
    OCR1B = 4000; // open = 2ms
  } else {
    OCR1B = 2000; // closed = 1ms
  }
}
