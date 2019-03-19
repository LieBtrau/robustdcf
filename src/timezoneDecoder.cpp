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
#include "timezoneDecoder.h"

TimeZoneDecoder::TimeZoneDecoder(){};

bool TimeZoneDecoder::update(SecondsDecoder::BITDATA *data)
{
    if (data->validBitCtr < SecondsDecoder::SECONDS_PER_MINUTE - STARTBIT)
    {
        //not enough valid samples in the data buffer
        return false;
    }
    if ((data->bitShifter & CEST_BIT) && (_isSummerTime < INT8_MAX))
    {
        _isSummerTime++;
    }
    if ((data->bitShifter & CET_BIT) && (_isSummerTime > INT8_MIN))
    {
        _isSummerTime--;
    }
    if (data->bitShifter & TIMEZONE_CHANGE_BIT)
    {
        if (_timeZoneChangeAnnounced < UINT8_MAX)
        {
            _timeZoneChangeAnnounced++;
        }
    }
    else
    {
        if (_timeZoneChangeAnnounced > 0)
        {
            _timeZoneChangeAnnounced--;
        }
    }
    return true;
}

bool TimeZoneDecoder::getSecondsOffset(int16_t &offset, uint8_t &hour, uint8_t minute)
{
    if ((!minute) && _timeZoneChangeAnnounced > 0)
    {
        switch (hour)
        {
        case 3:
            if ((_isSummerTime > 0))
            {
                //First minute of winter time
                hour--; //Correct hour reading: should have been read as 2, but due to binning will be read as 3.
                _isSummerTime = -1;
            }
            break;
        case 2:
            if ((_isSummerTime < 0))
            {
                //First minute of summer time
                hour++; //Correct hour reading: should have been read as 3, but due to binning will be read as 2.
                _isSummerTime = 1;
            }
            break;
        default:
            break;
        }
    }
    offset = _isSummerTime > 0 ? TWO_HOURS : ONE_HOUR;
    return true;
}
