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

#ifndef ASIODNP3_DNP3MANAGERIMPL_H
#define ASIODNP3_DNP3MANAGERIMPL_H

#include <openpal/logging/ILogHandler.h>
#include <openpal/util/Uncopyable.h>

#include <asiopal/ThreadPool.h>
#include <asiopal/SerialTypes.h>
#include <asiopal/TLSConfig.h>

#include <opendnp3/LogLevels.h>

#include "asiodnp3/ChannelSet.h"

namespace asiodnp3
{

class DNP3ManagerImpl : private openpal::Uncopyable
{

public:

	DNP3ManagerImpl(
	    uint32_t concurrencyHint,
	    std::shared_ptr<openpal::ILogHandler> handler,
	    std::function<void()> onThreadStart,
	    std::function<void()> onThreadExit
	);

	void Shutdown();

	IChannel* AddTCPClient(
	    const std::string& id,
	    uint32_t levels,
	    const opendnp3::ChannelRetry& retry,
	    const std::string& host,
	    const std::string& local,
	    uint16_t port,
	    std::shared_ptr<IChannelListener> listener);

	IChannel* AddTCPServer(
	    const std::string& id,
	    uint32_t levels,
	    const opendnp3::ChannelRetry& retry,
	    const std::string& endpoint,
	    uint16_t port,
	    std::shared_ptr<IChannelListener> listener);

	IChannel* AddSerial(
	    const std::string& id,
	    uint32_t levels,
	    const opendnp3::ChannelRetry& retry,
	    asiopal::SerialSettings settings,
	    std::shared_ptr<IChannelListener> listener);

	IChannel* AddTLSClient(
	    const std::string& id,
	    uint32_t levels,
	    const opendnp3::ChannelRetry& retry,
	    const std::string& host,
	    const std::string& local,
	    uint16_t port,
	    const asiopal::TLSConfig& config,
	    std::shared_ptr<IChannelListener> listener,
	    std::error_code& ec);

	IChannel* AddTLSServer(
	    const std::string& id,
	    uint32_t levels,
	    const opendnp3::ChannelRetry& retry,
	    const std::string& endpoint,
	    uint16_t port,
	    const asiopal::TLSConfig& config,
	    std::shared_ptr<IChannelListener> listener,
	    std::error_code& ec);

private:

	std::shared_ptr<openpal::ILogHandler> handler;
	asiopal::ThreadPool threadpool;
	ChannelSet channels;
};

}

#endif
