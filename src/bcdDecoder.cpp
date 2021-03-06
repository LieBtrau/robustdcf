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

BcdDecoder::BcdDecoder(uint8_t startBit, uint8_t bitWidth, bool withParity, uint8_t lowestValue, uint8_t highestValue, int8_t lockThreshold) : _startBit(startBit), _bitWidth(bitWidth), _withParity(withParity), _lowestValue(lowestValue), _highestValue(highestValue), _lockThreshold(lockThreshold), _bin(highestValue - lowestValue + 1)
{
}

bool BcdDecoder::getTime(uint8_t &value)
{
    uint8_t bin = _bin.maximum(_lockThreshold);
    if (bin == 0xFF)
    {
        return false;
    }
    //Convert index of the bin to a decimal value within the expected range.
    value = getValueInRange(bin);
    return true;
}

void BcdDecoder::setPrediction(uint8_t prediction)
{
    uint8_t bin = _bin.maximum(_lockThreshold);
    if (bin == 0xFF)
    {
        return;
    }
    _currentTick = (_bin.size() + prediction - _lowestValue - bin) % _bin.size();
}

/* Check validity of the data by correlating these with a calculated prediction value for each bin.
 * The matching score of the correlation gets added to the corresponding bin.
 */
bool BcdDecoder::update(SecondsDecoder::BITDATA *data)
{
    if (data->validBitCtr < SecondsDecoder::SECONDS_PER_MINUTE - _startBit)
    {
        //not enough valid samples in the data buffer
        return false;
    }
    uint64_t newData = data->bitShifter >> _startBit;
    newData &= (1 << (_bitWidth + (_withParity ? 1 : 0))) - 1;
    for (uint8_t i = 0; i < _bin.size(); i++)
    {
        uint8_t prediction = int2bcd(getValueInRange(i));
        if (_withParity && parityOdd(prediction))
        {
            prediction |= 1 << _bitWidth;
        }
        int8_t score = ((_bitWidth + (_withParity ? 1 : 0)) >> 1) - hammingWeight(newData ^ prediction);
        _bin.add(i, score);
    }
    return true;
}

uint8_t BcdDecoder::bcd2int(uint8_t bcd)
{
    //ret = highnibble * 10 + lownibble = highnibble*8 + highnibble*2 + lownibble
    uint8_t ret = bcd & 0xF;
    bcd &= 0xF0;
    return (bcd >> 1) + (bcd >> 3) + ret;
}

//https://www.electronicdesign.com/displays/easily-convert-decimal-numbers-their-binary-and-bcd-formats
uint8_t BcdDecoder::int2bcd(uint8_t hex)
{
    uint8_t highNibble = hex / 10;
    return (highNibble << 2) + (highNibble << 1) + hex;
}

bool BcdDecoder::parityOdd(uint8_t x)
{
    x ^= x >> 4;
    x &= 0xf;
    return (0x6996 >> x) & 1;
}

//https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer#109025
int BcdDecoder::hammingWeight(int i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

// binOffset = 0 to _datasize - 1;
uint8_t BcdDecoder::getValueInRange(uint8_t binOffset)
{
    return _lowestValue + ((binOffset + _currentTick) % _bin.size());
}
