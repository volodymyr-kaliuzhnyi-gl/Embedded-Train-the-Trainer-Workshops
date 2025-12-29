/*
 * pca9685.h
 *
 *  Created on: Dec 26, 2025
 *      Author: Admin
 */

#ifndef PCA9685_PCA9685_H_
#define PCA9685_PCA9685_H_

#include "stm32f4xx_hal.h"

// --- I2C Address Base ---
// 0x40 (binary 1000000) is the base address.
// The final address depends on the offset (A0-A5 pins).
#define PCA9685_I2C_BASE_ADDRESS (0x40)

// --- Register Addresses
#define PCA9685_MODE1           0x00
#define PCA9685_MODE2           0x01
#define PCA9685_PRE_SCALE       0xFE
#define PCA9685_LED0_ON_L       0x06
#define PCA9685_ALL_LED_ON_L    0xFA

// --- MODE1 Configuration Bits
#define MODE1_RESTART           0x80
#define MODE1_EXTCLK            0x40
#define MODE1_AI                0x20 // Auto-Increment
#define MODE1_SLEEP             0x10
#define MODE1_SUB1              0x08
#define MODE1_SUB2              0x04
#define MODE1_SUB3              0x02
#define MODE1_ALLCALL           0x01

// --- MODE2 Configuration Bits
#define MODE2_INVRT             0x10 // Invert output logic
#define MODE2_OCH               0x08 // Outputs change on ACK
#define MODE2_OUTDRV            0x04 // Totem pole structure

// Internal oscillator frequency
#define PCA9685_OSC_FREQ        25000000UL

// --- Driver Handle Structure ---
typedef struct {
    I2C_HandleTypeDef *hi2c; // Pointer to STM32 I2C handle
    uint8_t address;         // Calculated I2C address (8-bit format for HAL)
    uint8_t invrt;           // 1 = Inverted logic (for LEDs on cathode or P-FETs), 0 = Normal
} PCA9685_HandleTypeDef;

// --- Function Prototypes ---

/**
 * @brief  Initializes the PCA9685 with specific address offset.
 * @param  dev: Pointer to the driver handle.
 * @param  hi2c: Pointer to the HAL I2C handle.
 * @param  address_offset: Value determined by hardware pins A0-A5 (0 to 63).
 * @retval HAL Status
 */
HAL_StatusTypeDef PCA9685_Init(PCA9685_HandleTypeDef *dev, I2C_HandleTypeDef *hi2c, uint8_t address_offset);

/**
 * @brief  Sets the PWM frequency for all channels.
 * @param  dev: Pointer to the driver handle.
 * @param  frequency: Frequency in Hz (24 - 1526).
 * @retval HAL Status
 */
HAL_StatusTypeDef PCA9685_SetPWMFreq(PCA9685_HandleTypeDef *dev, uint16_t frequency);

/**
 * @brief  Sets raw PWM ON and OFF times for a specific channel.
 * @param  dev: Pointer to the driver handle.
 * @param  channel: Channel number (0-15).
 * @param  on_time: LED ON count (0-4095).
 * @param  off_time: LED OFF count (0-4095).
 * @retval HAL Status
 */
HAL_StatusTypeDef PCA9685_SetPWM(PCA9685_HandleTypeDef *dev, uint8_t channel, uint16_t on_time, uint16_t off_time);

/**
 * @brief  Sets the duty cycle in percent (0% to 100%).
 * Automatically applies phase shifting (staggering) to reduce current surges.
 * @param  dev: Pointer to the driver handle.
 * @param  channel: Channel number (0-15).
 * @param  duty_cycle_percent: Duty cycle (0 to 100).
 * @retval HAL Status
 */
HAL_StatusTypeDef PCA9685_SetDutyCycle(PCA9685_HandleTypeDef *dev, uint8_t channel, uint8_t duty_cycle_percent);

/**
 * @brief  Sets duty cycles for ALL 16 channels at once using an array.
 * Sends all data in a single I2C burst write (Optimization).
 * @param  dev: Pointer to the driver handle.
 * @param  duty_cycles: Pointer to an array of 16 values (uint8_t each, 0-100%).
 */
HAL_StatusTypeDef PCA9685_SetAllChannelsDuty(PCA9685_HandleTypeDef *dev, uint8_t *duty_cycles);

#endif /* PCA9685_PCA9685_H_ */
