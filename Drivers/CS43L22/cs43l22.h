/*
 * cs43l22.h
 *
 *  Created on: Dec 29, 2025
 *      Author: Volodymyr Kaliuzhnyi
 */

#ifndef CS43L22_H
#define CS43L22_H

#include "stm32f4xx_hal.h"

typedef enum
{
	C4,		   // 260.87 Hz
	C5,        // 521.74 Hz
	D5,        // 585.37 Hz
	E5,        // 666.67 Hz
	F5,        // 705.88 Hz
	G5,        // 774.19 Hz
	A5,        // 888.89 Hz
	B5,        // 1000.00 Hz
	C6,        // 1043.48 Hz
	D6,        // 1200.00 Hz
	E6,        // 1333.33 Hz
	F6,        // 1411.76 Hz
	G6,        // 1600.00 Hz
	A6,        // 1714.29 Hz
	B6,        // 2000.00 Hz
	C7,        // 2181.82 Hz
	MAX_VALUE
} soundToneType;

void CS43L22_Init(void);
void CS43L22_Beep(soundToneType pitch, uint32_t duration_ms);
void CS43L22_SetVolume(uint8_t volume);
void CS43L22_Play_HappyBirthday(void);
void CS43L22_Play_JingleBells(void);

#endif /* CS43L22_H */
