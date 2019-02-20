#ifndef _pcf8519_h_
#define _pcf8519_h_

#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <ResponsiveAnalogRead.h>

#define AUTOINCREMENT_READ B00000100
#define OUTPUT_MASK B01000000
#define DISABLE_OUTPUT B01000000
#define SINGLE_ENDED_INPUT B00000000
#define TREE_DIFFERENTIAL_INPUT B00010000
#define TWO_SINGLE_ONE_DIFFERENTIAL_INPUT B00100000
#define TWO_DIFFERENTIAL_INPUT B00110000
char buff[32];

struct analog_readings
{
	ResponsiveAnalogRead ain0;
	ResponsiveAnalogRead ain1;
	ResponsiveAnalogRead ain2;
	ResponsiveAnalogRead ain3;
};

extern struct analog_readings anal;
extern Bounce trig;

uint8_t pcf_read(int addr, uint8_t pin)
{
	Wire1.beginTransmission(addr);
	byte operation = pin | SINGLE_ENDED_INPUT| (DISABLE_OUTPUT&OUTPUT_MASK);
    Wire1.write(operation);
    Wire1.endTransmission ();
    Wire1.requestFrom (addr, 2);
    Wire1.read ();
    return (Wire1.read());
}

bool pcf_read_all(int addr, struct analog_readings *output)
{
	Wire1.beginTransmission(addr);
	byte operation = AUTOINCREMENT_READ | SINGLE_ENDED_INPUT | (DISABLE_OUTPUT&OUTPUT_MASK);
	Wire1.write(operation);
	Wire1.endTransmission();
	Wire1.requestFrom(addr, 5);
	Wire1.read();

	output->ain0.update(Wire1.read());
	output->ain1.update(Wire1.read());
	output->ain2.update(Wire1.read());
	output->ain3.update(Wire1.read());

	return (output->ain0.hasChanged() || 
		output->ain1.hasChanged() || 
		output->ain2.hasChanged() || 
		output->ain3.hasChanged());
}

void update_display(Adafruit_SSD1306 &display)
{
	display.clearDisplay();
	display.setCursor(0,0);
	sprintf(buff, "%03d %03d %03d %03d", 
		anal.ain0.getValue(),
		anal.ain1.getValue(),
		anal.ain2.getValue(),
		anal.ain3.getValue());

	display.print(buff);
	display.fillRect(128-16,32-16,16,16, (trig.read() == LOW) ? WHITE : BLACK);
	display.display();
}

#endif