/*
 * pca9685.c
 *
 *  Created on: Dec 26, 2025
 *      Author: Volodymyr Kaliuzhnyi
 */

#include "pca9685.h"

// --- Helper Functions ---

static HAL_StatusTypeDef PCA9685_WriteReg(PCA9685_HandleTypeDef *dev, uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write(dev->hi2c, dev->address, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 10);
}

static uint8_t PCA9685_ReadReg(PCA9685_HandleTypeDef *dev, uint8_t reg) {
    uint8_t data = 0;

    HAL_I2C_Mem_Read(dev->hi2c, dev->address, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 10);
    return data;
}

// --- Main Functions ---

HAL_StatusTypeDef PCA9685_Init(PCA9685_HandleTypeDef *dev, I2C_HandleTypeDef *hi2c, uint8_t address_offset) {
    dev->hi2c = hi2c;

    // Calculate 8-bit address based on Offset (A0-A5)
    // 0x40 is base. Offset is limited to 6 bits (0x3F). Shift left by 1 for HAL.
    dev->address = (PCA9685_I2C_BASE_ADDRESS | (address_offset & 0x3F)) << 1;

    // 1. Reset: Write 0x00 to MODE1
    HAL_StatusTypeDef status = PCA9685_WriteReg(dev, PCA9685_MODE1, 0x00);
    if (status != HAL_OK) return status;

    // 2. Setup MODE1: Enable Auto-Increment
    // Do NOT set SLEEP here, leave it as default (0) or logic will handle it in SetFreq
    uint8_t mode1 = MODE1_AI;
    status = PCA9685_WriteReg(dev, PCA9685_MODE1, mode1);

    // 3. Setup MODE2: Totem Pole (default) + Optional Inversion
    uint8_t mode2 = MODE2_OUTDRV; // Default to Totem Pole

    if (dev->invrt) {
        mode2 |= MODE2_INVRT; // Apply inversion if requested
    }

    status = PCA9685_WriteReg(dev, PCA9685_MODE2, mode2);

    return status;
}

HAL_StatusTypeDef PCA9685_SetPWMFreq(PCA9685_HandleTypeDef *dev, uint16_t frequency) {
    if (frequency < 24) frequency = 24;
    if (frequency > 1526) frequency = 1526;

    // Casting to float strictly for calculation precision
    // Formula: prescale = round(osc_clock / (4096 * update_rate)) - 1
    float prescaleval = ((float)PCA9685_OSC_FREQ / (4096.0f * (float)frequency)) - 1.0f;
    uint8_t prescale = (uint8_t)(prescaleval + 0.5f); // Simple rounding

    uint8_t oldmode = PCA9685_ReadReg(dev, PCA9685_MODE1);
    uint8_t newmode = (oldmode & 0x7F) | MODE1_SLEEP; // Set SLEEP bit

    HAL_StatusTypeDef status = HAL_OK;
    status = PCA9685_WriteReg(dev, PCA9685_MODE1, newmode); // Go to sleep
    if (status != HAL_OK) return status;

    status = PCA9685_WriteReg(dev, PCA9685_PRE_SCALE, prescale); // Set prescaler
    if (status != HAL_OK) return status;

    status = PCA9685_WriteReg(dev, PCA9685_MODE1, oldmode); // Wake up
    HAL_Delay(5); // Wait for oscillator stabilization (>500us)

    // Restore Auto-Increment and Restart
    status = PCA9685_WriteReg(dev, PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);

    return status;
}

HAL_StatusTypeDef PCA9685_SetPWM(PCA9685_HandleTypeDef *dev, uint8_t channel, uint16_t on_time, uint16_t off_time) {
    if (channel > 15) return HAL_ERROR;

    HAL_StatusTypeDef status = HAL_OK;

    // Calculate register address: LED0_ON_L (0x06) + 4 * channel
    uint8_t reg_addr = PCA9685_LED0_ON_L + (4 * channel);

    uint8_t data[4];
    data[0] = on_time & 0xFF;         // ON LSB
    data[1] = (on_time >> 8);         // ON MSB
    data[2] = off_time & 0xFF;        // OFF LSB
    data[3] = (off_time >> 8);        // OFF MSB

    // Write 4 bytes consecutively (Auto-Increment)
    status = HAL_I2C_Mem_Write(dev->hi2c, dev->address, reg_addr, I2C_MEMADD_SIZE_8BIT, data, 4, 10);

    return status;
}

