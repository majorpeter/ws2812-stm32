#ifndef LEDSTRIPCONTROLLERNODE_H_
#define LEDSTRIPCONTROLLERNODE_H_

#include <mprotocol-nodes/Node.h>

class LedStripController;
class Color;

class LedStripControllerNode : public Node {
public:
    DECLARE_PROP_BINARY_RW(ColorHex);
    DECLARE_PROP_STRING_RW(ColorRgb);

    ProtocolResult_t getColorBytes(AbstractPacketInterface* packetInterface);
    class SetColorBytes: public AbstractPacketInterface {
    public:
        inline SetColorBytes(LedStripControllerNode* that) : that(that) {}

        virtual bool startTransaction();
        virtual bool transmitData(const uint8_t *data, uint16_t length);
        virtual bool commitTransaction();
        virtual void cancelTransaction();

        friend class LedStripControllerNode;
    private:
        LedStripControllerNode* that;
    };
    SetColorBytes setColorBytes;
    static const Property_t prop_ColorBytes;

    DECLARE_PROP_METHOD(clear);

    LedStripControllerNode(LedStripController* controller);
    virtual ~LedStripControllerNode() {}
private:
    LedStripController* controller;
    Color* colors;
    uint8_t rgbBytes[3];
    uint16_t byteIndex;
};

#endif /* LEDSTRIPCONTROLLERNODE_H_ */
