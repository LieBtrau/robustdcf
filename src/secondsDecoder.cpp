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
 * Copyright Christoph Tack, 2021
 * 
*/
/**
 * The seconds decoder tries to find the first second of the minute.
 * If this is found, then it provides time data of the previous minute and also the current second in the current minute.
 */
#include "secondsDecoder.h"

SecondsDecoder::SecondsDecoder() : _bin(SECONDS_PER_MINUTE) {}

/**
 * @brief Each second, pulse data comes in.  It gets shifted into the bit shifter.
 * After that, a correlator is run on the bit shifter.  The results are added to the current bin.
 * The bin that has the highest score is the most likely to be the minute start.
 * Following markers will be used:
 *  - 0-bit on second 0
 *  - timezone bits : bit 17 must be different from bit 18
 *  - 1-bit on second 20
 *  - even parity over bits 21-28
 *  - even parity over bits 29–35
 *  - even parity over date bits 36–58
 *  - sync mark on second 59
 * @param isSyncMark true when this second is the first second of the minute.
 * @param pulseLength pulse length of the current second.
 */
void SecondsDecoder::updateSeconds(const bool isSyncMark, const SECONDS_DATA pulseLength)
{
    Serial.printf("%d %d\r\n", isSyncMark, pulseLength);
    _curData.validBitCtr++;
     //Shift in new data from right to left (because LSb is sent first)
   _curData.bitShifter >>= 1;
    if (pulseLength == LONGPULSE)
    {
        _curData.bitShifter |= 0x800000000000000U;
    }
    if (isSyncMark || (pulseLength != UNKNOWNPULSE))
    {
        int8_t score = 0;
        //Detect 0-bit on second 0
        score += _curData.bitShifter & 1 ? -1 : 1;
        //Detect bit 17 and bit 18 are different;
        score += ((_curData.bitShifter ^ (_curData.bitShifter >> 1)) & 0x20000) ? 1 : -1;
        //Detect 1-bit on second 20
        score += _curData.bitShifter & 0x100000U ? 1 : -1;
        //Detect even parity over bits 21-28
        uint32_t parityCheck = _curData.bitShifter & 0x1FE00000;
        score += dataValid(parityCheck) ? 1 : -1;
        //Compiler bug : & bit operations on uint64 don't work.  Only the lower 32bits are taken into account.
        //Detect even parity over bits 29–35
        parityCheck = (_curData.bitShifter >> 4) & 0xFE000000;
        score += dataValid(parityCheck) & 1 ? 1 : -1;
        //Detect even parity over bits 36–58
        parityCheck = (_curData.bitShifter >> 28) & 0x7fffff00;
        score += dataValid(parityCheck) & 1 ? 1 : -1;
        //Detect sync mark on second 59
        score += (isSyncMark && (pulseLength == SHORTPULSE)) ? 6 : -6;
        _bin.add(_activeBin, score);
    }

    _minuteStartBin = _bin.maximum(LOCK_THRESHOLD);

    //Advance current bin
    _activeBin = _activeBin < (SECONDS_PER_MINUTE - 1) ? _activeBin + 1 : 0;

    uint8_t second = 0;
    if (getSecond(second) && second == 59)
    {
        _prevData = _curData;
        _curData = {0, 0};
    }
}

/**
 * @brief get the current second of the local time
 * @returns true when the clock was synced, else false and then the second parameter should be discarded.
 */
bool SecondsDecoder::getSecond(uint8_t &second)
{
    // we have to subtract 2 seconds
    //   1 because the seconds already advanced by 1 tick
    //   1 because the sync mark is not second 0 but second 59
    if (_minuteStartBin == INVALID)
    {
        second = 0;
        return false;
    }
    second = ((SECONDS_PER_MINUTE << 1) + _activeBin - 2 - _minuteStartBin);
    second %= SECONDS_PER_MINUTE;
    return true;
}

/**
 * @brief Get the value of all the seconds of the previous minute
 * @returns true when clock synced.  If false, then the data from the parameter should be discarded.
 */
bool SecondsDecoder::getTimeData(BITDATA *pdata)
{
    *pdata = _prevData;
    return _minuteStartBin != INVALID;
}

/**
 * @brief Check if data not zero and if parity is even
 */
bool SecondsDecoder::dataValid(uint64_t x)
{
    if (!x)
    {
        return false;
    }
    //https://stackoverflow.com/questions/9133279/bitparity-finding-odd-number-of-bits-in-an-integer#9133404
    x ^= x >> 32;
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x &= 0xf;
    return (0x6996 >> x) & 1 ? false : true;
}
