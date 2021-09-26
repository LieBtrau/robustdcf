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
	BcdDecoder(uint8_t startBit, uint8_t bitWidth, bool withParity, uint8_t lowestValue, uint8_t highestValue);
	bool update(SecondsDecoder::BITDATA *data);
	bool getTime(uint8_t &value);
	void clear();
	void print();
	static bool dmyParityEven(SecondsDecoder::BITDATA *data);
	const int8_t INVALID = -1;

private:
	uint8_t bcd2int(uint8_t bcd);
	static bool parityOdd(uint32_t x);
	uint8_t _startBit;
	uint8_t _bitWidth;
	bool _withParity;
	uint8_t _lowestValue;
	uint8_t _highestValue;
	int8_t _currentValue = INVALID;
};
