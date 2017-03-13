const int PIN_COUNT = 12;
int g_arrCtrlPins [PIN_COUNT] = {2,3,4,5,6,7,8,9,10,11,12,13};
int g_nCurrentPinIndex = 0;

unsigned long nPreviousTime = 0;
unsigned long nCurrentTime = 0;

const unsigned long CHANGE_DURATION = 1000000;

void setup() {
	// turn off all LEDs
    for (int i=0; i<PIN_COUNT; ++i)
    {
        pinMode(g_arrCtrlPins[i], OUTPUT);
        digitalWrite(g_arrCtrlPins[i], LOW);
    }
    
    // turn the first pin on
    digitalWrite(g_arrCtrlPins[g_nCurrentPinIndex], HIGH);
    Serial.print("Current Pin:"); Serial.print(g_nCurrentPinIndex); Serial.print("\n");
    
    Serial.begin(9600);
}

void loop() {
  nCurrentTime = micros();
  if(nCurrentTime < nPreviousTime){ // The time value has been overflowed, ignore it. 
								    // Note there will be a frequency change if we ignore the overflow.
	  nPreviousTime = nCurrentTime;
	  return;
  }

  if (nCurrentTime - nPreviousTime >= CHANGE_DURATION) // time to change
  {
    // save the last time you blinked the LED
    nPreviousTime = nCurrentTime;
	
	// turn off current shinning LED & Turn on next LED
    digitalWrite(g_arrCtrlPins[g_nCurrentPinIndex], LOW);
    g_nCurrentPinIndex = (g_nCurrentPinIndex+1) % PIN_COUNT;
    digitalWrite(g_arrCtrlPins[g_nCurrentPinIndex], HIGH);
    Serial.print("Current Pin:"); Serial.print(g_arrCtrlPins[g_nCurrentPinIndex]); Serial.print("\n");

  }
}