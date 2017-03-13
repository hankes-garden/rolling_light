// This code drives multiple LEDs with different frequencies.
// by Jason


void setup() {
	// turn all LEDs on
	for (int i=2; i<14; ++i)
	{
        pinMode(i, OUTPUT);
        digitalWrite(i, HIGH);
	}
}

void loop() {

}