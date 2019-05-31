/*
 * Copyright 2013-2019 Automatak, LLC
 *
 * Licensed to Green Energy Corp (www.greenenergycorp.com) and Automatak
 * LLC (www.automatak.com) under one or more contributor license agreements.
 * See the NOTICE file distributed with this work for additional information
 * regarding copyright ownership. Green Energy Corp and Automatak LLC license
 * this file to you under the Apache License, Version 2.0 (the "License"); you
 * may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TLSClientIOHandler.h"

#include "channel/tls/TLSStreamChannel.h"

#include "opendnp3/LogLevels.h"

#include <utility>

namespace opendnp3
{

TLSClientIOHandler::TLSClientIOHandler(const log4cpp::Logger& logger,
                                       const std::shared_ptr<IChannelListener>& listener,
                                       const std::shared_ptr<exe4cpp::StrandExecutor>& executor,
                                       TLSConfig config,
                                       const ChannelRetry& retry,
                                       const IPEndpointsList& remotes,
                                       std::string adapter)
    : IOHandler(logger, false, listener),
      executor(executor),
      config(std::move(config)),
      retry(retry),
      remotes(remotes),
      adapter(std::move(adapter))
{
}

void TLSClientIOHandler::ShutdownImpl()
{
    this->ResetState();
}

void TLSClientIOHandler::BeginChannelAccept()
{
    std::error_code ec;

    this->client = TLSClient::Create(logger, executor, adapter, config, ec);

    if (ec)
    {
        this->client.reset();
    }
    else
    {
        this->StartConnect(this->client, this->retry.minOpenRetry);
    }
}

void TLSClientIOHandler::SuspendChannelAccept()
{
    this->ResetState();
}

void TLSClientIOHandler::OnChannelShutdown()
{
    this->BeginChannelAccept();
}

void TLSClientIOHandler::StartConnect(const std::shared_ptr<TLSClient>& client, const TimeDuration& delay)
{
    auto cb = [=, self = shared_from_this()](const std::shared_ptr<exe4cpp::StrandExecutor>& executor,
                                             const std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>>& stream,
                                             const std::error_code& ec) -> void {
        if (ec)
        {
            FORMAT_LOG_BLOCK(this->logger, flags::WARN, "Error Connecting: %s", ec.message().c_str());

            ++this->statistics.numOpenFail;

            const auto newDelay = this->retry.NextDelay(delay);

            auto cb = [self, newDelay, client, this]() {
                this->remotes.Next();
                this->StartConnect(client, newDelay);
            };

            this->retrytimer = this->executor->start(delay.value, cb);
        }
        else
        {
            FORMAT_LOG_BLOCK(this->logger, flags::INFO, "Connected to: %s, port %u",
                             this->remotes.GetCurrentEndpoint().address.c_str(),
                             this->remotes.GetCurrentEndpoint().port);

            this->OnNewChannel(TLSStreamChannel::Create(executor, stream));
        }
    };

    FORMAT_LOG_BLOCK(this->logger, flags::INFO, "Connecting to: %s, port %u",
                     this->remotes.GetCurrentEndpoint().address.c_str(), this->remotes.GetCurrentEndpoint().port);

    this->client->BeginConnect(this->remotes.GetCurrentEndpoint(), cb);
}

void TLSClientIOHandler::ResetState()
{
    if (this->client)
    {
        this->client->Cancel();
        this->client.reset();
    }

    this->remotes.Reset();

    retrytimer.cancel();
}

} // namespace opendnp3
