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

#include "StartupIntegrityPoll.h"

#include "MasterTasks.h"

#include "opendnp3/app/APDUParser.h"
#include "opendnp3/app/APDUBuilders.h"

#include "opendnp3/master/MeasurementHandler.h"

#include "opendnp3/LogLevels.h"

#include <openpal/logging/LogMacros.h>

using namespace openpal;

namespace opendnp3
{

StartupIntegrityPoll::StartupIntegrityPoll(const MasterParams& params, ISOEHandler* pSOEHandler_, openpal::Logger* pLogger_) :
	PollTaskBase("Startup Integrity Poll", pSOEHandler_, pLogger_),
	expiration(0),
	pParams(&params)
{
	
}

void StartupIntegrityPoll::BuildRequest(APDURequest& request, uint8_t seq)
{
	rxCount = 0;
	build::ReadIntegrity(request, pParams->startupIntegrityClassMask, seq);
	request.SetFunction(FunctionCode::READ);
	request.SetControl(AppControlField::Request(seq));
}

openpal::MonotonicTimestamp StartupIntegrityPoll::ExpirationTime() const
{
	return pParams->startupIntegrityClassMask.HasAnyClass() ? expiration : MonotonicTimestamp::Max();
}
	
void StartupIntegrityPoll::OnFailure(const openpal::MonotonicTimestamp& now)
{
	expiration = now.Add(pParams->taskRetryPeriod);
}

void StartupIntegrityPoll::OnSuccess(const openpal::MonotonicTimestamp& now)
{
	expiration = MonotonicTimestamp::Max();
}

void StartupIntegrityPoll::OnLowerLayerClose(const openpal::MonotonicTimestamp& now)
{
	expiration = 0;
}


} //end ns
