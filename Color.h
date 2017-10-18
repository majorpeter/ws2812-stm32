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
    static const uint8_t maxChannelValue = 255;

    static const Color black;
    static const Color white;
    static const Color red;
    static const Color green;
    static const Color blue;

    Color();
    Color(uint8_t red, uint8_t green, uint8_t blue);
    Color(const Color& c);
    ~Color() {}

    Color& operator+=(const Color& c);
    Color operator*(float v) const;

    inline Color& operator=(const Color& c) {
        color.value = c.color.value;
        return *this;
    }
    inline bool operator==(const Color& c) const {
        return (color.value & 0xffffff) == (c.color.value & 0xffffff);
    }
    inline uint8_t getRed() const {
        return color.components.red;
    }
    inline uint8_t getGreen() const {
        return color.components.green;
    }
    inline uint8_t getBlue() const {
        return color.components.blue;
    }
private:
    union color_ {
        uint32_t value;
        struct {
            uint8_t blue;
            uint8_t green;
            uint8_t red;
            uint8_t _reserved;
        } components;
    } color;
};

#endif /* COLOR_H_ */