HAL_StatusTypeDef PCA9685_SetDutyCycle(PCA9685_HandleTypeDef *dev, uint8_t channel, uint8_t duty_cycle_percent) {
    if (duty_cycle_percent > 100) duty_cycle_percent = 100;

    uint16_t on_time = 0;
    uint16_t off_time = 0;

    HAL_StatusTypeDef status = HAL_OK;

    // Handle Full OFF / Full ON cases (Bit 4 logic)
    if (duty_cycle_percent == 0) {
        on_time = 0;
        off_time = 4096; // Sets bit 4 of LEDn_OFF_H
    }
    else if (duty_cycle_percent == 100) {
        on_time = 4096;  // Sets bit 4 of LEDn_ON_H
        off_time = 0;
    }
    else {
        // --- Phase Shifting (Staggering) Implementation ---
        // Total ticks = 4096. Channels = 16.
        // Shift per channel = 4096 / 16 = 256.
        uint16_t phase_shift = 256;

        // Stagger ON time based on channel index
        on_time = channel * phase_shift;

        // Integer math calculation:
        // (4096 * percent) / 100
        // Use uint32_t to prevent overflow before division (4096 * 100 = 409600, fits in 32-bit)
        // Calculate duration in ticks
        uint32_t duration_calc = (4096UL * duty_cycle_percent) / 100UL;
        uint16_t duration = (uint16_t)duration_calc;

        // Calculate OFF time
        off_time = on_time + duration;

        // Handle wrap-around if OFF time exceeds 4095
        if (off_time >= 4096) {
            off_time -= 4096;
        }
    }

    status = PCA9685_SetPWM(dev, channel, on_time, off_time);

    return status;
}

HAL_StatusTypeDef PCA9685_SetAllChannelsDuty(PCA9685_HandleTypeDef *dev, uint8_t *duty_cycles) {
    // Buffer for all LED registers: 16 channels * 4 bytes (ON_L, ON_H, OFF_L, OFF_H)
    uint8_t buffer[64];
    uint8_t *pBuf = buffer; // Pointer to traverse the buffer
    HAL_StatusTypeDef status = HAL_OK;

    for (uint8_t i = 0; i < 16; i++) {
        uint8_t duty = duty_cycles[i];
        if (duty > 100) duty = 100;

        uint16_t on_time = 0;
        uint16_t off_time = 0;

        if (duty == 0) {
            on_time = 0;
            off_time = 4096; // Full OFF logic
        }
        else if (duty == 100) {
            on_time = 4096;  // Full ON logic
            off_time = 0;
        }
        else {
            // --- Phase Shifting Logic ---
            uint16_t phase_shift = 256;
            on_time = i * phase_shift;

            uint32_t duration_calc = (4096UL * duty) / 100UL;
            uint16_t duration = (uint16_t)duration_calc;

            off_time = on_time + duration;

            if (off_time >= 4096) {
                off_time -= 4096;
            }
        }

        // Populate buffer (Little Endian: LSB first)
        *pBuf++ = on_time & 0xFF;        // LEDn_ON_L
        *pBuf++ = (on_time >> 8);        // LEDn_ON_H
        *pBuf++ = off_time & 0xFF;       // LEDn_OFF_L
        *pBuf++ = (off_time >> 8);       // LEDn_OFF_H
    }

    // Burst Write starting from LED0_ON_L (0x06)
    // We write 64 bytes sequentially. The PCA9685 Auto-Increment feature handles the register addresses.
    // Timeout increased to 20ms because the packet is long.
    status = HAL_I2C_Mem_Write(dev->hi2c, dev->address, PCA9685_LED0_ON_L, I2C_MEMADD_SIZE_8BIT, buffer, 64, 20);

    return status;
}


