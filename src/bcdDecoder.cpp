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
#include "bcdDecoder.h"

BcdDecoder::BcdDecoder(uint8_t startBit, uint8_t bitWidth, bool withParity, uint8_t lowestValue, uint8_t highestValue) : _startBit(startBit),
																														 _bitWidth(bitWidth),
																														 _withParity(withParity),
																														 _lowestValue(lowestValue),
																														 _highestValue(highestValue)
{
}

//DCF77				UTC			Datetime
//0x623a4843141ae6 	1543022280	Sa, 24.11.18 02:18:00, WZ
bool BcdDecoder::getTime(uint8_t &value)
{
	value = _currentValue;
	return _currentValue != INVALID;
}

void BcdDecoder::print()
{
}

/**
 * @brief Check validity of the data by correlating these with a calculated prediction value for each bin.
 * The matching score of the correlation gets added to the corresponding bin.
 * @returns true when the object contains valid data.
 */
bool BcdDecoder::update(SecondsDecoder::BITDATA *data)
{
	if (data->validBitCtr < _startBit + _bitWidth + (_withParity ? 1 : 0) + 1)
	{
		//not enough valid samples in the data buffer
		//Serial.printf("%d\tnot enough samples\r\n", _startBit);
		return false;
	}
	uint64_t newData = data->bitShifter >> _startBit; //remove lower bits in bitshifter that don't belong to the BCD.
	uint64_t bitmask = (1 << (_bitWidth + (_withParity ? 1 : 0))) - 1;
	newData &= bitmask; //remove higher bits in the bitshifter that don't belong to the BCD.
	if (_withParity)
	{
		if (parityOdd(newData))
		{
			//Serial.printf("%d\twrong parity\r\n", _startBit);
			return false;
		}
		newData &= (bitmask >> 1); //remove parity bit
	}
	int tempVal = bcd2int(newData);
	if (tempVal < _lowestValue || tempVal > _highestValue)
	{
		//Serial.printf("%d\tvalue out of range\r\n", _startBit);
		return false;
	}
	_currentValue = tempVal;
	return true;
}

void BcdDecoder::clear()
{
	_currentValue = INVALID;
}

/**
 * Calculate parity over day, month, week, year
 */
bool BcdDecoder::dmyParityEven(SecondsDecoder::BITDATA *data)
{
	const int STARTBIT = 36;
	const int BITWIDTH = 23; //including parity bit
	const int PARITYBIT = 58;
	if (data->validBitCtr < PARITYBIT + 1)
	{
		//not enough valid samples in the data buffer
		//Serial.printf("dmy not enough samples\r\n");
		return false;
	}
	uint64_t newData = data->bitShifter >> STARTBIT; //remove lower bits in bitshifter that don't belong to the BCD.
	uint64_t bitmask = (1 << BITWIDTH) - 1;
	newData &= bitmask; //remove higher bits in the bitshifter that don't belong to the BCD.
	bool result = !parityOdd(newData);
	// if (!result)
	// {
	// 	Serial.println("dmy\twrong parity");
	// }
	return result;
}

uint8_t BcdDecoder::bcd2int(uint8_t bcd)
{
	//ret = highnibble * 10 + lownibble = highnibble*8 + highnibble*2 + lownibble
	uint8_t ret = bcd & 0xF;
	bcd &= 0xF0;
	return (bcd >> 1) + (bcd >> 3) + ret;
}

//https://stackoverflow.com/questions/21617970/how-to-check-if-value-has-even-parity-of-bits-or-odd
bool BcdDecoder::parityOdd(uint32_t x)
{
	x ^= x >> 16;
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return x & 1;
}
