#include <iostream>
#include "Led.hpp"

//#define ARDUINO 100
//#define __AVR_ATmega328P__

#include <Adafruit_NeoPixel.h>

int main()
{
	std::cout << "Test.Led.cpp" << std::endl;
	Adafruit_NeoPixel Leds = Adafruit_NeoPixel(2, 1, NEO_GRB + NEO_KHZ800);
	harpi::Led aLed(0, Leds);
	return 0;
}
