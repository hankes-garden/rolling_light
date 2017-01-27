const int GATE_1 = 2;
const int GATE_2 = 3;
const int GATE_3 = 4;

const int LOW_PIN = 13;

unsigned long nPreviousTime = 0;
unsigned long nCurrentTime = 0;

const unsigned long FLICKER_INTERVAL = 1000000;

void setup() {
	// turn off all LEDs
	pinMode(GATE_1, OUTPUT);
	digitalWrite(GATE_1, LOW);
	
	pinMode(GATE_2, OUTPUT);
	digitalWrite(GATE_2, LOW);
	
	pinMode(GATE_3, OUTPUT);
	digitalWrite(GATE_3, LOW);
	
	pinMode(LOW_PIN, OUTPUT);
	digitalWrite(LOW_PIN, LOW);
	


}

void loop() {
  nCurrentTime = micros();
  if(nCurrentTime < nPreviousTime){ // The time value has been overflowed, ignore it. 
								    // Note there will be a frequency change if we ignore the overflow.
	  nPreviousTime = nCurrentTime;
	  return;
  }

  if (nCurrentTime - nPreviousTime >= FLICKER_INTERVAL) // time for flicker
  {
    // save the last time you blinked the LED
    nPreviousTime = nCurrentTime;
	
	// turn off current shinning LED & Turn on next LED
    digitalWrite(GATE_1, !digitalRead(GATE_1) );
	digitalWrite(GATE_2, !digitalRead(GATE_2) );
	digitalWrite(GATE_3, !digitalRead(GATE_3) );
  }
}