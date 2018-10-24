#include "Adafruit_NeoPixel.h"

#include <cassert>

Adafruit_NeoPixel::Adafruit_NeoPixel(
		uint16_t number,
		uint8_t pin,
		uint8_t mode)
	: m_number(number)
{
}

void Adafruit_NeoPixel::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
	assert(n <= m_number);
}
