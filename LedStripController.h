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
	static const uint16_t maxLedCount = 60;

	LedStripController(GPIO_TypeDef const* dataOutGpioPort, uint16_t dataOutGpioPin);
	void init();
	void writeLeds(const Color* colors, uint16_t ledCount);
private:
	GPIO_TypeDef const* dataOutGpioPort;
	uint16_t dataOutGpioPin;
	/// the value that can be written to the GPIO port's BSRR or BRR (DMA transfer destination)
	uint32_t bitSetResetReg;
	/// each element of this array represents one bit of the DMA transfer and is transmitted to the port's BRR
	uint16_t ledBits[maxLedCount * 24];

	void rccInit();
	void dmaInit();
	void timerInit();
	void gpioInit();
	void colorToBitMask(const Color color, uint16_t* maskBase);
	void byteToBitMask(uint8_t byte, uint16_t* maskBase);
};

#endif /* LEDSTRIPCONTROLLER_H_ */
