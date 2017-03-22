// This code drives multiple LEDs with different frequencies, 
// and modulates them with both chromatic and illuminance changes.
// by Lin Yang, CSE, HKUST.

const int LIGHT_NUM = 4;	// Light number
const int STATE_NUM = 4;	// State number of each light

unsigned long g_arrLightFrequencies [LIGHT_NUM] = {67, 67, 67, 67};	// State-changing frequency of each light
int g_arrStates[LIGHT_NUM][STATE_NUM] = { {2,3,4,0}, {5,6,7,0}, \
                                          {8,9,10,0}, {11,12,13,0} }; // Control PIN of each state

unsigned long g_arrStateDurations [LIGHT_NUM] = {0};	    // Duration of state (in microsecond)
unsigned long g_arrPreviousChangeTime [LIGHT_NUM] = {0};	// Timestamp of previous state change
short g_arrCurrentStateIndex [LIGHT_NUM] = {0};	            // Current color index of each LED

unsigned long g_nCurrentTime = 0;	// current timestamp

void setup() {
	// compute the color duration for each LED
	for (int i=0; i<LIGHT_NUM; ++i)
	{
		g_arrStateDurations[i] = 1000000/g_arrLightFrequencies[i]/STATE_NUM;
	}
	
	// setup pin mode & turn off all PINs
	for (int i=0; i<LIGHT_NUM; ++i)
	{
		for (int j=0; j<STATE_NUM; ++j)
		{
			pinMode(g_arrStates[i][j], OUTPUT);
			digitalWrite(g_arrStates[i][j], LOW);
		}
	}
}

void loop() {
	// return;
  g_nCurrentTime = micros();
  
  // Note that the microsecond value may overflow after every 70 minutes.
  // Since we donot need long experiment over 70 minute, we just ignore it for now.
  for(int i=0; i<LIGHT_NUM; ++i)
  {
	  if(g_nCurrentTime<g_arrPreviousChangeTime[i]) // overflowed!
	  {
		  for(int j=0; j<LIGHT_NUM; ++i)
		  {
			g_arrPreviousChangeTime[j] = g_nCurrentTime;
		  }
		  
		  return;
	  }
  }
  
  for(int i=0; i<LIGHT_NUM; ++i)
  {
	  if (g_nCurrentTime-g_arrPreviousChangeTime[i] >= g_arrStateDurations[i])
	  {
			// turn off current color & Turn on next color
			short nCurrentColorIndex = g_arrCurrentStateIndex[i];
			digitalWrite(g_arrStates[i][nCurrentColorIndex], LOW);
			
			short nNextColorIndex = (++nCurrentColorIndex) % STATE_NUM;
			digitalWrite(g_arrStates[i][nNextColorIndex], HIGH);
			g_arrCurrentStateIndex[i] = nNextColorIndex;

			// save the time of this color change
			g_arrPreviousChangeTime[i] = g_nCurrentTime;
	  }
  }
  

}