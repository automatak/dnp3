#ifndef __SOE_HANDLER_ADAPTER_H_
#define __SOE_HANDLER_ADAPTER_H_

using namespace System::Collections::ObjectModel;

#include <opendnp3/master/ISOEHandler.h>
#include <vcclr.h>

#include "EnumerableConversions.h"

using namespace Automatak::DNP3::Interface;

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			private class SOEHandlerAdapter : public opendnp3::ISOEHandler
			{
			public:

				SOEHandlerAdapter(Automatak::DNP3::Interface::ISOEHandler^ proxy);

				virtual void Start() override final;
				virtual void End() override final;

				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::Binary, uint16_t>>& meas) override final;
				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::DoubleBitBinary, uint16_t>>& meas) override final;
				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::Analog, uint16_t>>& meas) override final;
				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::Counter, uint16_t>>& meas) override final;
				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::FrozenCounter, uint16_t>>& meas) override final;
				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::BinaryOutputStatus, uint16_t>>& meas) override final;
				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::AnalogOutputStatus, uint16_t>>& meas) override final;
				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::OctetString, uint16_t>>& meas) override final;			
				virtual void OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::TimeAndInterval, uint16_t>>& meas) override final;

			private:

				static HeaderInfo^ GetInfo(const opendnp3::HeaderRecord& record, opendnp3::TimestampMode tsmode);

				/*
				template <class Target, class Source>
				static System::Collections::Generic::IEnumerable<IndexedValue<Target>^>^ ToEnumerable(const opendnp3::IterableBuffer<opendnp3::IndexedValue<Source, uint16_t>>& meas)
				{
					auto list = gcnew System::Collections::Generic::List<IndexedValue<Target>^>();
					auto iterator = meas.Iterate();

					do
					{
						auto current = iterator.Current();
						auto value = gcnew IndexedValue<Target>(Conversions::ConvertMeas(current.value), current.index);
						list->Add(value);
					} while (iterator.MoveNext());

					return list;
				}
				*/

				gcroot < Automatak::DNP3::Interface::ISOEHandler^ > proxy;
			};

		}
	}
}

#endif
