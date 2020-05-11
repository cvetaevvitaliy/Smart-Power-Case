/********************************
* Created by Vitaliy on 5/5/20.
********************************/
#include "HDQ.h"

//#warning REDEFINE GPIO PORT and PIN, It is not necessary to solder 10K resistor to Vcc - GPIO MODE PULL UP ( del this warning)
#define HDQ_PIN                     GPIO_PIN_9
#define HDQ_PORT_PIN                GPIOA

#define _HDQ_readPin()  HAL_GPIO_ReadPin(HDQ_PORT_PIN,HDQ_PIN) // Change me to inline!*/


void Config_GPIO_Output(void){
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = HDQ_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HDQ_PORT_PIN, &GPIO_InitStruct);

}

void Config_GPIO_Input(void){
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = HDQ_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(HDQ_PORT_PIN, &GPIO_InitStruct);

}

void HDQ_doBreak(void) {

    Config_GPIO_Output(); // Set pin as output

    // Singal a break on the line
    HAL_GPIO_WritePin(HDQ_PORT_PIN, HDQ_PIN,GPIO_PIN_RESET);   // Bring pin low
    DWT_Delay_us(HDQ_DELAY_TB);

    // Make sure we leave enough time for the slave to recover
    HAL_GPIO_DeInit(HDQ_PORT_PIN, HDQ_PIN);    // Release pin
    DWT_Delay_us(HDQ_DELAY_TBR);

}


void HDQ_writeByte(uint8_t payload) {
    Config_GPIO_Output(); // Set pin as output

    for (uint8_t i = 0; i < 8; i++) {

        // Start bit
        HAL_GPIO_WritePin(HDQ_PORT_PIN, HDQ_PIN, GPIO_PIN_RESET);   // Bring pin low
        DWT_Delay_us(HDQ_DELAY_THW1);

        // Toggle the pin for this bit
        if (payload >> i & 0x01) {    // LSB First
            HAL_GPIO_WritePin(HDQ_PORT_PIN, HDQ_PIN, GPIO_PIN_SET); // High
        } else {
            HAL_GPIO_WritePin(HDQ_PORT_PIN, HDQ_PIN, GPIO_PIN_RESET); // Low
        }

        // Bit time
        DWT_Delay_us(HDQ_DELAY_THW0 - HDQ_DELAY_THW1 + 5);

        // Stop bit
        HAL_GPIO_WritePin(HDQ_PORT_PIN, HDQ_PIN, GPIO_PIN_SET);  // Bring the pin high
        DWT_Delay_us(HDQ_DELAY_TCYCH - HDQ_DELAY_THW0);

    }

// Make sure we leave enough time for the slave to recover
    HAL_GPIO_DeInit(HDQ_PORT_PIN, HDQ_PIN);     // Release pin
//delayMicroseconds(HDQ_DELAY_TBR);
    DWT_Delay_us(HDQ_DELAY_TCYCH - HDQ_DELAY_THW0);

    return;
}


bool HDQ_write(uint8_t reg, uint8_t payload) {
// Singal a break
    HDQ_doBreak();

// Write the register to write
    HDQ_writeByte((reg |= HDQ_ADDR_MASK_WRITE));

// Wait for the slave to finish reading the register
    DWT_Delay_us((HDQ_DELAY_TRSPS_MAX - HDQ_DELAY_BIT_TOTAL) / 2);

// Write the payload
    HDQ_writeByte(payload);

// Wait for the slave to finish writing the payload
    DWT_Delay_us((HDQ_DELAY_TRSPS_MAX - HDQ_DELAY_BIT_TOTAL) / 2);

    HAL_GPIO_DeInit(HDQ_PORT_PIN, HDQ_PIN);     // Release pin

    return true;

}

uint8_t HDQ_read(uint8_t reg) {
    uint8_t result = 0;
    uint16_t maxTries = HDQ_DELAY_FAIL_TRIES; // ~128uS at 8Mhz with 8 instructions per loop

// Singal a break
    HDQ_doBreak();

// Write the register to read
    HDQ_writeByte((reg |= HDQ_ADDR_MASK_READ));

    Config_GPIO_Input();
    for (uint8_t ii = 0; ii < 8; ii++) {
        // Wait for the slave to toggle a low, or fail
        maxTries = HDQ_DELAY_FAIL_TRIES;
        while (_HDQ_readPin() != 0 && maxTries-- > 0)
            if (maxTries == 1) return 0xFF;

        // Wait until Tdsub and half or one bit has passed
        DWT_Delay_us(((HDQ_DELAY_TDW0 - HDQ_DELAY_TDW1) / 2) + HDQ_DELAY_TDW1);
        // Read the bit
        result |= _HDQ_readPin()<<ii;

        // Wait until Tssub has passed
        DWT_Delay_us(HDQ_DELAY_TCYCD - HDQ_DELAY_TDW0);
    }

    DWT_Delay_us(HDQ_DELAY_TB);

    return result;

}



bool HDQ_write_Verif(uint8_t reg, uint8_t payload, bool verif) { // Write the payload
    HDQ_write(reg, payload);

// Verify the write
    if (payload == HDQ_read(reg)) return true;

    return false;

}


