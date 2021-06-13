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
/* This class tries to find the start of each second in data stream of 0's & 1's coming from the DCF-module.
 * Once the phase is found, it also returns the data from the stream : long/short pulse or no pulse at all (minute sync mark)
 */
#pragma once
#include "Arduino.h"
#include "bin.h"
#include "secondsDecoder.h"

typedef enum
{
	LOWV = -1,
	DONTKNOW = 0,
	HIGHV = 1
} FUZZY;



typedef void (*event)(const bool isSync, const SECONDS_DATA pulseLength);

class PhaseDetector
{
public:
	PhaseDetector(const byte inputPin, bool pulseHighPolarity);
	void init(event secondTickEvent);
	void process_one_sample();

private:
	static const int BIN_COUNT = 100;
	static const uint8_t INVALID = 255;
	static const uint16_t SAMPLE_FREQ = 1000;
	static const uint16_t SAMPLES_PER_BIN = SAMPLE_FREQ / BIN_COUNT;
	static const uint16_t BINS_PER_10ms = BIN_COUNT / 100;
	static const uint16_t BINS_PER_100ms = 10 * BINS_PER_10ms;
	static const uint16_t BINS_PER_200ms = 20 * BINS_PER_10ms;
	const uint32_t LOCK_THRESHOLD = 75;

	uint8_t wrap(const uint8_t value);
	bool phaseCorrelator();
	void phase_binning(const FUZZY input);
	void averager(const uint8_t sampled_data);
	void secondsSampler(const FUZZY averagedInput);

	byte _inputPin = 0;
	event _secondsEvent = nullptr;
	Bin _bin; //100bins, each holding for 10ms of data
	bool _pulseActiveHigh;
	uint32_t _phaseCorrelation[BIN_COUNT];
	uint8_t _activeBin = 0;
	uint8_t _pulseStartBin = INVALID;
};
