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
#ifndef OPENDNP3_EVENTSCANTASK_H
#define OPENDNP3_EVENTSCANTASK_H

#include "opendnp3/master/PollTaskBase.h"

namespace opendnp3
{

	class ISOEHandler;

	/**
	* A generic interface for defining master request/response style tasks
	*/
	class EventScanTask : public PollTaskBase
	{

	public:

		EventScanTask(ISOEHandler* pSOEHandler_, openpal::Logger* pLogger_);

		virtual char const* Name() const override final { return "Event Scan"; }

		virtual void BuildRequest(APDURequest& request, const MasterParams& params, uint8_t seq) override final;

		virtual bool Enabled(const MasterParams& params) override final;

	private:

		virtual void OnFailure(const MasterParams& params, IMasterScheduler& scheduler) override final;

		virtual void OnSuccess(const MasterParams& params, IMasterScheduler& scheduler) override final;

	};

} //end ns


#endif
