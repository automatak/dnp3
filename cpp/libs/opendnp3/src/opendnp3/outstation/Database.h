/**
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

#include <openpal/executor/IMutex.h>
#include <openpal/executor/Action0.h>
#include <openpal/container/Settable.h>

#include "opendnp3/gen/AssignClassType.h"
#include "opendnp3/app/StaticRange.h"
#include "opendnp3/outstation/IEventBuffer.h"
#include "opendnp3/outstation/IDatabase.h"
#include "opendnp3/outstation/DatabaseBuffers.h"

namespace opendnp3
{

/**
Manages the static data model of a DNP3 outstation. Dual-interface to update data points and read current values.

Passes data updates to an associated event buffer for event generation/management.
*/
class Database : public IDatabase
{
public:

	Database(const DatabaseTemplate&, openpal::IMutex* pMutex = nullptr);	

	void SetEventBuffer(IEventBuffer& eventBuffer);

	void DoubleBuffer();

	// IMeasurementLoader*functions
	void Update(const Binary& value, uint16_t) override final;
	void Update(const DoubleBitBinary& value, uint16_t) override final;
	void Update(const Analog& value, uint16_t) override final;
	void Update(const Counter& value, uint16_t) override final;
	void Update(const FrozenCounter& value, uint16_t) override final;
	void Update(const BinaryOutputStatus& value, uint16_t) override final;
	void Update(const AnalogOutputStatus& value, uint16_t) override final;
	void Update(const TimeAndInterval& value, uint16_t) override final;

	template <class T>
	openpal::Indexable<DualValue<T>, uint16_t>& Values();

	template <class T>
	uint16_t NumValues() const;

	bool AssignClass(AssignClassType type, PointClass clazz, const StaticRange& range);

	template <class T>
	StaticRange FullRange() const;

	DatabaseBuffers buffers;
	
	void SetEventHandler(const openpal::Action0& callback);

private:

	template <class T>
	bool AssignClassTo(T& metadata, PointClass clazz, const StaticRange& range)
	{
        StaticRange rangit(range);
		if (rangit.IsDefined() && (range.IsContainedBy(metadata.Size())))
        {
            while(rangit.IsDefined())
            {
                metadata[rangit.position].clazz = clazz;
                rangit.Advance();
            }
            return true;
        }
        else
        {
            return false;
        }
	}

	template <class T>
	static void FreezeCollection(openpal::Indexable<DualValue<T>, uint16_t>& collection)
	{
		collection.foreach([](DualValue<T>& value)
		{
			value.Freeze();
		});
	}

	Database();
	Database(const Database&);

	template <class T, class U>
	void UpdateEvent(const T& value, uint16_t index, U& collection)
	{
        auto position = collection.indexes.GetPosition(index);
		if(collection.values.Contains(position))
		{
			auto& metadata = collection.metadata[position];
			EventClass eventClass;
			if (metadata.GetEventClass(eventClass) && metadata.CheckForEvent(value))
			{
				if (pEventBuffer)
				{
					pEventBuffer->Update(Event<T>(value, index, eventClass));		
					transactionHasEvents = true;
				}
			}
			collection.values[position].Update(value);
		}
	}

	IEventBuffer* pEventBuffer;
	

	openpal::IMutex* pMutex;
	openpal::Settable<openpal::Action0> onEventAction;

	bool transactionHasEvents;

	// ITransactable  functions, proxies to the given transactable

	virtual void Start() override final;
	virtual void End() override final;
};

}

#endif

