// This code drives multiple lights with different changing patterns: 
// 1. modulates them with both chromatic and illuminance changes.
// 2. Synchronize btw colors (not periods).
// Coded by Lin Yang, CSE, HKUST.
#include "name.h"

const int LIGHT_COUNT = 4;  // light number
const int COLOR_COUNT = 3;  // color number in each light
const int BASE_FREQ = 61;   // base frequency for synchronization & luminance padding
                            // Note: this value must be less than or equal to minimal color frequency!
const int STATE_NUMBER = 6; // number of states in each pattern.

unsigned long g_nBasePeriod = 1000000/BASE_FREQ;  // base period
int g_arrStatePins[LIGHT_COUNT][STATE_NUMBER] = { {2,0,3,0,4,0}, {5,0,6,0,7,0}, \
                                                  {8,0,9,0,10,0}, {11,0,12,0,13,0} };
int g_arrColorFrequencies [LIGHT_COUNT] = {61, 61, 67, 91};

CState* g_arrCurrentState[LIGHT_COUNT] = {};  // current state of each light
unsigned long g_arrLastStateChangeTime [LIGHT_COUNT] = {};  // last state change time of each light

unsigned long g_nCurrentTime = 0; // current time stamp.

CState* createChangePattern(int nChangeCount, int arrPins[], unsigned long arrDurations[]);
void initLights(CState* arrLightPatterns[]);

// intialize the state-changing pattern for each light
void initLights(CState* arrLightPatterns[])
{
    int nColorFreq = 0;
    unsigned long nColorDuration = 0;
    unsigned long nDarkDuration = 0;
    float dDutyCycle = 0.0;
    unsigned long arrDurations [STATE_NUMBER] = {};
    for (int nLightID=0; nLightID<LIGHT_COUNT; ++nLightID)
    {
        nColorFreq = g_arrColorFrequencies[nLightID];    // color freq
        nColorDuration = 1000000/nColorFreq/COLOR_COUNT; // duration of each color
        nDarkDuration = g_nBasePeriod/COLOR_COUNT - nColorDuration; // dark padding duration
        dDutyCycle = 1.0 - (nDarkDuration*COLOR_COUNT*1.0/g_nBasePeriod);
        Serial.print("\nLight #"); Serial.print(nLightID); Serial.print(":\n");
        Serial.print("--> Dark duration = "); Serial.print(nDarkDuration); Serial.print(".\n");
        Serial.print("--> Duty cycle = "); Serial.print(dDutyCycle); Serial.print(".\n");
        
        for(int i=0; i<STATE_NUMBER; ++i)
        {
            arrDurations[i] = ( (i%2==0) ? nColorDuration : nDarkDuration);
        }
        CState* pPattern = createChangePattern(STATE_NUMBER, g_arrStatePins[nLightID], arrDurations);
        g_arrCurrentState[nLightID] = pPattern;
    }
}

// Alloc & initialize a single linked circular list for light change pattern
// Note: the memory should be released manually!
CState* createChangePattern(int nChangeCount, int arrPins[], unsigned long arrDurations[])
{
    // create head
    CState* pHead = new CState();
    pHead->nPin = arrPins[0];
    pHead->nDuration = arrDurations[0];
    CState* pPreNode = pHead;
    
    for(int i=1; i<nChangeCount; ++i)
    {
        CState* pNode = new CState();
        pNode->nPin = arrPins[i];
        pNode->nDuration = arrDurations[i];
        
        pPreNode->pNext = pNode;
        pPreNode = pNode;
    }
    pPreNode->pNext = pHead; // make it a circular linked list
    
    return pHead;
}

// Note: Considering the setup function is only called once at the begin,
// the dynamic memory alloced by initLights() is not released.
void setup() 
{
    Serial.begin(9600);
    
    initLights(g_arrCurrentState);
    
    // setup pin mode & turn off all PINs
	for (int i=2; i<13; ++i)
	{
        pinMode(i, OUTPUT);
        digitalWrite(i, HIGH);
	}
}

void loop() 
{
    g_nCurrentTime = micros();
    // Note that the microsecond value may overflow after every 70 minutes.
    // Since we donot need long experiment over 70 minute, we just ignore it for now.
    for(int i=0; i<LIGHT_COUNT; ++i)
    {
      if(g_nCurrentTime<g_arrLastStateChangeTime[i]) // overflowed!
      {
          g_arrLastStateChangeTime[i] = g_nCurrentTime;
          return;
      }
    }
    
    for(int i=0; i<LIGHT_COUNT; ++i)
    {
        if(g_arrLastStateChangeTime[i] == 0) // first time
        {
            // turn on the first PIN
            digitalWrite(g_arrCurrentState[i]->nPin, HIGH);
            g_arrLastStateChangeTime[i] = g_nCurrentTime;
            
        }
        else if(g_nCurrentTime-g_arrLastStateChangeTime[i]>=g_arrCurrentState[i]->nDuration) // time to change
        {
            // turn off current PIN
            digitalWrite(g_arrCurrentState[i]->nPin, LOW);
            // move to the next state
            g_arrCurrentState[i] = g_arrCurrentState[i]->pNext;
            digitalWrite(g_arrCurrentState[i]->nPin, HIGH);
            // Serial.print(g_arrCurrentState[i]->nPin); Serial.print(",  ");
            // Serial.print(g_arrCurrentState[i]->nDuration); Serial.print(".\n");
            // update last change time
            g_arrLastStateChangeTime[i] = g_nCurrentTime;
            
        }
        else
        {
            // do nothing
        }
    }
}