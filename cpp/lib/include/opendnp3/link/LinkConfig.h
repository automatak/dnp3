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
#ifndef OPENDNP3_LINKCONFIG_H
#define OPENDNP3_LINKCONFIG_H

#include "opendnp3/util/TimeDuration.h"
#include "opendnp3/link/Addresses.h"

namespace opendnp3
{

/**
    Configuration for the dnp3 link layer
*/
struct LinkConfig
{
    LinkConfig() = delete;

    LinkConfig(bool isMaster,
               bool useConfirms,
               uint32_t numRetry,
               uint16_t localAddr,
               uint16_t remoteAddr,
               TimeDuration timeout,
               TimeDuration keepAliveTimeout)
        :

          IsMaster(isMaster),
          UseConfirms(useConfirms),
          NumRetry(numRetry),
          LocalAddr(localAddr),
          RemoteAddr(remoteAddr),
          Timeout(timeout),
          KeepAliveTimeout(keepAliveTimeout)
    {
    }

    LinkConfig(bool isMaster, bool useConfirms)
        :

          IsMaster(isMaster),
          UseConfirms(useConfirms),
          NumRetry(0),
          LocalAddr(isMaster ? 1 : 1024),
          RemoteAddr(isMaster ? 1024 : 1),
          Timeout(TimeDuration::Seconds(1)),
          KeepAliveTimeout(TimeDuration::Minutes(1))
    {
    }

    inline Addresses GetAddresses() const
    {
        return Addresses(this->LocalAddr, this->RemoteAddr);
    }

    /// The master/outstation bit set on all messages
    bool IsMaster;

    /// If true, the link layer will send data requesting confirmation
    bool UseConfirms;

    /// The number of retry attempts the link will attempt after the initial try
    uint32_t NumRetry;

    /// dnp3 address of the local device
    uint16_t LocalAddr;

    /// dnp3 address of the remote device
    uint16_t RemoteAddr;

    /// the response timeout in milliseconds for confirmed requests
    TimeDuration Timeout;

    /// the interval for keep-alive messages (link status requests)
    /// if set to TimeDuration::Max(), the keep-alive is disabled
    TimeDuration KeepAliveTimeout;
};

} // namespace opendnp3

#endif
