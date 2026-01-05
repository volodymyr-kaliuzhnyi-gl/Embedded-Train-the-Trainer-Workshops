/*
 * cs43l22.c
 *
 *  Created on: Dec 29, 2025
 *      Author: Volodymyr Kaliuzhnyi
 */

#include "cs43l22.h"
#include "main.h"

#define CS43L22_I2C_ADDRESS     0x94
#define I2C_TIMEOUT             10

extern I2C_HandleTypeDef hi2c1;

void CS43L22_Init(void)
{
	// Enable chip
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);

	//
	// Initialization
	//
	uint8_t TxBuffer[2];

	TxBuffer[0] = 0x0D;
	TxBuffer[1] = 0x01;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x00;
	TxBuffer[1] = 0x99;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x47;
	TxBuffer[1] = 0x80;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x32;
	TxBuffer[1] = 0xFF;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x32;
	TxBuffer[1] = 0x7F;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x00;
	TxBuffer[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x04;
	TxBuffer[1] = 0xAF;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x0D;
	TxBuffer[1] = 0x70;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x05;
	TxBuffer[1] = 0x81;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x06;
	TxBuffer[1] = 0x07;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x0A;
	TxBuffer[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x27;
	TxBuffer[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x1A;
	TxBuffer[1] = 0x0A;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x1B;
	TxBuffer[1] = 0x0A;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x1F;
	TxBuffer[1] = 0x0F;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x02;
	TxBuffer[1] = 0x9E;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);
}

void CS43L22_Beep(soundToneType pitch, uint32_t duration_ms)
{
	uint8_t TxBuffer[2];

	// --- 1. LED Visualization Logic (Start) ---
	// Reset all LEDs first to ensure a clean state
	HAL_GPIO_WritePin(LED_GPIO_Port, Green_LED_Pin|Orange_LED_Pin|Red_LED_Pin|Blue_LED_Pin, GPIO_PIN_RESET);

	// Turn on specific LED based on pitch range
	if (pitch <= E5) {
		HAL_GPIO_WritePin(LED_GPIO_Port, Green_LED_Pin, GPIO_PIN_SET);   // Low Notes
	}
	else if (pitch <= B5) {
		HAL_GPIO_WritePin(LED_GPIO_Port, Orange_LED_Pin, GPIO_PIN_SET);  // Mid-Low Notes
	}
	else if (pitch <= F6) {
		HAL_GPIO_WritePin(LED_GPIO_Port, Red_LED_Pin, GPIO_PIN_SET);     // Mid-High Notes
	}
	else {
		HAL_GPIO_WritePin(LED_GPIO_Port, Blue_LED_Pin, GPIO_PIN_SET);    // High Notes
	}

	// Set volume and off time
	TxBuffer[0] = 0x1D;		// Register address
	TxBuffer[1] = 0x00;		// Value (volume and off time)
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	// Set sound frequency
	TxBuffer[0] = 0x1C;		// Register address
	switch (pitch)
	{
	case C4:
		TxBuffer[1] = 0x00;		// 261 Hz

		break;
	case C5:
		TxBuffer[1] = 0x10;		// 522 Hz

		break;
	case D5:
		TxBuffer[1] = 0x20;		// 585 Hz

		break;

	case E5:
		TxBuffer[1] = 0x30;		// 667 Hz

		break;
	case F5:
		TxBuffer[1] = 0x40;		// 706 Hz

		break;
	case G5:
		TxBuffer[1] = 0x50;		// 774 Hz

		break;
	case A5:
		TxBuffer[1] = 0x60;		// 889 Hz

		break;
	case B5:
		TxBuffer[1] = 0x70;		// 1000 Hz

		break;
	case C6:
		TxBuffer[1] = 0x80;		// 1043 Hz

		break;
	case D6:
		TxBuffer[1] = 0x90;		// 1200 Hz

		break;
	case E6:
		TxBuffer[1] = 0xA0;		// 1333 Hz

		break;
	case F6:
		TxBuffer[1] = 0xB0;		// 1412 Hz

		break;
	case G6:
		TxBuffer[1] = 0xC0;		// 1600 Hz

		break;
	case A6:
		TxBuffer[1] = 0xD0;		// 1714 Hz

		break;
	case B6:
		TxBuffer[1] = 0xE0;		// 2000 Hz

		break;
	case C7:
		TxBuffer[1] = 0xF0;		// 2182 Hz

		break;

	case MAX_VALUE:
		default:
		TxBuffer[1] = 0x00;		// Default to C4

		break;
	}

	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	// Enable continuous mode (SOUND STARTED)
	TxBuffer[0] = 0x1E;		// Register address
	TxBuffer[1] = 0xC0;		// Value (beep and tone configuration)
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	// Playing...
	HAL_Delay(duration_ms);

	// Disable continuous mode (SOUND STOPED)
	TxBuffer[0] = 0x1E;		// Register address
	TxBuffer[1] = 0x00;		// Value (beep and tone configuration)
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	// --- 2. LED Visualization Logic (Stop) ---
	// Turn off all LEDs when sound stops
	HAL_GPIO_WritePin(LED_GPIO_Port, Green_LED_Pin|Orange_LED_Pin|Red_LED_Pin|Blue_LED_Pin, GPIO_PIN_RESET);
}

/**
 * @brief  Sets the analog Headphone Volume (Master Fader).
 * @param  volume: 0 (Mute) to 100 (Max 0dB)
 */
void CS43L22_SetVolume(uint8_t volume)
{
    uint8_t convertedVol;

    if (volume > 100) volume = 100;

    if (volume == 0)
    {
        convertedVol = 0x01; /* MUTE */
    }
    else if (volume == 100)
    {
        convertedVol = 0x00; /* 0 dB (Max) */
    }
    else
    {
        /* Map 1-99% to the range roughly -96dB (0x34) to -0.5dB (0xFF)
           255 - (99 * 2) = 57 (0x39), which is close to the minimum    */

        convertedVol = 255 - ((100 - volume) * 2);

        if (convertedVol < 0x34)
        {
        	convertedVol = 0x34;
        }
    }

    /* Write to Headphone A (0x22) and B (0x23) */
    uint8_t TxBuffer[2];

    TxBuffer[0] = 0x22;       /* Head phone Volume A */
    TxBuffer[1] = convertedVol;
    HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*)&TxBuffer, 2, I2C_TIMEOUT);

    TxBuffer[0] = 0x23;       /* Head phone Volume B */
    TxBuffer[1] = convertedVol;
    HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*)&TxBuffer, 2, I2C_TIMEOUT);
}

