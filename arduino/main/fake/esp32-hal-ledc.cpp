#include "esp32-hal-ledc.h"

#include <iostream>

double      ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits)
{
	std::clog << "ledcSetup(" << static_cast< uint16_t >(channel) << ", " << freq << ", " << static_cast< uint16_t >(resolution_bits) << ")\n";
	return 0;
}

void        ledcWrite(uint8_t channel, uint32_t duty)
{
	std::clog << "ledcWrite(" << static_cast< uint16_t >(channel) << ", " << duty << ")\n";
}

double      ledcWriteTone(uint8_t channel, double freq)
{
	std::clog << "ledcWriteTone(" << static_cast< uint16_t >(channel) << ", " << freq << ")\n";
	return 0;
}

double      ledcWriteNote(uint8_t channel, note_t note, uint8_t octave)
{
	std::clog << "ledcWrite(" << static_cast< uint16_t >(channel) << ", " << note << ", " << static_cast< uint16_t >(octave) << ")\n";
	return 0;
}

uint32_t    ledcRead(uint8_t channel)
{
	std::clog << "ledcRead(" << static_cast< uint16_t >(channel) << ")\n";
	return 0;
}

double      ledcReadFreq(uint8_t channel)
{
	std::clog << "ledcReadFreq(" << static_cast< uint16_t >(channel) << ")\n";
	return 0;
}

void        ledcAttachPin(uint8_t pin, uint8_t channel)
{
	std::clog << "ledcAttachPin(" << static_cast< uint16_t >(pin) << ", " << static_cast< uint16_t >(channel) << ")\n";
}

void        ledcDetachPin(uint8_t pin)
{
	std::clog << "ledcDetachPin(" << static_cast< uint16_t >(pin) << ")\n";
}
