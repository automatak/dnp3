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
#include "MasterStackImpl.h"

#include <opendnp3/app/APDUBuilders.h>

#include <asiopal/ASIOExecutor.h>
#include <asiopal/StrandGetters.h>

using namespace openpal;
using namespace opendnp3;

namespace asiodnp3
{

MasterStackImpl::MasterStackImpl(   const char* id,
									LogRoot& root_,
									asiopal::ASIOExecutor& executor,
									opendnp3::ISOEHandler& SOEHandler,                                    
									opendnp3::IMasterApplication& application,
                                    const MasterStackConfig& config,
                                    const StackActionHandler& handler_,									
									opendnp3::ITaskLock& taskLock) :
	root(root_, id),
	handler(handler_),
	stack(root, &executor, config.master.maxRxFragSize, &statistics, config.link),
	master(executor, root, stack.transport, SOEHandler, application, config.master, taskLock)
{
	stack.transport.SetAppLayer(&master);
}

ICommandProcessor* MasterStackImpl::GetCommandProcessor()
{
	return &master.GetCommandProcessor();
}

bool MasterStackImpl::Enable()
{
	return handler.EnableRoute(&stack.link);
}

bool MasterStackImpl::Disable()
{
	return handler.DisableRoute(&stack.link);
}

void MasterStackImpl::Shutdown()
{
	handler.Shutdown(&stack.link);
	shutdownAction.Apply();
}

StackStatistics MasterStackImpl::GetStackStatistics()
{	
	auto get = [this]() { return this->statistics; };
	return asiopal::SynchronouslyGet<StackStatistics>(handler.GetExecutor()->strand, get);
}

void MasterStackImpl::SetLinkRouter(opendnp3::ILinkRouter& router)
{
	stack.link.SetRouter(router);
}

void MasterStackImpl::SetShutdownAction(const openpal::Action0& action)
{
	shutdownAction = action;
}

opendnp3::ILinkSession* MasterStackImpl::GetLinkContext()
{
	return &stack.link;
}

MasterScan MasterStackImpl::AddScan(TimeDuration period, const std::function<void(HeaderWriter&)>& builder, int id)
{
	auto add = [this, period, builder, id]() { return master.AddScan(period, builder, id); };
	return asiopal::SynchronouslyGet<MasterScan>(handler.GetExecutor()->strand, add);
}

MasterScan MasterStackImpl::AddAllObjectsScan(GroupVariationID gvId, openpal::TimeDuration period, int id)
{
	auto add = [this, gvId, period, id]() { return master.AddAllObjectsScan(gvId, period, id); };
	return asiopal::SynchronouslyGet<MasterScan>(handler.GetExecutor()->strand, add);
}

MasterScan MasterStackImpl::AddClassScan(const ClassField& field, openpal::TimeDuration period, int id)
{	
	auto add = [this, field, period, id]() { return master.AddClassScan(field, period, id); };
	return asiopal::SynchronouslyGet<MasterScan>(handler.GetExecutor()->strand, add);
}

MasterScan  MasterStackImpl::AddRangeScan(opendnp3::GroupVariationID gvId, uint16_t start, uint16_t stop, openpal::TimeDuration period, int id)
{	
	auto add = [this, gvId, start, stop, period, id]() { return master.AddRangeScan(gvId, start, stop, period, id); };
	return asiopal::SynchronouslyGet<MasterScan>(handler.GetExecutor()->strand, add);
}

void MasterStackImpl::Scan(const std::function<void(opendnp3::HeaderWriter&)>& builder, int id)
{
	auto add = [this, builder, id]() { master.Scan(builder, id); };
	return asiopal::SynchronouslyExecute(handler.GetExecutor()->strand, add);
}

void MasterStackImpl::ScanAllObjects(opendnp3::GroupVariationID gvId, int id)
{
	auto add = [this, gvId, id]() { return master.ScanAllObjects(gvId, id); };
	return asiopal::SynchronouslyExecute(handler.GetExecutor()->strand, add);
}

void MasterStackImpl::ScanClasses(const opendnp3::ClassField& field, int id)
{
	auto add = [this, field, id]() { return master.ScanClasses(field, id); };
	return asiopal::SynchronouslyExecute(handler.GetExecutor()->strand, add);
}

void MasterStackImpl::ScanRange(opendnp3::GroupVariationID gvId, uint16_t start, uint16_t stop, int id)
{
	auto add = [this, gvId, start, stop, id]() { return master.ScanRange(gvId, start, stop, id); };
	return asiopal::SynchronouslyExecute(handler.GetExecutor()->strand, add);
}

}
