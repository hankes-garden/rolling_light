// This script drive LED to generate chromatic & illuminance flicker
// by Jason
const int COLOR_COUNT = 3;							// color number of LED
const int ARR_LED_PIN [COLOR_COUNT] = {3, 5, 6};	// LED pins

unsigned long nPreColorTime = 0;					// timestamp of previous color change
const unsigned long CHROMATIC_FREQ = 67;			// chromatic change frequency in Hertz
const unsigned long CHROMATIC_CHANGE_INTERVAL = 1000000/CHROMATIC_FREQ/COLOR_COUNT;	// chromatic change in microseconds

short nCurrentIlluminance = 127;
unsigned long nPreIlluminanceTime = 0;				// timestamp of previous illuminance change
const unsigned long ILLUMINANCE_FREQ = 10000;			// illuminance change frequency in Hertz
const unsigned long ILLUMINANCE_CHANGE_INTERVAL = 1000000/ILLUMINANCE_FREQ/2;	// illuminance change in microseconds

unsigned long nCurrentTime = 0;			// current time

short nCurrentPin = 0;	// current shinning LED
short nNextPin = 0;		// next LED to shine

void setup() {
	// turn LED on and set it to medium illuminance
	for (int i=0; i<COLOR_COUNT; ++i) {
		analogWrite(ARR_LED_PIN[i], 127);
	} 
}

void loop() {
  nCurrentTime = micros();
  if(nCurrentTime < nPreIlluminanceTime)
  {
	  // The time value has been overflowed, ignore it.
	  // Note there will be a frequency change if we ignore the overflow.
	  nPreIlluminanceTime = nCurrentTime;
	  return;
  }

  if (nCurrentTime - nPreIlluminanceTime >= CHROMATIC_CHANGE_INTERVAL) // time for change
  {
	  // save the time of this change
	  nPreIlluminanceTime = nCurrentTime;
	  
	  nCurrentIlluminance = (nCurrentIlluminance+127)%254;
	  for(int i=0; i<COLOR_COUNT; ++i)
	  {
		  analogWrite(ARR_LED_PIN[i], nCurrentIlluminance);
	  }
	  
  }
}