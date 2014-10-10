
#include "Stdafx.h"
#include "SOEHandlerAdapter.h"
#include "Conversions.h"

namespace Automatak
{
	namespace DNP3
	{
		namespace Adapter
		{

			SOEHandlerAdapter::SOEHandlerAdapter(Automatak::DNP3::Interface::ISOEHandler^ aProxy) : proxy(aProxy)
			{}

			void SOEHandlerAdapter::Start()
			{
				proxy->Start();
			}

			void SOEHandlerAdapter::End()
			{
				proxy->End();
			}

			HeaderInfo^ SOEHandlerAdapter::GetInfo(const opendnp3::HeaderRecord& record, opendnp3::TimestampMode tsmode)
			{				
				return gcnew HeaderInfo((GroupVariation)record.enumeration, (QualifierCode)record.qualifier, (TimestampMode)tsmode);
			}

			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::Binary, uint16_t>>& meas)
			{
				auto values = ToEnumerable<Binary^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}

			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::DoubleBitBinary, uint16_t>>& meas)
			{
				auto values = ToEnumerable<DoubleBitBinary^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}

			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::Analog, uint16_t>>& meas)
			{
				auto values = ToEnumerable<Analog^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}
			
			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::Counter, uint16_t>>& meas)
			{
				auto values = ToEnumerable<Counter^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}
			
			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::FrozenCounter, uint16_t>>& meas)
			{
				auto values = ToEnumerable<FrozenCounter^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}
			
			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::BinaryOutputStatus, uint16_t>>& meas)
			{
				auto values = ToEnumerable<BinaryOutputStatus^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}
			
			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::AnalogOutputStatus, uint16_t>>& meas)
			{
				auto values = ToEnumerable<AnalogOutputStatus^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}
			
			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::OctetString, uint16_t>>& meas)
			{
				auto values = ToEnumerable<OctetString^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}

			void SOEHandlerAdapter::OnReceiveHeader(const opendnp3::HeaderRecord& header, opendnp3::TimestampMode tsmode, const opendnp3::IterableBuffer<opendnp3::IndexedValue<opendnp3::TimeAndInterval, uint16_t>>& meas)
			{
				auto values = ToEnumerable<TimeAndInterval^>(meas);
				proxy->OnReceiveHeader(GetInfo(header, tsmode), values);
			}
		}
	}
}
