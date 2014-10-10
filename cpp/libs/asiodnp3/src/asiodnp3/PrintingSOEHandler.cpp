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

#include "PrintingSOEHandler.h"

using namespace std;

namespace opendnp3
{

PrintingSOEHandler PrintingSOEHandler::instance;

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Binary, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
}

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<DoubleBitBinary, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
}

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Analog, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
}

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Counter, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
}

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<FrozenCounter, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
}

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<BinaryOutputStatus, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
}

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<AnalogOutputStatus, uint16_t>>& meas)
{
	Print(header, meas, tsmode);
}

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<TimeAndInterval, uint16_t>>& meas)
{
	PrintHeaderInfo(header, tsmode);

	meas.foreach([&](const IndexedValue<TimeAndInterval, uint16_t>& pair)
	{		
			std::cout << "TimeAndInterval: " <<
			"[" << pair.index << "] : " <<
			pair.value.time << " : " <<
			pair.value.interval << " : " << 
			IntervalUnitsToString(pair.value.GetUnitsEnum()) << std::endl;
	});
}

void PrintingSOEHandler::OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<OctetString, uint16_t>>& meas)
{
	this->PrintHeaderInfo(header, tsmode);

	meas.foreach([&](const IndexedValue<OctetString, uint16_t>& pair)
	{
		std::cout << "OctetString " <<  " [" << pair.index << "] : Size : " << pair.value.ToReadOnly().Size() << std::endl;
	});
}

}
