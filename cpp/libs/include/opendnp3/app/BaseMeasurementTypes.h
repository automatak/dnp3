/*
 * Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
 * more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 * Green Energy Corp licenses this file to you under the Apache License,
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project was forked on 01/01/2013 by Automatak, LLC and modifications
 * may have been made to this file. Automatak, LLC licenses these modifications
 * to you under the terms of the License.
 */
#ifndef OPENDNP3_BASEMEASUREMENTTYPES_H
#define OPENDNP3_BASEMEASUREMENTTYPES_H

#include <cstdint>

#include "opendnp3/Types.h"

namespace opendnp3
{

/**
  Base class shared by all of the DataPoint types.
*/
class Measurement
{
public:

	virtual ~Measurement() {}

	uint8_t quality;	//	bitfield that stores type specific quality information
	DNPTime time;		//	timestamp associated with the measurement

protected:

	Measurement() : quality(0)
	{}

	Measurement(uint8_t quality_) : quality(quality_)
	{}

	Measurement(uint8_t quality_, DNPTime time_) : quality(quality_), time(time_)
	{}

};


/// Common subclass to analogs and counters
template <class T>
class TypedMeasurement : public Measurement
{
public:

	T value;

	typedef T Type;

	bool no_value_change;

protected:

	TypedMeasurement(): Measurement(), value(), no_value_change(true) {}
	TypedMeasurement(uint8_t quality) : Measurement(quality), value(), no_value_change(true) {}
	TypedMeasurement(T value, uint8_t quality) : Measurement(quality), value(value), no_value_change(false) {}
	TypedMeasurement(T value, uint8_t quality, DNPTime time) : Measurement(quality, time), value(value), no_value_change(false) {}
	TypedMeasurement(uint8_t quality, DNPTime time) : Measurement(quality, time), no_value_change(true) {}
};

}


#endif
