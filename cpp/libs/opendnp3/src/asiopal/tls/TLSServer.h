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
#ifndef OPENDNP3_TLSSERVER_H
#define OPENDNP3_TLSSERVER_H

#include <openpal/logging/Logger.h>
#include <openpal/util/Uncopyable.h>

#include "asiopal/Executor.h"
#include "asiopal/IListener.h"
#include "asiopal/IPEndpoint.h"
#include "asiopal/TLSConfig.h"
#include "asiopal/tls/SSLContext.h"

namespace opendnp3
{
/**
 * Binds and listens on an IPv4 TCP port. Implement TLS negotiation.
 *
 * Meant to be used exclusively as a shared_ptr
 */
class TLSServer : public IListener, public std::enable_shared_from_this<TLSServer>, private openpal::Uncopyable
{

public:
    TLSServer(const log4cpp::Logger& logger,
              const std::shared_ptr<Executor>& executor,
              const IPEndpoint& endpoint,
              const TLSConfig& config,
              std::error_code& ec);

    /// Stop listening for connections, permanently shutting down the listener
    void Shutdown() override;

protected:
    // Inherited classes must implement these methods

    virtual bool AcceptConnection(uint64_t sessionid, const asio::ip::tcp::endpoint& remote) = 0;
    virtual bool VerifyCallback(uint64_t sessionid, bool preverified, asio::ssl::verify_context& ctx) = 0;
    virtual void AcceptStream(uint64_t sessionid,
                              const std::shared_ptr<Executor>& executor,
                              std::shared_ptr<asio::ssl::stream<asio::ip::tcp::socket>> stream)
        = 0;
    virtual void OnShutdown() = 0;

    void StartAccept(std::error_code& ec);

    log4cpp::Logger logger;
    std::shared_ptr<Executor> executor;

private:
    std::error_code ConfigureContext(const TLSConfig& config, std::error_code& ec);
    std::error_code ConfigureListener(const std::string& adapter, std::error_code& ec);

    SSLContext ctx;
    asio::ip::tcp::endpoint endpoint;
    asio::ip::tcp::acceptor acceptor;

    uint64_t session_id;
};

} // namespace opendnp3

#endif
