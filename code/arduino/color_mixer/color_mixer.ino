// This script tune the input voltage for each color by using PWM.
#include <stdio.h>
const int SERIAL_BAUD_RATE = 9600;

const int PIN_RED = 3;
const int PIN_GREEN = 5;
const int PIN_BLUE = 6;
const int PIN_WHILE = 9;

int nRedVol = 127;
int nGreenVol = 254;
int nBlueVol = 43;
int nWhiteVol = 178;

const byte INPUT_BUF_SIZE = 32;
char arrInputBuf[INPUT_BUF_SIZE];
char * strtokIndx; 						// this is used by strtok() as an index

char arrMsg [32]={0};

boolean newData = false;

void setup() {
	Serial.begin(SERIAL_BAUD_RATE); // opens serial port, sets data rate to 9600 bps
	
	analogWrite(PIN_RED, nRedVol);
	analogWrite(PIN_GREEN, nGreenVol);
	analogWrite(PIN_BLUE, nBlueVol);

}

void loop() {
	recvWithStartEndMarkers();
    if (newData == true) {
        parseData();
		
		// set RGB
		sprintf(arrMsg, "Set RGB to (%d, %d, %d).", nRedVol, nGreenVol, nBlueVol);
		Serial.println(arrMsg);
		
		analogWrite(PIN_RED, nRedVol);
		analogWrite(PIN_GREEN, nGreenVol);
		analogWrite(PIN_BLUE, nBlueVol);
		
        newData = false;
    }
	
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '(';
    char endMarker = ')';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                arrInputBuf[ndx] = rc;
                ndx++;
                if (ndx >= INPUT_BUF_SIZE) {
                    ndx = INPUT_BUF_SIZE - 1;
                }
            }
            else {
                arrInputBuf[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

void parseData() {

    strtokIndx = strtok(arrInputBuf,",");
	nRedVol = min(atoi(strtokIndx), 255);
 
    strtokIndx = strtok(NULL, ","); 
	nGreenVol = min(atoi(strtokIndx), 255);

    strtokIndx = strtok(NULL, ",");
	nBlueVol = min(atoi(strtokIndx), 255);
}

