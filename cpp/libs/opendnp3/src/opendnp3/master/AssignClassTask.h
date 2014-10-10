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
#ifndef OPENDNP3_ASSIGNCLASSTASK_H
#define OPENDNP3_ASSIGNCLASSTASK_H

#include "opendnp3/master/NullResponseTask.h"
#include "opendnp3/master/TaskPriority.h"

namespace opendnp3
{

class IMasterApplication;

class AssignClassTask : public NullResponseTask
{	

public:	

	AssignClassTask(const MasterParams& params, IMasterApplication& application, openpal::Logger* pLogger);

	virtual TaskId Id() const override final { return TaskId::From(TaskIds::ASSIGN_CLASS); }

	virtual char const* Name() const override final { return "Assign Class"; }

	virtual bool IsRecurring() const override final { return true; }

	virtual void BuildRequest(APDURequest& request, uint8_t seq) override final;

	virtual int Priority(void) const override final { return priority::ASSIGN_CLASS; }

	virtual openpal::MonotonicTimestamp ExpirationTime() const override final;

	virtual bool BlocksLowerPriority() const { return true; }

	virtual void OnLowerLayerClose(const openpal::MonotonicTimestamp& now) override final;

	virtual void Demand() override final;

private:

	const MasterParams* pParams;

	IMasterApplication* pApplication;

	openpal::MonotonicTimestamp expiration;

	virtual void OnSuccess(const openpal::MonotonicTimestamp& now) override final;

	virtual void OnTimeoutOrBadControlOctet(const openpal::MonotonicTimestamp& now) override final;

};


} //end ns


#endif
