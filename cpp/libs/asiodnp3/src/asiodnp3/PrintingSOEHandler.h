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
#ifndef ASIODNP3_PRINTINGSOEHANDLER_H
#define ASIODNP3_PRINTINGSOEHANDLER_H

#include <opendnp3/master/ISOEHandler.h>

#include <iostream>
#include <sstream>

namespace opendnp3
{

class PrintingSOEHandler : public ISOEHandler
{

public:
	static ISOEHandler& Instance()
	{
		return instance;
	}

	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Binary, uint16_t>>& meas) override final;
	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<DoubleBitBinary, uint16_t>>& meas) override final;
	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Analog, uint16_t>>& meas) override final;
	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<Counter, uint16_t>>& meas) override final;
	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<FrozenCounter, uint16_t>>& meas) override final;
	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<BinaryOutputStatus, uint16_t>>& meas) override final;
	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<AnalogOutputStatus, uint16_t>>& meas) override final;
	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<OctetString, uint16_t>>& meas) override final;
	void OnReceiveHeader(const HeaderRecord& header, TimestampMode tsmode, const IterableBuffer<IndexedValue<TimeAndInterval, uint16_t>>& meas) override final;
	

protected:

	void Start() final {}
	void End() final {}

private:

	static void PrintHeaderInfo(const HeaderRecord& header, TimestampMode tsmode)
	{
		std::cout << "Header: " << " Group" << static_cast<int>(header.group) << "Var" << static_cast<int>(header.variation);
		std::cout << QualifierCodeToString(header.qualifier) << " timestamps: " << GetTimeString(tsmode) << std::endl;
	}

	template <class T>
	static void Print(const HeaderRecord& header, const IterableBuffer<IndexedValue<T, uint16_t>>& buffer, TimestampMode tsmode)
	{
		PrintHeaderInfo(header, tsmode);

		buffer.foreach([&](const IndexedValue<T, uint16_t>& pair)
		{

			std::cout << "[" << pair.index << "] : " <<
			          ValueToString(pair.value) << " : " <<
			          static_cast<int>(pair.value.quality) << " : " <<
					  pair.value.time << std::endl;
		});
	}	

	template <class T>
	static std::string ValueToString(const T& meas)
	{
		std::ostringstream oss;
		oss << meas.value;
		return oss.str();
	}

	static std::string GetTimeString(TimestampMode tsmode)
	{
		std::ostringstream oss;	
		switch (tsmode)
		{
			case(TimestampMode::SYNCHRONIZED) :
				return "synchronized";
				break;
			case(TimestampMode::UNSYNCHRONIZED):
				oss << "unsynchronized";
				break;
			default:
				oss << "no timestamp";
				break;
		}
		
		return oss.str();
	}

	static std::string ValueToString(const DoubleBitBinary& meas)
	{
		return DoubleBitToString(meas.value);
	}

	PrintingSOEHandler()
	{}

	static PrintingSOEHandler instance;
};

}

#endif
