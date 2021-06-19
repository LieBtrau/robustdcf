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
/*  The BcdDecoder gets some BCD-encoded data bytes of the SecondsDecoder, checks validity and converts these to decimal values.
 */
#pragma once
#include "Arduino.h"
#include "bin.h"
#include "secondsDecoder.h"
class BcdDecoder
{
public:
	BcdDecoder(uint8_t startBit, uint8_t bitWidth, bool withParity, uint8_t lowestValue, uint8_t highestValue, int8_t lockThreshold);
	bool update(SecondsDecoder::BITDATA *data);
	void setPrediction(uint8_t prediction);
	bool getTime(uint8_t &value);
	void clear();

private:
	uint8_t bcd2int(uint8_t bcd);
	uint8_t int2bcd(uint8_t hex);
	bool parityOdd(uint8_t x);
	int hammingWeight(int i);
	uint8_t getValueInRange(uint8_t i);
	uint8_t _startBit;
	uint8_t _bitWidth;
	bool _withParity;
	uint8_t _lowestValue;
	uint8_t _highestValue;
	int8_t _lockThreshold = 0;
	uint8_t _currentTick = 0;
	Bin _bin;
};
