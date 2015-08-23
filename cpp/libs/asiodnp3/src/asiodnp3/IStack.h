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
#ifndef ASIODNP3_ISTACK_H
#define ASIODNP3_ISTACK_H

#include "DestructorHook.h"

#include <openpal/executor/IExecutor.h>

#include <opendnp3/StackStatistics.h>
#include <opendnp3/link/ILinkStatusListener.h>

#include <assert.h>
#include <vector>
#include <functional>


namespace asiodnp3
{

/**
* Base class for masters or outstations
*/
class IStack : public DestructorHook , public opendnp3::ILinkStatusListener
{
public:	

	IStack() : current_status(opendnp3::LinkStatus::TIMEOUT) {}

	virtual ~IStack() {}	

	/**
	* Synchronously enable communications
	*/
	virtual bool Enable() = 0;

	/**
	* Synchronously disable communications
	*/
	virtual bool Disable() = 0;

	/**
	* Synchronously shutdown the endpoint. No more calls are allowed after this call.
	*/
	virtual void Shutdown() = 0;

	/*
	* Receive callbacks for link reset/unreset transitions
	*/
	void AddLinkStatusListener(const std::function<void(opendnp3::LinkStatus)>& listener)
	{
		listener(current_status);
		callbacks.push_back(listener);
	};

	virtual void OnStateChange(opendnp3::LinkStatus status)
	{
		current_status = status;
		for (auto& cb : callbacks)
		{
			cb(status);
		}
	}

private:
	std::vector<std::function<void(opendnp3::LinkStatus)>> callbacks;
	opendnp3::LinkStatus current_status;
};

}

#endif
