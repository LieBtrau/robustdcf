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
#include "robustDcf.h"

static volatile bool secondTicked = false;
static bool syncMark;
static SECONDS_DATA clockPulseLength;

RobustDcf::RobustDcf(const byte inputPin, bool pulseHighPolarity) : _pd(inputPin, pulseHighPolarity),
                                                                   _minutes(21, 7, true, 0, 59, 4),
                                                                   _hours(29, 6, true, 0, 23, 3),
                                                                   _days(36, 6, false, 1, 31, 3),
                                                                   _months(45, 5, false, 1, 12, 2),
                                                                   _years(50, 8, false, 0, 99, 4)
{
}

//secondsTick is called by an ISR.  It should be kept as short as possible
static void secondsTick(const bool isSyncMark, const SECONDS_DATA pulseLength)
{
    syncMark = isSyncMark;
    clockPulseLength = pulseLength;
    secondTicked = true;
}

void RobustDcf::init()
{
    _pd.init(secondsTick);
}

//Becomes true once a minute (on second 59) to let you know that unixEpoch has been updated.
bool RobustDcf::update(Chronos::EpochTime &unixEpoch)
{
    if (!secondTicked)
    {
        return false;
    }

    secondTicked = false;
    _sd.updateSeconds(syncMark, clockPulseLength);
    uint8_t second;
    SecondsDecoder::BITDATA data;
    if ((!_sd.getSecond(second)) || (second != 59) || (!_sd.getTimeData(&data)))
    {
        return false;
    }
    return updateClock(&data, &unixEpoch);
}

bool RobustDcf::updateClock(SecondsDecoder::BITDATA *pdata, Chronos::EpochTime *pEpoch)
{
    bool bSuccess = true;
    bSuccess &= _minutes.update(pdata);
    bSuccess &= _hours.update(pdata);
    bSuccess &= _days.update(pdata);
    bSuccess &= _months.update(pdata);
    bSuccess &= _years.update(pdata);
    bSuccess &= _tzd.update(pdata);
    if (!bSuccess)
    {
        return false;
    }
    if (!getUnixEpochTime(pEpoch))
    {
        return false;
    }
    TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, +120}; //Last Sunday of March, at 2AM, go to UTC+120min
    TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, +60};   //Last Sunday of October, at 3AM, go to UTC+60min
    Timezone myTZ(myDST, mySTD);
    Chronos::DateTime localTime = myTZ.toLocal(*pEpoch);

    //set prediction for next minute
    Chronos::DateTime predictionTime = localTime + Chronos::Span::Minutes(1);
    _minutes.setPrediction(predictionTime.minute());
    _hours.setPrediction(predictionTime.hour());
    _days.setPrediction(predictionTime.day());
    _months.setPrediction(predictionTime.month());
    _years.setPrediction(tmYearToY2k(CalendarYrToTm(predictionTime.year())));
    return true;
}

//Currently only minute resolution.
bool RobustDcf::getUnixEpochTime(Chronos::EpochTime *pUnixEpoch)
{
    uint8_t minute, hour, day, month, year;
    int16_t secondsOffset;
    if (_minutes.getTime(minute) && _hours.getTime(hour) && _days.getTime(day) && _months.getTime(month) && _years.getTime(year))
    {
        _tzd.getSecondsOffset(secondsOffset, hour, minute);
        Chronos::DateTime localtime(tmYearToCalendar(y2kYearToTm(year)), month, day, hour, minute);
        *pUnixEpoch = localtime.asEpoch() - secondsOffset;
        return true;
    }
    return false;
}
