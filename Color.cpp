/*
 * Color.cpp
 *
 *  Created on: 2017. aug. 6.
 *      Author: peti
 */

#include "Color.h"

const Color Color::black(0, 0, 0);
const Color Color::red(255, 0, 0);
const Color Color::green(0, 255, 0);
const Color Color::blue(0, 0, 255);

Color::Color() {
	color.value = 0;
}

Color::Color(uint8_t red, uint8_t green, uint8_t blue) {
	color.components.r = red;
	color.components.g = green;
	color.components.b = blue;
}

Color::Color(const Color& c) {
	this->color.value = c.color.value;
}
