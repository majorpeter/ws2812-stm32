/*
 * LedStripController.h
 *
 *  Created on: 2017. aug. 5.
 *      Author: peti
 */

#ifndef LEDSTRIPCONTROLLER_H_
#define LEDSTRIPCONTROLLER_H_

#include <stm32f10x.h>

class Color;

class LedStripController {
public:
    LedStripController(GPIO_TypeDef * const dataOutGpioPort,
            uint16_t dataOutGpioPin, uint16_t maxLedCount,
            uint16_t ledIndexOffset, bool reverse);
    ~LedStripController();

    void init();
    void writeLeds(const Color* colors, uint16_t ledCount);

    inline uint16_t getLedCount() const {
        return maxLedCount;
    }
private:
    GPIO_TypeDef const* dataOutGpioPort;
    uint16_t dataOutGpioPin;
    const uint16_t maxLedCount;
    uint16_t ledIndexOffset;
    bool reverse;
    /// the value that can be written to the GPIO port's BSRR or BRR (DMA transfer destination)
    uint32_t bitSetResetReg;
    /// each element of this array represents one bit of the DMA transfer and is transmitted to the port's BRR
    uint16_t* ledBits;

    void rccInit();
    void dmaInit();
    void timerInit();
    void gpioInit();
    void colorToBitMask(const Color color, uint16_t* maskBase);
    void byteToBitMask(uint8_t byte, uint16_t* maskBase);
};

#endif /* LEDSTRIPCONTROLLER_H_ */
