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

#include "Outstation.h"

#include "opendnp3/app/APDUResponse.h"
#include "opendnp3/LogLevels.h"

#include <openpal/logging/LogMacros.h>

using namespace openpal;

namespace opendnp3
{

Outstation::Outstation(
		const OutstationConfig& config,
		const DatabaseTemplate& dbTemplate,		
		IExecutor& executor,
		openpal::LogRoot& root,
		ILowerLayer& lower,
		ICommandHandler& commandHandler,
		IOutstationApplication& application) :
		context(config, dbTemplate, executor, root, lower, commandHandler, application)
{
	
}
	
void Outstation::OnLowerLayerUp()
{
	if (context.isOnline)
	{
		SIMPLE_LOG_BLOCK(context.logger, flags::ERR, "already online");		
	}
	else
	{
		context.SetOnline();		
	}
}
	
void Outstation::OnLowerLayerDown()
{
	if (context.isOnline)
	{
		context.SetOffline();
	}
	else
	{
		SIMPLE_LOG_BLOCK(context.logger, flags::ERR, "not online");
	}
}

void Outstation::OnReceive(const openpal::ReadBufferView& fragment)
{
	if (context.isOnline)
	{
		context.OnReceiveAPDU(fragment);		
	}
	else
	{
		SIMPLE_LOG_BLOCK(context.logger, flags::ERR, "ignoring received data while offline");
	}
}

void Outstation::OnSendResult(bool isSuccess)
{	
	if (context.isOnline)
	{		
		context.OnSendResult(isSuccess);		
	}
	else
	{
		SIMPLE_LOG_BLOCK(context.logger, flags::ERR, "Unexpected send callback");
	}	
}

bool Outstation::OnLowerSend()
{
	return context.OnLowerSend();
}

void Outstation::SetRestartIIN()
{
	context.staticIIN.SetBit(IINBit::DEVICE_RESTART);
}

void Outstation::CheckForUpdates()
{
	context.CheckForNewEventData();
}

IDatabase& Outstation::GetDatabase()
{
	return context.database;
}

DatabaseConfigView Outstation::GetConfigView()
{
	return context.database.GetConfigView();
}
	
}


