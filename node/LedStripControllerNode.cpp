#include "LedStripControllerNode.h"
#include "../LedStripController.h"
#include "../Color.h"

#include <string.h>
#include <stdio.h>

MK_PROP_BINARY_RW(LedStripControllerNode, ColorHex, "Current color in hexadecimal digits");
MK_PROP_STRING_RW(LedStripControllerNode, ColorRgb, "Current color as RGB decimal values");
MK_PROP_METHOD(LedStripControllerNode, clear, "Disable all LED's");

PROP_ARRAY(props) = {
        PROP_ADDRESS(LedStripControllerNode, ColorHex),
        PROP_ADDRESS(LedStripControllerNode, ColorRgb),
        PROP_ADDRESS(LedStripControllerNode, clear)
};

LedStripControllerNode::LedStripControllerNode(LedStripController* controller): Node("LEDSTRIP"), controller(controller) {
    NODE_SET_PROPS(props);

    colors = new Color[controller->getLedCount()];
    memset(rgbBytes, 0x00, sizeof(rgbBytes));
}

ProtocolResult_t LedStripControllerNode::getColorHex(const void** dest, uint16_t* length) const {
    *dest = rgbBytes;
    *length = sizeof(rgbBytes);
    return ProtocolResult_Ok;
}

ProtocolResult_t LedStripControllerNode::setColorHex(const void* value, uint16_t length) {
    if (length != sizeof(rgbBytes)) {
        return ProtocolResult_InvalidValue;
    }
    const uint8_t* bytes = static_cast<const uint8_t*>(value);
    rgbBytes[0] = bytes[0];
    rgbBytes[1] = bytes[1];
    rgbBytes[2] = bytes[2];

    Color c(rgbBytes[0], rgbBytes[1], rgbBytes[2]);
    for (uint16_t i = 0; i < controller->getLedCount(); i++) {
        colors[i] = c;
    }
    controller->writeLeds(colors, controller->getLedCount());

    invalidateProperty(&prop_ColorRgb);
    return ProtocolResult_Ok;
}

ProtocolResult_t LedStripControllerNode::getColorRgb(char* dest) const {
    sprintf(dest, "%d,%d,%d", (int) rgbBytes[0], (int) rgbBytes[1], (int) rgbBytes[2]);
    return ProtocolResult_Ok;
}

ProtocolResult_t LedStripControllerNode::setColorRgb(const char* value) {
    int r, g, b;
    if (sscanf(value, "%d,%d,%d", &r, &g, &b) != 3) {
        return ProtocolResult_InvalidValue;
    }
    rgbBytes[0] = r;
    rgbBytes[1] = g;
    rgbBytes[2] = b;

    Color c(r, g, b);
    for (uint16_t i = 0; i < controller->getLedCount(); i++) {
        colors[i] = c;
    }
    controller->writeLeds(colors, controller->getLedCount());

    invalidateProperty(&prop_ColorHex);
    return ProtocolResult_Ok;
}

ProtocolResult_t LedStripControllerNode::invokeclear(const char*) {
    for (uint16_t i = 0; i < controller->getLedCount(); i++) {
        colors[i] = Color::black;
    }
    controller->writeLeds(colors, controller->getLedCount());
    return ProtocolResult_Ok;
}
