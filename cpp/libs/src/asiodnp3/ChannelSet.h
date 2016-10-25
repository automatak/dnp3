/*
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
#ifndef ASIODNP3_CHANNELSET_H
#define ASIODNP3_CHANNELSET_H

#include <set>

#include <cstdint>
#include <memory>

#include <openpal/logging/Logger.h>
#include <openpal/executor/TimeDuration.h>

#include <opendnp3/link/ChannelRetry.h>

#include "asiodnp3/IChannelListener.h"

namespace asiopal
{
class ASIOExecutor;
class PhysicalLayerASIO;
}

namespace asiodnp3
{

class IChannel;
class DNP3Channel;

class ChannelSet
{

public:

	~ChannelSet();

	IChannel* CreateChannel(	std::unique_ptr<openpal::LogRoot> root,
	                            const opendnp3::ChannelRetry& retry,
	                            std::shared_ptr<IChannelListener> listener,
	                            std::unique_ptr<asiopal::PhysicalLayerASIO> phys);


	/// Synchronously shutdown all channels. Block until complete.
	void Shutdown();

private:

	std::set<DNP3Channel*> channels;

	void OnShutdown(DNP3Channel* pChannel);
};

}

#endif
