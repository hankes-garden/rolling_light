// This code drives multiple lights with different changing patterns: 
// 1. modulates them with both chromatic and illuminance changes.
// 2. Synchronize btw colors (not periods).
// Coded by Lin Yang, CSE, HKUST.

#include "name.h"

const int LIGHT_COUNT = 2;  // light number
const int COLOR_COUNT = 3;  // color number in each light
const int BASE_FREQ = 97;   // base frequency for synchronization & luminance padding
                            // Note: this value must be less than or equal to all the color frequencies!
unsigned long g_nBasePeriod = 1000000/BASE_FREQ;  // base period

const int STATE_COUNT_L1 = 6;   // state number of light 1
const int STATE_COUNT_L2 = 6;   // state number of light 2

CState* g_arrCurrentState[LIGHT_COUNT] = {NULL, NULL};  // current state of each light
short g_arrStateChangeCount[LIGHT_COUNT] = {0,0};
unsigned long g_arrLastStateChangeTime [LIGHT_COUNT] = {0,0};  // last state change time of each light

unsigned long g_nCurrentTime = 0; // current time stamp.

CState* createChangePattern(int nChangeCount, int arrPins[], unsigned long arrDurations[]);
void initLights(CState* arrLightPatterns[]);

// intialize the state-changing pattern for each light
void initLights(CState* arrLightPatterns[])
{
    // light 1
    unsigned long nColorFreq_l1 = 103;   // color freq of light 1
    unsigned long nColorDuration_l1 = 1000000/nColorFreq_l1/COLOR_COUNT; // duration of each color
    unsigned long nDarkDuration_l1 = g_nBasePeriod/COLOR_COUNT - nColorDuration_l1; // duration of each dark padding
    float dDutyCycle_l1 = 1.0 - (nDarkDuration_l1*COLOR_COUNT*1.0/g_nBasePeriod);
    Serial.print("Dark duration of L1 is: "); Serial.print(nDarkDuration_l1); Serial.print(".\n");
    Serial.print("Duty cycle of L1 is: "); Serial.print(dDutyCycle_l1); Serial.print(".\n");
    int arrPins_l1 [STATE_COUNT_L1] = {2,0,3,0,4,0};
    unsigned long arrDurations_l1 [STATE_COUNT_L1] = {nColorDuration_l1, nDarkDuration_l1,\
                                                      nColorDuration_l1, nDarkDuration_l1,\
                                                      nColorDuration_l1, nDarkDuration_l1};
    CState* pPattern_l1 = createChangePattern(STATE_COUNT_L1, arrPins_l1, arrDurations_l1);
    g_arrCurrentState[0] = pPattern_l1; // start with the head state
    
    // light 2
    unsigned long nColorFreq_l2 = 107;    // color freq fo light 2
    unsigned long nColorDuration_l2 = 1000000/nColorFreq_l2/COLOR_COUNT;
    unsigned long nDarkDuration_l2 = g_nBasePeriod/COLOR_COUNT - nColorDuration_l2;
    float dDutyCycle_l2 = 1.0 - (nDarkDuration_l2*COLOR_COUNT*1.0/g_nBasePeriod);
    Serial.print("Dark duration of L2 is: "); Serial.print(nDarkDuration_l2); Serial.print(".\n");
    Serial.print("Duty cycle of L2 is: "); Serial.print(dDutyCycle_l2); Serial.print(".\n");
    int arrPins_l2 [STATE_COUNT_L2] = {5,0,6,0,7,0};
    unsigned long arrDurations_l2 [STATE_COUNT_L2] = {nColorDuration_l2, nDarkDuration_l2, \
                                                      nColorDuration_l2, nDarkDuration_l2, \
                                                      nColorDuration_l2, nDarkDuration_l2};
    CState* pPattern_l2 = createChangePattern(STATE_COUNT_L2, arrPins_l2, arrDurations_l2);
    g_arrCurrentState[1] = pPattern_l2;
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

// shuffle the changing pattern
CState* shufflePatttern(CState* pPattern, int nSize)
{
   // create a array for shuffle
   CState** array = new CState* [nSize];
   CState* pTemp = pPattern;
   // Serial.print("Previous order: ");
   for(int i=0; i<nSize; ++i)
   {
       array[i] = pTemp;
       // Serial.print(pTemp->nPin); Serial.print(", ");
       
       pTemp = pTemp->pNext;
       
   }
   // Serial.print("\n");
   

   // shuffle array
   for(int i=nSize-1; i>0; --i)
   {
        int j = rand()%i;
        CState* pData = array[i];
        array[i] = array[j];
        array[j] = pData;
   }
   
   // re-order the linked list according to shuffled array.
   // Serial.print("Shuffled order: ");
   for (int i=0; i<nSize; ++i)
   {
       // Serial.print(array[i]->nPin); Serial.print(", ");
       array[i]->pNext = array[(i+1)%nSize];
   }
   // Serial.print("\n");
   // Serial.print("\n");

   CState* pNewHead = array[0];
   delete[] array;

   return pNewHead;
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
            
            ++g_arrStateChangeCount[i];
            if(g_arrStateChangeCount[i]==STATE_COUNT_L1)//time to shuffle pattern!
            {
                g_arrCurrentState[i] = shufflePatttern(g_arrCurrentState[i], STATE_COUNT_L1);
                g_arrStateChangeCount[i] = 0;
            }
            
        }
        else
        {
            // do nothing
        }
    }
}