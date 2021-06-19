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
#include "secondsDecoder.h"
#include "bin.h"

class TimeZoneDecoder
{
public:
	TimeZoneDecoder();
	bool update(SecondsDecoder::BITDATA *data);
	bool getSecondsOffset(int16_t &offset, uint8_t &hour, uint8_t minute);
	void clear();

private:
	static const uint8_t NR_OR_TIMEZONES = 2;
	static const uint8_t STARTBIT = 16;
	static const int8_t THRESHOLD = 1;
	static const uint32_t TIMEZONE_CHANGE_BIT = 0x10000;
	static const uint32_t CEST_BIT = 0x20000;
	static const uint32_t CET_BIT = 0x40000;
	static const int16_t ONE_HOUR = 3600;
	static const int16_t TWO_HOURS = 7200;
	uint8_t _timeZoneChangeAnnounced = 0;
	int8_t _isSummerTime = 0;
	bool _isPredictionCEST = false;
};
