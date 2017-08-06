/*
 * LedStripController.h
 *
 *  Created on: 2017. aug. 5.
 *      Author: peti
 */

#ifndef LEDSTRIPCONTROLLER_H_
#define LEDSTRIPCONTROLLER_H_

#include <stm32f10x.h>

class LedStripController {
public:
	LedStripController(GPIO_TypeDef const* dataOutGpioPort, uint16_t dataOutGpioPin);
	void init();
	void writeLeds();
private:
	GPIO_TypeDef const* dataOutGpioPort;
	uint16_t dataOutGpioPin;
	uint32_t bitSetResetReg;
	void rccInit();
	void dmaInit();
	void timerInit();
	void gpioInit();
};

#endif /* LEDSTRIPCONTROLLER_H_ */
