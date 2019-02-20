#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthSimpleDrum     drum1;          //xy=621.0057601928711,403.005735874176
AudioFilterStateVariable filter1;        //xy=817.0039348602295,434.0039119720459
AudioMixer4              outputMix;         //xy=988.0313682556152,417.53136444091797
AudioOutputUSB           usb1;           //xy=1201.031265258789,382.0312490463257
AudioOutputI2S           i2s1;           //xy=1216.00390625,466.00390625
AudioConnection          patchCord1(drum1, 0, filter1, 0);
AudioConnection          patchCord2(filter1, 0, outputMix, 0);
AudioConnection          patchCord3(outputMix, 0, usb1, 0);
AudioConnection          patchCord4(outputMix, 0, usb1, 1);
AudioConnection          patchCord5(outputMix, 0, i2s1, 0);
AudioConnection          patchCord6(outputMix, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=669.00390625,820.00390625
// GUItool: end automatically generated code

#include "pcf8591.h"
#include <Adafruit_MCP23017.h>
#include <Metro.h>

#define BTN 33
#define PCF 0x48
Adafruit_SSD1306 display(-1);
Adafruit_MCP23017 mcp;
Bounce trig = Bounce();
struct analog_readings anal;
Metro anal_read(42);

void setup()
{
	AudioMemory(15);
	sgtl5000_1.enable();
    sgtl5000_1.volume(0.65);
  	Serial.begin(9600);
  	mcp.begin();
  	mcp.pinMode(2,OUTPUT);
  	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  	display.clearDisplay();
  	display.setTextSize(1);
  	display.setTextColor(WHITE);
  	display.setCursor(0,0);
  	display.print("Start...");
  	display.display();
  	
  	trig.attach(BTN, INPUT_PULLUP);
  	trig.interval(25);

  	AudioNoInterrupts();
	drum1.frequency(120);
	drum1.length(150);
	drum1.pitchMod(0.1);
	outputMix.gain(0, 0.8);
	filter1.frequency(220);
	AudioInterrupts();
  	
  	delay(150);

  	anal.ain0.setSnapMultiplier(0.1);
  	anal.ain0.enableSleep();
  	anal.ain1.setSnapMultiplier(0.1);
  	anal.ain1.enableSleep();
  	anal.ain2.setSnapMultiplier(0.1);
  	anal.ain2.enableSleep();
  	anal.ain3.setSnapMultiplier(0.1);
  	anal.ain3.enableSleep();

  	pcf_read_all(PCF, &anal);
}

void loop()
{
	trig.update();
	if (anal_read.check() == 1)
	{
		if (pcf_read_all(PCF, &anal))
		{
			update_display(display);
			filter1.frequency(map(anal.ain3.getValue(), 0, 255, 15, 300));
			drum1.frequency(map(anal.ain0.getValue(), 0, 255, 15, 300));
			drum1.pitchMod(anal.ain1.getValue() / 255.0f);
			drum1.length(map(anal.ain2.getValue(), 0, 255, 1, 1000));
		}
	}

	if (trig.fell() || trig.rose())
	{
		
		if (trig.read() == LOW)
			drum1.noteOn();

		update_display(display);

	}
}