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

#include "WriteHandler.h"

#include <openpal/logging/LogMacros.h>

#include "opendnp3/ErrorCodes.h"

using namespace openpal;

namespace opendnp3
{

WriteHandler::WriteHandler(openpal::Logger& aLogger, IOutstationApplication& application, IINField* pWriteIIN_) :
	APDUHandlerBase(aLogger),
	pApplication(&application),
	pWriteIIN(pWriteIIN_),
	wroteTime(false),
	wroteIIN(false)
{}

void WriteHandler::_OnIIN(const HeaderRecord&, const IterableBuffer<IndexedValue<bool, uint16_t>>& meas)
{
	IndexedValue<bool, uint16_t> v;
	if(meas.ReadOnlyValue(v))
	{
		if (wroteIIN) errors.SetBit(IINBit::PARAM_ERROR);
		else
		{
			if (v.index == static_cast<int>(IINBit::DEVICE_RESTART))
			{
				if (v.value) errors.SetBit(IINBit::PARAM_ERROR);
				else
				{
					wroteIIN = true;
					pWriteIIN->ClearBit(IINBit::DEVICE_RESTART);
				}
			}
			else errors.SetBit(IINBit::PARAM_ERROR);
		}
	}
	else errors.SetBit(IINBit::PARAM_ERROR);
}

void WriteHandler::_OnCountOf(const HeaderRecord&, const IterableBuffer<Group50Var1>& times)
{
	if (wroteTime)
	{
		errors.SetBit(IINBit::PARAM_ERROR);
	}
	else
	{		
		Group50Var1 time;
		if (times.ReadOnlyValue(time))
		{
			if (pApplication->SupportsWriteAbsoluteTime())
			{
				wroteTime = true;				
				
				bool accepted = pApplication->WriteAbsoluteTime(UTCTimestamp(time.time));
				
				if (!accepted)
				{
					errors.SetBit(IINBit::PARAM_ERROR);
				}
			}
			else
			{
				errors.SetBit(IINBit::FUNC_NOT_SUPPORTED);
			}
		}
		else
		{
			errors.SetBit(IINBit::PARAM_ERROR);
		}		
	}
}

void WriteHandler::_OnIndexPrefix(const HeaderRecord& record, TimestampMode tsmode, const IterableBuffer<IndexedValue<TimeAndInterval, uint16_t>>& meas)
{
	if (pApplication->SupportsWriteTimeAndInterval())
	{
		if (!pApplication->WriteTimeAndInterval(meas))
		{
			errors.SetBit(IINBit::PARAM_ERROR);
		}
	}
	else
	{
		errors.SetBit(IINBit::FUNC_NOT_SUPPORTED);
	}
}

}


