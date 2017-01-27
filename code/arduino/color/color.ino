// availabe PIN: 2(blue), 4(Green), 7(red), 13(no light)
const int LED_COUNT = 3;	// number of LED to use
const int ARR_LED_PIN [LED_COUNT] = {2, 3, 4};	// LED to use

unsigned long nPreviousTime = 0;	// last shinning time
unsigned long nCurrentTime = 0;		// current time


const unsigned long FLICKER_FREQ = 67;		// flicker frequency in Hertz
const unsigned long FLICKER_INTERVAL = 1000000/FLICKER_FREQ/LED_COUNT;	// shinning duration for each LED in microseconds

short nCurrentLED = 0;	// current shinning LED
short nNextLED = 0;		// next LED to shine

void setup() {
	// turn off all LEDs
	for (int i=0; i<LED_COUNT; ++i) {
		pinMode(ARR_LED_PIN[i], OUTPUT);
		digitalWrite(ARR_LED_PIN[i], LOW);
	} 
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
    digitalWrite(ARR_LED_PIN[nCurrentLED], LOW);
	digitalWrite(ARR_LED_PIN[nNextLED], HIGH);
	
	nCurrentLED = nNextLED;
	nNextLED = (++nNextLED) % LED_COUNT;
  }
}