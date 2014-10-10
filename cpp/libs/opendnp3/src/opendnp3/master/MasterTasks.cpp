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

#include "MasterTasks.h"

using namespace openpal;

namespace opendnp3
{

MasterTasks::MasterTasks(const MasterParams& params, openpal::Logger* pLogger, IMasterApplication& application, ISOEHandler& SOEHandler, openpal::IUTCTimeSource& timeSource) :
enableUnsol(params, pLogger),
clearRestart(params, pLogger),
assignClass(params, application, pLogger),
startupIntegrity(params, &SOEHandler, pLogger),
disableUnsol(params, pLogger),
timeSync(pLogger, &timeSource)
{
	
}

void MasterTasks::Initialize(MasterScheduler& scheduler)
{
	scheduler.Schedule(ManagedPtr<IMasterTask>::WrapperOnly(&enableUnsol));
	scheduler.Schedule(ManagedPtr<IMasterTask>::WrapperOnly(&clearRestart));
	scheduler.Schedule(ManagedPtr<IMasterTask>::WrapperOnly(&assignClass));
	scheduler.Schedule(ManagedPtr<IMasterTask>::WrapperOnly(&startupIntegrity));
	scheduler.Schedule(ManagedPtr<IMasterTask>::WrapperOnly(&disableUnsol));
	scheduler.Schedule(ManagedPtr<IMasterTask>::WrapperOnly(&timeSync));

	for (auto& pTask : boundTasks)
	{
		scheduler.Schedule(ManagedPtr<IMasterTask>::WrapperOnly(pTask.get()));
	}
}

void MasterTasks::BindTask(IMasterTask* pTask)
{
	boundTasks.push_back(std::unique_ptr<IMasterTask>(pTask));
}

}