void CS43L22_Play_HappyBirthday(void)
{
    const float TEMPO = 0.9f;
    const uint32_t GAP = 50; /* Silence between notes in ms */

    // Note Durations (calculated relative to TEMPO)
    const uint32_t N_Q  = (uint32_t)(400 * TEMPO); // Quarter Note (Beat)
    const uint32_t N_H  = (uint32_t)(800 * TEMPO); // Half Note
    const uint32_t N_E  = (uint32_t)(200 * TEMPO); // Eighth Note (Quick)

    // Part 1: "Happy Birthday to You"
    // Notes: G5. G5 A5 G5 C6 B5
    CS43L22_Beep(G5, N_E); HAL_Delay(GAP);
    CS43L22_Beep(G5, N_E); HAL_Delay(GAP);
    CS43L22_Beep(A5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(G5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(C6, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(B5, N_H); HAL_Delay(GAP);

    // Part 2: "Happy Birthday to You"
    // Notes: G5. G5 A5 G5 D6 C6
    CS43L22_Beep(G5, N_E); HAL_Delay(GAP);
    CS43L22_Beep(G5, N_E); HAL_Delay(GAP);
    CS43L22_Beep(A5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(G5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(D6, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(C6, N_H); HAL_Delay(GAP);

    // Part 3: "Happy Birthday Dear Denys"
    // Notes: G5. G5 G6 E6 C6 B5 A5
    CS43L22_Beep(G5, N_E); HAL_Delay(GAP);
    CS43L22_Beep(G5, N_E); HAL_Delay(GAP);
    CS43L22_Beep(G6, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E6, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(C6, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(B5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(A5, N_H); HAL_Delay(GAP);

    // Part 4: "Happy Birthday to You"
    // Notes: F6. F6 E6 C6 D6 C6
    CS43L22_Beep(F6, N_E); HAL_Delay(GAP);
    CS43L22_Beep(F6, N_E); HAL_Delay(GAP);
    CS43L22_Beep(E6, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(C6, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(D6, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(C6, N_H); HAL_Delay(GAP);
}

void CS43L22_Play_JingleBells(void)
{
    // Adjust tempo: 1.0 = Normal, 0.8 = Faster
    const float TEMPO = 0.9f;
    const uint32_t GAP = 50;        // Silence between notes

    // Note Durations
    const uint32_t N_Q = (uint32_t)(400 * TEMPO);   // Quarter Note (Beat)
    const uint32_t N_H = (uint32_t)(800 * TEMPO);   // Half Note
    const uint32_t N_W = (uint32_t)(1600 * TEMPO);  // Whole Note

    // Part 1: "Jingle bells, jingle bells"
    // Notes: E5 E5 E5 | E5 E5 E5
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_H); HAL_Delay(GAP);

    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_H); HAL_Delay(GAP);

    // Part 2: "Jingle all the way"
    // Notes: E5 G5 C5 D5 E5
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(G5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(C5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(D5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_W); HAL_Delay(GAP);

    // Part 3: "Oh what fun it is to ride"
    // Notes: F5 F5 F5 F5 F5 E5 E5 E5
    CS43L22_Beep(F5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(F5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(F5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(F5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(F5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);

    // Part 4: "In a one-horse open sleigh"
    // Notes: E5 D5 D5 E5 D5 G5
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(D5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(D5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(E5, N_Q); HAL_Delay(GAP);
    CS43L22_Beep(D5, N_H); HAL_Delay(GAP);
    CS43L22_Beep(G5, N_H); HAL_Delay(GAP);
}


