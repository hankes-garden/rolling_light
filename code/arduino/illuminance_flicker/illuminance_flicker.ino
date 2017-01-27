// This code drives multiple LEDs to create illuminance flicker.
// by Jason

const int LED_COUNT = 3;	// number of multi-color LEDs
const int COLOR_COUNT = 3;	// color number of each LED
const unsigned long ARR_FLICKER_FREQ [LED_COUNT] = {67, 67, 67};	// flicker frequency of each LED
const int ARR_COLOR_PIN[LED_COUNT][COLOR_COUNT] = { {2,3,4}, {5,6,7}, {8,9,10} };	// PIN of each color for each LED

unsigned long arrLEDStateDuration [LED_COUNT] = {0};	// ON-state duration of each LED (in microsecond)
unsigned long arrPreviousTime [LED_COUNT] = {0};	// timestamp of previous state change of each LED

unsigned long nCurrentTime = 0;	// current timestamp

void setup() {
	// compute the color duration for each LED
	for (int i=0; i<LED_COUNT; ++i)
	{
		arrLEDStateDuration[i] = 1000000/ARR_FLICKER_FREQ[i]/2;
	}
	
	// setup pin mode & turn off all PINs
	for (int i=0; i<LED_COUNT; ++i)
	{
		for (int j=0; j<COLOR_COUNT; ++j)
		{
			pinMode(ARR_COLOR_PIN[i][j], OUTPUT);
			digitalWrite(ARR_COLOR_PIN[i][j], LOW);
		}
		
	}
}

void loop() {
  nCurrentTime = micros();
  
  // Note that the microsecond value may overflow after every 70 minutes.
  // Since we donot need long experiment over 70 minute, we just ignore it for now.
  for(int i=0; i<LED_COUNT; ++i)
  {
	  if(nCurrentTime<arrPreviousTime[i]) // overflowed!
	  {
		  arrPreviousTime[i] = nCurrentTime;
		  return;
	  }
  }
  
  for(int i=0; i<LED_COUNT; ++i)
  {
	  if (nCurrentTime-arrPreviousTime[i] >= arrLEDStateDuration[i])
	  {
			
			
			for (int j=0; j<COLOR_COUNT; ++j)
			{
				digitalWrite(ARR_COLOR_PIN[i][j], !digitalRead(ARR_COLOR_PIN[i][j]) );
			}
			
			// save the time of this color change
			arrPreviousTime[i] = nCurrentTime;
	  }
  }
  

}