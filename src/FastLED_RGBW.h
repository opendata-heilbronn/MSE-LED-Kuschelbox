 /* 
 * Hack to enable SK6812 RGBW strips to work with FastLED.
 *
 * Original code by Jim Bumgardner (http://krazydad.com).
 * Modified by David Madison (http://partsnotincluded.com).
 * 
*/
 
#ifndef FastLED_RGBW_h
#define FastLED_RGBW_h
 
struct CRGBW  {
	union {
		struct {
			union {
				uint8_t g;
				uint8_t green;
			};
			union {
				uint8_t r;
				uint8_t red;
			};
			union {
				uint8_t b;
				uint8_t blue;
			};
			union {
				uint8_t w;
				uint8_t white;
			};
		};
		uint8_t raw[4];
		uint32_t raw32;
	};
 
	CRGBW(){}
 
	CRGBW(uint8_t rd, uint8_t grn, uint8_t blu, uint8_t wht){
		r = rd;
		g = grn;
		b = blu;
		w = wht;
	}
 
	inline void operator = (const CRGB c) __attribute__((always_inline)) { 
		this->r = c.r;
		this->g = c.g;
		this->b = c.b;
		this->white = 0;
	}

	inline CRGBW(const CRGB& c) __attribute__((always_inline)) {
		this->r = c.r;
		this->g = c.g;
		this->b = c.b;
		this->white = 0;
	}

	/// allow construction from 32-bit bit 0xWWRRGGBB color code
	inline CRGBW( uint32_t colorcode)  __attribute__((always_inline))
	: g((colorcode >> 8) & 0xFF), r((colorcode >> 16) & 0xFF), b((colorcode >> 0) & 0xFF), w((colorcode >> 24) & 0xFF) {
	}

	/// allow assignment from 32-bit 0xWWRRGGBB color code
	inline CRGBW& operator= (const uint32_t colorcode) __attribute__((always_inline)) {
		w = (colorcode >> 24) & 0xFF;
		r = (colorcode >> 16) & 0xFF;
		g = (colorcode >>  8) & 0xFF;
		b = (colorcode >>  0) & 0xFF;
		return *this;
	}

	// allow assignment and cast to 32-bit 0xWWRRGGBB color code
	inline operator uint32_t() const {
		uint32_t out;
		out |= (w << 24);
		out |= (r << 16);
		out |= (g <<  8);
		out |= (b <<  0);
		return out;
	}
};
 
inline uint16_t getRGBWsize(uint16_t nleds) {
	uint16_t nbytes = nleds * 4;
	if(nbytes % 3 > 0) return nbytes / 3 + 1;
	else return nbytes / 3;
}
 
#endif