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

#ifndef __EVENT_BUFFER_FACADE_H_
#define __EVENT_BUFFER_FACADE_H_

#include "Event.h"

#include <openpal/RandomInsertAdapter.h>
#include <openpal/DoublyLinkedListAdapter.h>
#include <opendnp3/DataTypes.h>

#include "EventType.h"

#include "EventCount.h"

namespace opendnp3
{

struct SequenceRecord
{
	EventType type;
	int16_t index;
	EventClass clazz;
	bool selected;
};

struct EventBufferFacade
{
	public:

	EventBufferFacade(	openpal::RandomInsertAdapter<Event<Binary>, uint16_t> aBinaryEvents,
						openpal::RandomInsertAdapter<Event<Analog>, uint16_t> aAnalogEvents,
						openpal::RandomInsertAdapter<Event<Counter>, uint16_t> aCounterEvents,
						openpal::RandomInsertAdapter<Event<FrozenCounter>, uint16_t> aFrozenCounterEvents,						
						openpal::DoublyLinkedListAdapter<SequenceRecord, uint16_t> aSequenceOfEvents,
						openpal::StackAdapter<openpal::DoubleListNode<SequenceRecord>*, uint16_t> aSelectedEvents) :

		binaryEvents(aBinaryEvents),
		analogEvents(aAnalogEvents),
		counterEvents(aCounterEvents),
		frozenCounterEvents(aFrozenCounterEvents),		
		sequenceOfEvents(aSequenceOfEvents),
		selectedEvents(aSelectedEvents)
	{
		
	}

	openpal::RandomInsertAdapter<Event<Binary>, uint16_t> binaryEvents;
	openpal::RandomInsertAdapter<Event<Analog>, uint16_t> analogEvents;
	openpal::RandomInsertAdapter<Event<Counter>, uint16_t> counterEvents;	
	openpal::RandomInsertAdapter<Event<FrozenCounter>, uint16_t> frozenCounterEvents;
	openpal::DoublyLinkedListAdapter<SequenceRecord, uint16_t> sequenceOfEvents;
	openpal::StackAdapter<openpal::DoubleListNode<SequenceRecord>*, uint16_t> selectedEvents;
};


}

#endif
