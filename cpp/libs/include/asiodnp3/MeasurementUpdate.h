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
#ifndef ASIODNP3_MEASUREMENTUPDATE_H
#define ASIODNP3_MEASUREMENTUPDATE_H

#include "IOutstation.h"

#include <openpal/util/Uncopyable.h>
#include <asiopal/UTCTimeSource.h>

namespace asiodnp3
{

class ChangeSet;

/**
 *	A helper class that allows user code to update the measurement values in an outstation
 */
class MeasurementUpdate : private openpal::Uncopyable
{
public:

	/**
	*	Construct a MeasUpdate with a pointer to an outstation session. Apply the timestamp to all values used in Update(..) methods.
	*/
	MeasurementUpdate(IOutstation* outstation, openpal::UTCTimestamp timestamp);

	/**
	*	Construct a MeasUpdate with a pointer to an outstation session.
	*/
	MeasurementUpdate(IOutstation* outstation);

	~MeasurementUpdate();

	void Update(const opendnp3::Binary& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	void Update(const opendnp3::DoubleBitBinary& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	void Update(const opendnp3::Analog& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	void Update(const opendnp3::Counter& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	void Update(const opendnp3::FrozenCounter& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	void Update(const opendnp3::BinaryOutputStatus& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	void Update(const opendnp3::AnalogOutputStatus& meas, uint16_t index, opendnp3::EventMode mode = opendnp3::EventMode::Detect);
	void Update(const opendnp3::TimeAndInterval& meas, uint16_t index);

	void commit();

private:

	template <class T>
	void UpdateAny(const T& meas, uint16_t index, opendnp3::EventMode mode);

	IOutstation* m_outstation;
	openpal::UTCTimestamp m_timestamp;
	bool m_use_timestamp;
	ChangeSet* m_changes;
};



}

#endif
