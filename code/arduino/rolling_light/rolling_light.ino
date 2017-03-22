// This is the Rolling-Light system driver, which will
// 1. drive multiple lights with different changing patterns: 
// 1. modulate them with both chromatic and illuminance changes.
// Created by Lin Yang, HKUST (lyangab@connect.ust.hk).
// All copyright reserved.
#include "name.h"

const int LIGHT_COUNT = 2;  // light number
const int COLOR_COUNT = 3;  // color number in each light
const int STATE_NUMBER = 6; // number of states in each pattern.

int g_arrStatePins[LIGHT_COUNT][STATE_NUMBER] = { {2,0,3,0,4,0}, {5,0,6,0,7,0}  };
int g_arrLightFrequencies [LIGHT_COUNT] = {79, 79,};
float g_arrLightDutyCycles [LIGHT_COUNT] = {0.75, 0.75,};

CState* g_arrCurrentStatePointer[LIGHT_COUNT] = {};  // pointer to current state of each light
unsigned long g_arrLastStateChangeTime [LIGHT_COUNT] = {};  // last state change time of each light

unsigned long g_nCurrentTime = 0; // current time stamp.

CState* createChangePattern(int nChangeCount, int arrPins[], unsigned long arrDurations[]);
void initLight(CState* arrLightPatterns[]);

// Intialize the state-changing patterns of lights, return the 
// head pointer of each pattern.
void initLight(CState* arrLightPatterns[])
{
    unsigned long arrDurations [STATE_NUMBER] = {};
    int nFrequency = 0;
    float dDutyCycle = 0.0;
    unsigned long nColorDuration = 0;
    unsigned long nDarkDuration = 0;
    for (int nLightID=0; nLightID<LIGHT_COUNT; ++nLightID)
    {
        nFrequency = g_arrLightFrequencies[nLightID];
        dDutyCycle = g_arrLightDutyCycles[nLightID];
        nColorDuration = (dDutyCycle*1000000/nFrequency)/COLOR_COUNT; // duration of each color
        nDarkDuration = (1-dDutyCycle)*1000000/nFrequency; // dark duration
        Serial.print("\nLight #"); Serial.print(nLightID); Serial.print(":\n");
        Serial.print("--> Dark duration = "); Serial.print(nDarkDuration); Serial.print(".\n");
        Serial.print("--> Duty cycle = "); Serial.print(dDutyCycle); Serial.print(".\n");
        
        for(int i=0; i<STATE_NUMBER; ++i)
        {
            arrDurations[i] = (i%2==0)?nColorDuration:(nDarkDuration/COLOR_COUNT);
        }
        
        // Note: memory leak here, because we do not release the pPattern.
        CState* pPattern = createChangePattern(STATE_NUMBER, g_arrStatePins[nLightID], arrDurations);
        g_arrCurrentStatePointer[nLightID] = pPattern;
    }
}

// Given the state number, the control pin and duration of each state,
// alloc & initialize a single linked circular list for light-changing pattern.
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
// the dynamic memory alloced by initLight() is not released.
void setup() 
{
    Serial.begin(9600);
    
    initLight(g_arrCurrentStatePointer);
    
    // setup pin mode & turn off all PINs
	for (int i=2; i<14; ++i)
	{
        pinMode(i, OUTPUT);
        digitalWrite(i, (i>=8)? HIGH: LOW);
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
            digitalWrite(g_arrCurrentStatePointer[i]->nPin, HIGH);
            g_arrLastStateChangeTime[i] = g_nCurrentTime;
            
        }
        else if(g_nCurrentTime-g_arrLastStateChangeTime[i]>=g_arrCurrentStatePointer[i]->nDuration) // time to change
        {
            // turn off current PIN
            digitalWrite(g_arrCurrentStatePointer[i]->nPin, LOW);
            // move to the next state
            g_arrCurrentStatePointer[i] = g_arrCurrentStatePointer[i]->pNext;
            digitalWrite(g_arrCurrentStatePointer[i]->nPin, HIGH);
            // Serial.print(g_arrCurrentStatePointer[i]->nPin); Serial.print(",  ");
            // Serial.print(g_arrCurrentStatePointer[i]->nDuration); Serial.print(".\n");
            // update last change time
            g_arrLastStateChangeTime[i] = g_nCurrentTime;
            
        }
        else
        {
            // do nothing
        }
    }
}