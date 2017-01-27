unsigned long nPreviousTime = 0;	// last shinning time
unsigned long nCurrentTime = 0;		// current time

const unsigned long FLICKER_FREQ = 1;		// flicker frequency in Hertz
const unsigned long FLICKER_INTERVAL = 1000000/FLICKER_FREQ;	// shinning duration for each LED in microseconds

short nCurrentLED = 2;	// current shinning LED, availabe PIN: 2(blue), 4(Green), 7(red), 13(no light)

void setup() {
	// turn off LEDs
	pinMode(nCurrentLED, OUTPUT);
	digitalWrite(nCurrentLED, LOW);
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
	
	// ultra-short pulse
	digitalWrite(nCurrentLED, LOW);
    digitalWrite(nCurrentLED, HIGH);
	// delay(5);
	digitalWrite(nCurrentLED, LOW);
	
  }
}