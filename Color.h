/*
 * Color.h
 *
 *  Created on: 2017. aug. 6.
 *      Author: peti
 */

#ifndef COLOR_H_
#define COLOR_H_

#include <stdint.h>

class Color {
public:
	static const Color black;
	static const Color red;
	static const Color green;
	static const Color blue;

	Color();
	Color(uint8_t red, uint8_t green, uint8_t blue);
	Color(const Color& c);
	~Color() {}

	inline Color& operator=(const Color& c) {
		color.value = c.color.value;
		return *this;
	}
	inline uint8_t getRed() const {
		return color.components.r;
	}
	inline uint8_t getGreen() const {
		return color.components.g;
	}
	inline uint8_t getBlue() const {
		return color.components.b;
	}
private:
	union color_ {
		uint32_t value;
		struct {
			uint8_t b;
			uint8_t g;
			uint8_t r;
			uint8_t _reserved;
		} components;
	} color;
};

#endif /* COLOR_H_ */
