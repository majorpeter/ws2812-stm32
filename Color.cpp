/*
 * Color.cpp
 *
 *  Created on: 2017. aug. 6.
 *      Author: peti
 */

#include "Color.h"

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
