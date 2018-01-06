#ifndef LEDSTRIPCONTROLLERNODE_H_
#define LEDSTRIPCONTROLLERNODE_H_

#include <mprotocol-nodes/Node.h>

class LedStripController;
class Color;

class LedStripControllerNode : public Node {
public:
    DECLARE_PROP_BINARY_RW(ColorHex);
    DECLARE_PROP_STRING_RW(ColorRgb);
    DECLARE_PROP_BINARY_SEGMENTED_RW(ColorBytes, LedStripControllerNode);
    DECLARE_PROP_METHOD(clear);

    LedStripControllerNode(LedStripController* controller);
    virtual ~LedStripControllerNode() {}
private:
    LedStripController* controller;
    Color* colors;
    uint8_t rgbBytes[3];
    uint16_t byteIndex;

    void clearColorsBuffer();
};

#endif /* LEDSTRIPCONTROLLERNODE_H_ */
