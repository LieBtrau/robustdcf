/*
 * This file is part of RobustDcf.
 * 
 * RobustDcf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 * 
 * Copyright Christoph Tack, 2018
*/
/* The secondsDecoder finds the start of a minute inside the pulse data (long/short/none) that it gets from the phase decoder.
 * It also holds the databits for the next minute and transfers these on the start of each minute to other objects.
 */
#pragma once
#include "Arduino.h"
#include "bin.h"

typedef enum
{
	SHORTPULSE,
	LONGPULSE,
	UNKNOWNPULSE
} SECONDS_DATA;

class SecondsDecoder
{
public:
	typedef struct
	{
		uint64_t bitShifter;
		uint8_t validBitCtr;
	} BITDATA;
	static const uint8_t SECONDS_PER_MINUTE = 60;
	SecondsDecoder();
	void updateSeconds(const bool isSyncMark, const SECONDS_DATA pulseLength);
	bool getSecond(uint8_t &second);
	bool getTimeData(BITDATA *pdata);

private:
	static const int8_t LOCK_THRESHOLD = 7;
	bool dataValid(uint64_t x);
	Bin _bin;
	uint8_t _activeBin = 0;
	BITDATA _curData = {0, 0};
	BITDATA _prevData = {0, 0};
	uint8_t _minuteStartBin = INVALID;
};
