#ifndef FAKE_ADAFRUIT_NEO_PIXEL
#define FAKE_ADAFRUIT_NEO_PIXEL

#include <Arduino.h>

// RGB NeoPixel permutations; white and red offsets are always same
// Offset:         W          R          G          B   // HEX representation
#define NEO_RGB  ((0 << 6) | (0 << 4) | (1 << 2) | (2)) // 0x06
#define NEO_RBG  ((0 << 6) | (0 << 4) | (2 << 2) | (1)) // 0x09
#define NEO_GRB  ((1 << 6) | (1 << 4) | (0 << 2) | (2)) // 0x52
#define NEO_GBR  ((2 << 6) | (2 << 4) | (0 << 2) | (1)) // 0xA1
#define NEO_BRG  ((1 << 6) | (1 << 4) | (2 << 2) | (0)) // 0x58
#define NEO_BGR  ((2 << 6) | (2 << 4) | (1 << 2) | (0)) // 0xA4

// RGBW NeoPixel permutations; all 4 offsets are distinct
// Offset:         W          R          G          B   // HEX representation
#define NEO_WRGB ((0 << 6) | (1 << 4) | (2 << 2) | (3)) // 0x1B
#define NEO_WRBG ((0 << 6) | (1 << 4) | (3 << 2) | (2)) // 0x1E
#define NEO_WGRB ((0 << 6) | (2 << 4) | (1 << 2) | (3)) // 0x27
#define NEO_WGBR ((0 << 6) | (3 << 4) | (1 << 2) | (2)) // 0x36
#define NEO_WBRG ((0 << 6) | (2 << 4) | (3 << 2) | (1)) // 0x2D
#define NEO_WBGR ((0 << 6) | (3 << 4) | (2 << 2) | (1)) // 0x39

#define NEO_RWGB ((1 << 6) | (0 << 4) | (2 << 2) | (3)) // 0x4B
#define NEO_RWBG ((1 << 6) | (0 << 4) | (3 << 2) | (2)) // 0x4E
#define NEO_RGWB ((2 << 6) | (0 << 4) | (1 << 2) | (3)) // 0x87
#define NEO_RGBW ((3 << 6) | (0 << 4) | (1 << 2) | (2)) // 0xC6
#define NEO_RBWG ((2 << 6) | (0 << 4) | (3 << 2) | (1)) // 0x8D
#define NEO_RBGW ((3 << 6) | (0 << 4) | (2 << 2) | (1)) // 0xC9

#define NEO_GWRB ((1 << 6) | (2 << 4) | (0 << 2) | (3)) // 0x63
#define NEO_GWBR ((1 << 6) | (3 << 4) | (0 << 2) | (2)) // 0x72
#define NEO_GRWB ((2 << 6) | (1 << 4) | (0 << 2) | (3)) // 0x93
#define NEO_GRBW ((3 << 6) | (1 << 4) | (0 << 2) | (2)) // 0xD2
#define NEO_GBWR ((2 << 6) | (3 << 4) | (0 << 2) | (1)) // 0xB1
#define NEO_GBRW ((3 << 6) | (2 << 4) | (0 << 2) | (1)) // 0xE1

#define NEO_BWRG ((1 << 6) | (2 << 4) | (3 << 2) | (0)) // 0x6C
#define NEO_BWGR ((1 << 6) | (3 << 4) | (2 << 2) | (0)) // 0x78
#define NEO_BRWG ((2 << 6) | (1 << 4) | (3 << 2) | (0)) // 0x9C
#define NEO_BRGW ((3 << 6) | (1 << 4) | (2 << 2) | (0)) // 0xD8
#define NEO_BGWR ((2 << 6) | (3 << 4) | (1 << 2) | (0)) // 0xB4
#define NEO_BGRW ((3 << 6) | (2 << 4) | (1 << 2) | (0)) // 0xE4

#define NEO_KHZ800 0x0000 // 800 KHz datastream

class Adafruit_NeoPixel
{
public:
	Adafruit_NeoPixel(
			uint16_t number,
			uint8_t pin=6,
			uint8_t mode=NEO_GRB + NEO_KHZ800);

    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);

private:
	uint16_t m_number;
};

#endif // #ifndef FAKE_ADAFRUIT_NEO_PIXEL
