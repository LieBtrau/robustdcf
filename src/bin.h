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
#pragma once
#include "Arduino.h"
static const uint8_t INVALID = 0xFF;

class Bin
{
public:
    Bin(uint8_t dataSize, int8_t initVal = 0);
    ~Bin();
    void add(uint8_t index, int8_t N);
    void clear();
    uint8_t maximum(int8_t threshold);
    uint8_t count();
    uint8_t getUnsigned(uint8_t index);
   
private:
    void bounded_increment(uint8_t index, int8_t N);
    int8_t *_pData = nullptr;
    uint8_t _dataSize = 0;
    int8_t _initVal = 0;
};
