/*
 * LedStripController.cpp
 *
 *  Created on: 2017. aug. 5.
 *      Author: peti
 */

#include "LedStripController.h"
#include "Color.h"

volatile bool dmaBusy = false;

LedStripController::LedStripController(GPIO_TypeDef * const dataOutGpioPort,
        uint16_t dataOutGpioPin, uint16_t ledIndexOffset, bool reverse) :
        dataOutGpioPort(dataOutGpioPort), dataOutGpioPin(dataOutGpioPin),
        ledIndexOffset(ledIndexOffset), reverse(reverse) {
    bitSetResetReg = dataOutGpioPin;
}

void LedStripController::init() {
    this->rccInit();
    this->dmaInit();
    this->timerInit();
    this->gpioInit();
}

void LedStripController::writeLeds(const Color* colors, uint16_t ledCount) {
    while (dmaBusy);

    dmaBusy = true;
    if (ledCount > maxLedCount) {
        ledCount = maxLedCount;
    }

    for (uint16_t inputDataIndex = 0; inputDataIndex < maxLedCount; inputDataIndex++) {
        int16_t outputDataIndex = ledIndexOffset + (reverse ? -inputDataIndex : inputDataIndex);
        if (outputDataIndex >= maxLedCount) {
            outputDataIndex -= maxLedCount;
        } else if (outputDataIndex < 0) {
            outputDataIndex += maxLedCount;
        }
        if (inputDataIndex < ledCount) {
            this->colorToBitMask(colors[inputDataIndex], &ledBits[outputDataIndex * 24]);
        } else {
            this->colorToBitMask(Color(0,0,0), &ledBits[outputDataIndex * 24]);
        }
    }

    // clear all relevant DMA flags
    DMA_ClearFlag(DMA1_FLAG_TC2 | DMA1_FLAG_HT2 | DMA1_FLAG_GL2 | DMA1_FLAG_TE2);
    DMA_ClearFlag(DMA1_FLAG_TC5 | DMA1_FLAG_HT5 | DMA1_FLAG_GL5 | DMA1_FLAG_TE5);
    DMA_ClearFlag(DMA1_FLAG_HT7 | DMA1_FLAG_GL7 | DMA1_FLAG_TE7);

    // configure the number of bytes to be transferred by the DMA controller
    DMA_SetCurrDataCounter(DMA1_Channel2, maxLedCount * 24);
    DMA_SetCurrDataCounter(DMA1_Channel5, maxLedCount * 24);
    DMA_SetCurrDataCounter(DMA1_Channel7, maxLedCount * 24);

    // clear all TIM2 flags
    TIM2->SR = 0;

    // enable the corresponding DMA channels
    DMA_Cmd(DMA1_Channel2, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
    DMA_Cmd(DMA1_Channel7, ENABLE);

    // IMPORTANT: enable the TIM2 DMA requests AFTER enabling the DMA channels!
    TIM_DMACmd(TIM2, TIM_DMA_CC1, ENABLE);
    TIM_DMACmd(TIM2, TIM_DMA_CC2, ENABLE);
    TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);

    // preload counter with 29 so TIM2 generates UEV directly to start DMA transfer
    TIM_SetCounter(TIM2, 29);

    // start TIM2
    TIM_Cmd(TIM2, ENABLE);
}

void LedStripController::rccInit() {
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void LedStripController::dmaInit() {
    DMA_InitTypeDef DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);

    // TIM2 Update event
    // DMA1 Channel2
    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(&dataOutGpioPort->BSRR);
    DMA_InitStructure.DMA_MemoryBaseAddr = reinterpret_cast<uint32_t>(&bitSetResetReg);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    // TIM2 CC1 event
    // DMA1 Channel5
    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(&dataOutGpioPort->BRR);
    DMA_InitStructure.DMA_MemoryBaseAddr = reinterpret_cast<uint32_t>(this->ledBits);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);

    // TIM2 CC2 event
    // DMA1 Channel7
    DMA_DeInit(DMA1_Channel7);
    DMA_InitStructure.DMA_PeripheralBaseAddr = reinterpret_cast<uint32_t>(&dataOutGpioPort->BRR);
    DMA_InitStructure.DMA_MemoryBaseAddr = reinterpret_cast<uint32_t>(&bitSetResetReg);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 1;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
}

void LedStripController::timerInit() {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_OCStructInit(&TIM_OCInitStructure);

    // Base
    TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 24000000) - 1; //24MHz
    TIM_TimeBaseStructure.TIM_Period = 30-1; // 800kHz, 1250ns
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ARRPreloadConfig(TIM2, DISABLE);

    // Channel 1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 9-1; // 375ns
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

    // Channel 2
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 18-1; // 750ns
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
}

void LedStripController::gpioInit() {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = dataOutGpioPin;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init((GPIO_TypeDef*) dataOutGpioPort, &GPIO_InitStruct);
}

// G7 G6 G5 G4 G3 G2 G1 G0 R7 R6 R5 R4 R3 R2 R1 R0 B7 B6 B5 B4 B3 B2 B1 B0
void LedStripController::colorToBitMask(const Color color, uint16_t* maskBase) {
    this->byteToBitMask(color.getGreen(), maskBase);
    this->byteToBitMask(color.getRed(), maskBase + 8);
    this->byteToBitMask(color.getBlue(), maskBase + 16);
}

void LedStripController::byteToBitMask(uint8_t byte, uint16_t* maskBase) {
    uint8_t mask = 0x80;
    while (mask != 0) {
        if ((mask & byte) != 0x00) {
            *maskBase = 0;
        } else {
            *maskBase = dataOutGpioPin;
        }
        maskBase++;
        mask >>= 1;
    }
}

extern "C"
void DMA1_Channel7_IRQHandler(void){

    // clear DMA7 transfer complete interrupt flag
    DMA_ClearITPendingBit(DMA1_IT_TC7);

    // enable TIM2 Update interrupt to append 50us dead period
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    // disable the DMA channels
    DMA_Cmd(DMA1_Channel2, DISABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
    DMA_Cmd(DMA1_Channel7, DISABLE);

    // IMPORTANT: disable the DMA requests, too!
    TIM_DMACmd(TIM2, TIM_DMA_CC1, DISABLE);
    TIM_DMACmd(TIM2, TIM_DMA_CC2, DISABLE);
    TIM_DMACmd(TIM2, TIM_DMA_Update, DISABLE);

}

extern "C"
void TIM2_IRQHandler(void){
    static uint8_t overflows = 0;
    static const uint8_t deadperiods = 50;

    // Clear TIM2 Interrupt Flag
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    /* check if certain number of overflows has occured yet
     * this ISR is used to guarantee a >= 50us dead time on the data lines
     * before another frame is transmitted */
    if (overflows < deadperiods){
        // count the number of occured overflows
        overflows++;
    }
    else{
        // clear the number of overflows
        overflows = 0;
        // stop TIM2 now because dead period has been reached
        TIM_Cmd(TIM2, DISABLE);
        // disable the TIM2 Update interrupt so it doesn't occur while transmitting data
        TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);

        dmaBusy = false;
    }

}
