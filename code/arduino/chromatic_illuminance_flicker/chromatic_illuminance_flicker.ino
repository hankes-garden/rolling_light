// This code drives multiple LEDs with different frequencies, 
// and modulates them with both chromatic and illuminance changes.
// by Lin Yang, CSE, HKUST.

const int LED_COUNT = 2;	// number of multi-color LEDs
const int CHANGE_COUNT = 18;	// change number of each LED
const unsigned long ARR_FLICKER_FREQ [LED_COUNT] = {73, 79};	// flicker frequency of each LED
const int ARR_COLOR_PIN[LED_COUNT][CHANGE_COUNT] = { {2,2,2,2,0,0,3,3,3,3,0,0,4,4,4,4,0,0},\
 {5,5,5,5,0,0,6,6,6,6,0,0,7,7,7,7,0,0}}; // PIN of each change

unsigned long arrColorDuration [LED_COUNT] = {0};	// color duration of each LED (in microsecond)
unsigned long arrPreviousTime [LED_COUNT] = {0};	// timestamp of previous color change of each LED
short arrCurrentColorIndex [LED_COUNT] = {0};	// current color index of each LED

unsigned long nCurrentTime = 0;	// current timestamp

void setup() {
	// compute the color duration for each LED
	for (int i=0; i<LED_COUNT; ++i)
	{
		arrColorDuration[i] = 1000000/ARR_FLICKER_FREQ[i]/CHANGE_COUNT;
	}
	
	// setup pin mode & turn off all PINs
	for (int i=0; i<LED_COUNT; ++i)
	{
		for (int j=0; j<CHANGE_COUNT; ++j)
		{
			pinMode(ARR_COLOR_PIN[i][j], OUTPUT);
			digitalWrite(ARR_COLOR_PIN[i][j], LOW);
			// digitalWrite(ARR_COLOR_PIN[i][j], HIGH);
		}
	}
}

void loop() {
	// return;
  nCurrentTime = micros();
  
  // Note that the microsecond value may overflow after every 70 minutes.
  // Since we donot need long experiment over 70 minute, we just ignore it for now.
  for(int i=0; i<LED_COUNT; ++i)
  {
	  if(nCurrentTime<arrPreviousTime[i]) // overflowed!
	  {
		  for(int j=0; j<LED_COUNT; ++i)
		  {
			arrPreviousTime[j] = nCurrentTime;
		  }
		  
		  return;
	  }
  }
  
  for(int i=0; i<LED_COUNT; ++i)
  {
	  if (nCurrentTime-arrPreviousTime[i] >= arrColorDuration[i])
	  {
			// turn off current color & Turn on next color
			short nCurrentColorIndex = arrCurrentColorIndex[i];
			digitalWrite(ARR_COLOR_PIN[i][nCurrentColorIndex], LOW);
			
			short nNextColorIndex = (++nCurrentColorIndex) % CHANGE_COUNT;
			digitalWrite(ARR_COLOR_PIN[i][nNextColorIndex], HIGH);
			arrCurrentColorIndex[i] = nNextColorIndex;

			// save the time of this color change
			arrPreviousTime[i] = nCurrentTime;
	  }
  }
  

}