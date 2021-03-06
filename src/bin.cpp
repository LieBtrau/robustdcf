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
#include "bin.h"

Bin::Bin(uint8_t dataSize, int8_t initVal) : _dataSize(dataSize)
{
    _pData = (int8_t *)malloc(_dataSize);
    if (_pData)
    {
        memset(_pData, initVal, _dataSize);
    }
}

Bin::~Bin()
{
    if(_pData)
    {
        free(_pData);
    }
}

void Bin::add(uint8_t index, int8_t N)
{
    if (_pData[index] == INT8_MAX)
    {
        //if bin is already at maximum, decrease the other bins
        for (uint8_t j = 0; j < _dataSize; j++)
        {
            if (j != index)
            {
                bounded_increment(j, -N);
            }
        }
    }
    else
    {
        bounded_increment(index, N);
    }
}

uint8_t Bin::size()
{
    return _dataSize;
}

uint8_t Bin::getUnsigned(uint8_t index)
{
    return _pData[index]+128;
}

uint8_t Bin::maximum(int8_t threshold)
{
    int8_t maximum = INT8_MIN;
    uint8_t maxBin = 0xFF;
    //Find bin with the highest score.
    for (uint8_t i = 0; i < _dataSize; i++)
    {
        if (_pData[i] >= max(maximum, threshold))
        {
            maximum = _pData[i];
            maxBin = i;
        }
        // Serial1.print(_pData[i]);
        // Serial1.print(" ");
    }
    return maxBin;
}

void Bin::bounded_increment(uint8_t index, int8_t N)
{
    int8_t *pval = &_pData[index];
    if (N > 0)
    {
        *pval = *pval > INT8_MAX - N ? INT8_MAX : *pval + N;
    }
    else
    {
        *pval = *pval < INT8_MIN - N ? INT8_MIN : *pval + N;
    }
}
