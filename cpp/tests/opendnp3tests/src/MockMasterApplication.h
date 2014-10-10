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
#ifndef __MOCK_MASTER_APPLICATION_H_
#define __MOCK_MASTER_APPLICATION_H_

#include <opendnp3/master/IMasterApplication.h>

#include <vector>

namespace opendnp3
{

class MockMasterApplication : public IMasterApplication
{

public:

	MockMasterApplication() : time(0)
	{}

	virtual openpal::UTCTimestamp Now() override final
	{
		return openpal::UTCTimestamp(time);
	}

	virtual void OnReceiveIIN(const IINField& iin) override final
	{
		rxIIN.push_back(iin);
	}

	virtual void OnTaskStateChange(TaskId id, TaskState state) override final
	{
		taskEvents.push_back(std::pair<TaskId, TaskState>(id, state));
	}

	virtual bool AssignClassDuringStartup() override final
	{
		return assignClass ? true : false;
	}
	
	virtual void ConfigureAssignClassRequest(HeaderWriter& writer) override final
	{
		return assignClass(writer);
	}
	

	std::function<void(HeaderWriter&)> assignClass;

	std::vector<IINField> rxIIN;

	std::vector<std::pair<TaskId, TaskState>> taskEvents;

	uint64_t time;
};


}

#endif

