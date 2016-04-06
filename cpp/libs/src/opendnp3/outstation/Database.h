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
#ifndef OPENDNP3_DATABASE_H
#define OPENDNP3_DATABASE_H

#include "opendnp3/gen/IndexMode.h"
#include "opendnp3/gen/AssignClassType.h"

#include "opendnp3/outstation/IDatabase.h"
#include "opendnp3/outstation/IEventReceiver.h"
#include "opendnp3/outstation/DatabaseBuffers.h"

namespace opendnp3
{

/**
The database coordinates all updates of measurement data
*/
class Database : public IDatabase, private openpal::Uncopyable
{
public:
	Database(const DatabaseTemplate&, IndexMode indexMode_, StaticTypeBitField allowedClass0Types);

	// ------- IDatabase --------------

	virtual bool Update(const Binary&, uint16_t, EventMode = EventMode::Detect) override final;
	virtual bool Update(const DoubleBitBinary&, uint16_t, EventMode = EventMode::Detect) override final;
	virtual bool Update(const Analog&, uint16_t, EventMode = EventMode::Detect) override final;
	virtual bool Update(const Counter&, uint16_t, EventMode = EventMode::Detect) override final;
	virtual bool Update(const FrozenCounter&, uint16_t, EventMode = EventMode::Detect) override final;
	virtual bool Update(const BinaryOutputStatus&, uint16_t, EventMode = EventMode::Detect) override final;
	virtual bool Update(const AnalogOutputStatus&, uint16_t, EventMode = EventMode::Detect) override final;
	virtual bool Update(const TimeAndInterval&, uint16_t) override final;

	// ------- Misc ---------------

	IResponseLoader& GetResponseLoader() override final { return buffers; }
	IStaticSelector& GetStaticSelector() override final { return buffers; }
	IClassAssigner& GetClassAssigner() override final { return buffers; }

	void SetEventReceiver(IEventReceiver *eventReceiver) override final;

	/**
	* @return A view of all the static data for configuration purposes
	*/
	DatabaseConfigView GetConfigView()
	{
		return buffers.buffers.GetView();
	}

private:

	template <class T>
	uint16_t GetRawIndex(uint16_t index);

	IEventReceiver* pEventReceiver;
	IndexMode indexMode;


	static bool ConvertToEventClass(PointClass pc, EventClass& ec);

	template <class T>
	bool UpdateEvent(const T& value, uint16_t index, EventMode mode);

	template <class T>
	bool UpdateAny(Cell<T>& cell, const T& value, EventMode mode);

	// stores the most recent values, selected values, and metadata
	DatabaseBuffers buffers;
};

template <class T>
uint16_t Database::GetRawIndex(uint16_t index)
{
	if (indexMode == IndexMode::Contiguous)
	{
		return index;
	}
	else
	{
		auto view = buffers.buffers.GetArrayView<T>();
		auto result = IndexSearch::FindClosestRawIndex(view, index);
		return result.match ? result.index : openpal::MaxValue<uint16_t>();
	}
}

template <class T>
bool Database::UpdateEvent(const T& value, uint16_t index, EventMode mode)
{
	auto rawIndex = GetRawIndex<T>(index);
	auto view = buffers.buffers.GetArrayView<T>();

	if (view.Contains(rawIndex))
	{
		this->UpdateAny(view[rawIndex], value, mode);
		return true;
	}
	else
	{
		return false;
	}
}

template <class T>
bool Database::UpdateAny(Cell<T>& cell, const T& value, EventMode mode)
{
	EventClass ec;
	if (ConvertToEventClass(cell.metadata.clazz, ec))
	{
		bool createEvent = false;

		switch (mode)
		{
		case(EventMode::Force) :
			createEvent = true;
			break;
		case(EventMode::Detect):
			createEvent = cell.metadata.IsEvent(value);
			break;
		default:
			break;
		}

		if (createEvent)
		{
			cell.metadata.lastEvent = value;

			if (pEventReceiver)
			{
				pEventReceiver->Update(Event<T>(value, cell.vIndex, ec, cell.metadata.variation));
			}
		}
	}

	if (value.no_value_change)
	{
		// this is a lot quicker to code and a lot less error-prone that copying the members one by one :)
		typename T::Type temp(cell.value.value);
		cell.value = value;
		cell.value.value = temp;
	}
	else
	{
		cell.value = value;
	}
	return true;
}

}

#endif

