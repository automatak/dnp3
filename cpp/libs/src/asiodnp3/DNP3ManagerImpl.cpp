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

#include "DNP3ManagerImpl.h"

#include <opendnp3/LogLevels.h>

#include <asiopal/PhysicalLayerSerial.h>
#include <asiopal/PhysicalLayerTCPClient.h>
#include <asiopal/PhysicalLayerTCPServer.h>

#ifdef OPENDNP3_USE_TLS
#include <asiopal/tls/PhysicalLayerTLSClient.h>
#include <asiopal/tls/PhysicalLayerTLSServer.h>
#endif

#include "asiodnp3/ErrorCodes.h"

using namespace openpal;

namespace asiodnp3
{

DNP3ManagerImpl::DNP3ManagerImpl(
    uint32_t concurrencyHint,
    std::shared_ptr<openpal::ILogHandler> handler,
    std::function<void()> onThreadStart,
    std::function<void()> onThreadExit
) :
	handler(handler),
	threadpool(handler.get(), opendnp3::flags::INFO, concurrencyHint, onThreadStart, onThreadExit)
{}

void DNP3ManagerImpl::Shutdown()
{
	channels.Shutdown();
}

IChannel* DNP3ManagerImpl::AddTCPClient(
    const std::string& id,
    uint32_t levels,
    const opendnp3::ChannelRetry& retry,
    const std::string& host,
    const std::string& local,
    uint16_t port,
    std::shared_ptr<IChannelListener> listener)
{
	auto root = std::unique_ptr<LogRoot>(new LogRoot(handler.get(), id.c_str(), levels));
	auto phys = std::unique_ptr<asiopal::PhysicalLayerTCPClient>(new asiopal::PhysicalLayerTCPClient(root->logger, this->threadpool.GetIOService(), host, local, port));
	return this->channels.CreateChannel(std::move(root), retry, listener, std::move(phys));
}

IChannel* DNP3ManagerImpl::AddTCPServer(
    const std::string& id,
    uint32_t levels,
    const opendnp3::ChannelRetry& retry,
    const std::string& endpoint,
    uint16_t port,
    std::shared_ptr<IChannelListener> listener)
{

	auto root = std::unique_ptr<LogRoot>(new LogRoot(this->handler.get(), id.c_str(), levels));
	auto phys = std::unique_ptr<asiopal::PhysicalLayerTCPServer>(
	                new asiopal::PhysicalLayerTCPServer(root->logger, this->threadpool.GetIOService(), endpoint, port)
	            );
	return this->channels.CreateChannel(std::move(root), retry, listener, std::move(phys));
}

IChannel* DNP3ManagerImpl::AddSerial(
    const std::string& id,
    uint32_t levels,
    const opendnp3::ChannelRetry& retry,
    asiopal::SerialSettings settings,
    std::shared_ptr<IChannelListener> listener)
{

	auto root = std::unique_ptr<LogRoot>(new LogRoot(this->handler.get(), id.c_str(), levels));
	auto phys = std::unique_ptr<asiopal::PhysicalLayerSerial>(
	                new asiopal::PhysicalLayerSerial(root->logger, this->threadpool.GetIOService(), settings)
	            );
	return this->channels.CreateChannel(std::move(root), retry, listener, std::move(phys));
}

IChannel* DNP3ManagerImpl::AddTLSClient(
    const std::string& id,
    uint32_t levels,
    const opendnp3::ChannelRetry& retry,
    const std::string& host,
    const std::string& local,
    uint16_t port,
    const asiopal::TLSConfig& config,
    std::shared_ptr<IChannelListener> listener,
    std::error_code& ec)
{

#ifdef OPENDNP3_USE_TLS

	auto root = std::unique_ptr<LogRoot>(new LogRoot(this->handler.get(), id.c_str(), levels));
	auto phys = std::unique_ptr<asiopal::PhysicalLayerTLSClient>(
	                new asiopal::PhysicalLayerTLSClient(root->logger, this->threadpool.GetIOService(), host, local, port, config, ec)
	            );
	return ec ? nullptr : this->channels.CreateChannel(std::move(root), retry, listener, std::move(phys));
#else
	ec = Error::NO_TLS_SUPPORT;
	return nullptr;
#endif

}

IChannel* DNP3ManagerImpl::AddTLSServer(
    const std::string& id,
    uint32_t levels,
    const opendnp3::ChannelRetry& retry,
    const std::string& endpoint,
    uint16_t port,
    const asiopal::TLSConfig& config,
    std::shared_ptr<IChannelListener> listener,
    std::error_code& ec)
{

#ifdef OPENDNP3_USE_TLS
	auto root = std::unique_ptr<LogRoot>(new LogRoot(this->handler.get(), id.c_str(), levels));
	auto phys = std::unique_ptr<asiopal::PhysicalLayerTLSServer>(
	                new asiopal::PhysicalLayerTLSServer(root->logger, this->threadpool.GetIOService(), endpoint, port, config, ec)
	            );
	return ec ? nullptr : this->channels.CreateChannel(std::move(root), retry, listener, std::move(phys));
#else
	ec = Error::NO_TLS_SUPPORT;
	return nullptr;
#endif

}

}

