// This code drives multiple LEDs with different frequencies.
// by Jason

const int LED_COUNT = 3;	// number of multi-color LEDs
const int COLOR_COUNT = 3;	// color number of each LED
const int ARR_COLOR_PIN[LED_COUNT][COLOR_COUNT] = { {2,3,4}, {5,6,7}, {8,9,10} };	// PIN of each color for each LED


void setup() {
	// turn all LEDs on
	for (int i=0; i<LED_COUNT; ++i)
	{
		for (int j=0; j<COLOR_COUNT; ++j)
		{
			pinMode(ARR_COLOR_PIN[i][j], OUTPUT);
			digitalWrite(ARR_COLOR_PIN[i][j], HIGH);
		}
	}
}

void loop() {

}