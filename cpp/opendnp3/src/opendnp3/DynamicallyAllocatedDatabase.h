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
#ifndef __DYNAMICICALLY_ALLOCATED_DATABASE_H_
#define __DYNAMICICALLY_ALLOCATED_DATABASE_H_

#include "StaticDataFacade.h"
#include <opendnp3/DatabaseConfiguration.h>

#include <openpal/DynamicArray.h>

namespace opendnp3
{

class DynamicallyAllocatedDatabase
{	
	public:

	DynamicallyAllocatedDatabase(const DatabaseTemplate& databaseTemplate);

	StaticDataFacade GetFacade();	

	void Configure(const DatabaseConfiguration& dataTemplate);

	private:
	
	openpal::DynamicArray<Binary, uint16_t> binaryValues;
	openpal::DynamicArray<Analog, uint16_t> analogValues;
	openpal::DynamicArray<Counter, uint16_t> counterValues;
	openpal::DynamicArray<FrozenCounter, uint16_t> frozenCounterValues;
	openpal::DynamicArray<ControlStatus, uint16_t> controlStatusValues;
	openpal::DynamicArray<SetpointStatus, uint16_t> setpointStatusValues;

	openpal::DynamicArray<BinaryMetadata, uint16_t> binaryMetadata;
	openpal::DynamicArray<AnalogMetadata, uint16_t> analogMetadata;
	openpal::DynamicArray<CounterMetadata, uint16_t> counterMetadata;
	openpal::DynamicArray<FrozenCounterMetadata, uint16_t> frozenCounterMetadata;
};

}

#endif
