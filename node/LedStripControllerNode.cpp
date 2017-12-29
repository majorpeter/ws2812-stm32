#include "LedStripControllerNode.h"
#include "../LedStripController.h"
#include "../Color.h"

#include <string.h>
#include <stdio.h>

MK_PROP_BINARY_RW(LedStripControllerNode, ColorHex, "Current color in hexadecimal digits");
MK_PROP_STRING_RW(LedStripControllerNode, ColorRgb, "Current color as RGB decimal values");
const Property_t LedStripControllerNode::prop_ColorBytes = {
    "ColorBytes", "Current color values for each LED.",
    PropertyType_BinarySegmented, PropAccessLevel_ReadWrite,
    (void*) &LedStripControllerNode::getColorBytes,
    &((LedStripControllerNode*)NULL)->setColorBytes,
    ((size_t)(Node*)(LedStripControllerNode*) 1) - 1
};
MK_PROP_METHOD(LedStripControllerNode, clear, "Disable all LED's");

PROP_ARRAY(props) = {
        PROP_ADDRESS(LedStripControllerNode, ColorHex),
        PROP_ADDRESS(LedStripControllerNode, ColorRgb),
        PROP_ADDRESS(LedStripControllerNode, ColorBytes),
        PROP_ADDRESS(LedStripControllerNode, clear)
};

LedStripControllerNode::LedStripControllerNode(LedStripController* controller): Node("LEDSTRIP"), setColorBytes(this), controller(controller) {
    NODE_SET_PROPS(props);

    colors = new Color[controller->getLedCount()];
    memset(rgbBytes, 0x00, sizeof(rgbBytes));
    byteIndex = 0;
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
    invalidateProperty(&prop_ColorBytes);
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
    invalidateProperty(&prop_ColorBytes);
    return ProtocolResult_Ok;
}

ProtocolResult_t LedStripControllerNode::getColorBytes(AbstractPacketInterface *packetInterface) {
    if (!packetInterface->startTransaction()) {
        return ProtocolResult_InternalError;
    }

    // bytes by 3
    uint8_t buffer[3];
    for (uint16_t i = 0; i < controller->getLedCount(); i++) {
        buffer[0] = colors[i].getRed();
        buffer[1] = colors[i].getGreen();
        buffer[2] = colors[i].getBlue();
        bool result = packetInterface->transmitData(buffer, 3);
        if (result == false) {
            packetInterface->cancelTransaction();
            return ProtocolResult_InternalError;
        }
    }

    if (!packetInterface->commitTransaction()) {
        packetInterface->cancelTransaction();
        return ProtocolResult_InternalError;
    }

    return ProtocolResult_Ok;
}

bool LedStripControllerNode::SetColorBytes::startTransaction() {
    that->byteIndex = 0;
    return true;
}

bool LedStripControllerNode::SetColorBytes::transmitData(const uint8_t *data, uint16_t length) {
    uint16_t byteIndexAfter = that->byteIndex + length;
    if (byteIndexAfter > that->controller->getLedCount() * 3) {
        return false;
    }

    while (length > 0) {
        Color& c = that->colors[that->byteIndex / 3];
        switch (that->byteIndex % 3) {
        case 0:
            c.setRed(*data);
            break;
        case 1:
            c.setGreen(*data);
            break;
        case 2:
            c.setBlue(*data);
            break;
        }

        that->byteIndex++;
        data++;

        length--;
    }

    return true;
}

bool LedStripControllerNode::SetColorBytes::commitTransaction() {
    that->controller->writeLeds(that->colors, that->controller->getLedCount());
    return true;
}

void LedStripControllerNode::SetColorBytes::cancelTransaction() {
}

ProtocolResult_t LedStripControllerNode::invokeclear(const char*) {
    for (uint16_t i = 0; i < controller->getLedCount(); i++) {
        colors[i] = Color::black;
    }
    controller->writeLeds(colors, controller->getLedCount());
    return ProtocolResult_Ok;
}
