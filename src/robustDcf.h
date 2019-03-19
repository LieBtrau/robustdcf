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
#include "phaseDetector.h"
#include "secondsDecoder.h"
#include "bcdDecoder.h"
#include "timezoneDecoder.h"
#include <Timezone.h>
#include <Chronos.h>

class RobustDcf
{
public:
  RobustDcf(const byte inputPin);
  void init();
  bool update(Chronos::EpochTime &unixEpoch);
  bool updateClock(SecondsDecoder::BITDATA *data, Chronos::EpochTime *pEpoch);

private:
  bool getUnixEpochTime(Chronos::EpochTime* unixEpoch);
  PhaseDetector _pd;
  SecondsDecoder _sd;
  BcdDecoder _minutes, _hours, _days, _months, _years;
  TimeZoneDecoder _tzd;
};
