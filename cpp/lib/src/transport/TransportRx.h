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
#ifndef OPENDNP3_TRANSPORTRX_H
#define OPENDNP3_TRANSPORTRX_H

#include "app/Message.h"
#include "transport/TransportConstants.h"
#include "transport/TransportSeqNum.h"

#include "opendnp3/StackStatistics.h"

#include <ser4cpp/container/Buffer.h>
#include <ser4cpp/container/SequenceTypes.h>

#include <log4cpp/Logger.h>

namespace opendnp3
{

/**
State/validation for the DNP3 transport layer's receive channel.
*/
class TransportRx
{

public:
    TransportRx(const log4cpp::Logger&, uint32_t maxRxFragSize);

    Message ProcessReceive(const Message& segment);

    void Reset();

    const StackStatistics::Transport::Rx& Statistics() const
    {
        return statistics;
    }

private:
    ser4cpp::wseq_t GetAvailable();

    void ClearRxBuffer();

    log4cpp::Logger logger;
    StackStatistics::Transport::Rx statistics;

    ser4cpp::Buffer rxBuffer;
    uint32_t numBytesRead;
    Addresses lastAddresses;

    TransportSeqNum expectedSeq;
};

} // namespace opendnp3

#endif
