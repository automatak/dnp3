/*
 * Copyright 2013-2020 Automatak, LLC
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
#ifndef OPENDNP3_LINK_CONTEXT_H
#define OPENDNP3_LINK_CONTEXT_H

#include "link/ILinkLayer.h"
#include "link/ILinkSession.h"
#include "link/ILinkTx.h"
#include "link/LinkLayerConfig.h"
#include "link/LinkLayerConstants.h"

#include "opendnp3/StackStatistics.h"
#include "opendnp3/gen/LinkStatus.h"
#include "opendnp3/link/ILinkListener.h"
#include "opendnp3/logging/Logger.h"
#include "opendnp3/util/Timestamp.h"

#include <ser4cpp/container/Settable.h>
#include <ser4cpp/container/StaticBuffer.h>

#include <exe4cpp/IExecutor.h>

namespace opendnp3
{

class PriStateBase;
class SecStateBase;

enum class LinkTransmitMode : uint8_t
{
    Idle,
    Primary,
    Secondary
};

//	@section desc Implements the contextual state of DNP3 Data Link Layer
class LinkContext
{

public:
    LinkContext(const Logger& logger,
                const std::shared_ptr<exe4cpp::IExecutor>&,
                std::shared_ptr<IUpperLayer>,
                std::shared_ptr<ILinkListener>,
                ILinkSession& session,
                const LinkLayerConfig&);

    // ---- helpers for dealing with the FCB bits ----

    void ResetReadFCB()
    {
        nextReadFCB = true;
    }
    void ToggleReadFCB()
    {
        nextReadFCB = !nextReadFCB;
    }

    // --- helpers for dealing with layer state transitations ---
    bool OnLowerLayerUp();
    bool OnLowerLayerDown();
    bool OnTxReady();
    bool SetTxSegment(ITransportSegment& segments);

    // --- helpers for formatting user data messages ---
    ser4cpp::rseq_t FormatPrimaryBufferWithUnconfirmed(const Addresses& addr, const ser4cpp::rseq_t& tpdu);

    // --- Helpers for queueing frames ---
    void QueueAck(uint16_t destination);
    void QueueLinkStatus(uint16_t destination);
    void QueueRequestLinkStatus(uint16_t destination);

    void QueueTransmit(const ser4cpp::rseq_t& buffer, bool primary);

    // --- public members ----

    void PushDataUp(const Message& message);
    void CompleteSendOperation();
    void TryStartTransmission();
    void OnKeepAliveTimeout();
    void OnResponseTimeout();
    void StartResponseTimer();
    void StartKeepAliveTimer(const Timestamp& expiration);
    void CancelTimer();
    void FailKeepAlive(bool timeout);
    void CompleteKeepAlive();
    bool OnFrame(const LinkHeaderFields& header, const ser4cpp::rseq_t& userdata);
    bool TryPendingTx(ser4cpp::Settable<ser4cpp::rseq_t>& pending, bool primary);

    // buffers used for primary and secondary requests
    ser4cpp::StaticBuffer<LPDU_MAX_FRAME_SIZE> priTxBuffer;
    ser4cpp::StaticBuffer<LPDU_HEADER_SIZE> secTxBuffer;

    ser4cpp::Settable<ser4cpp::rseq_t> pendingPriTx;
    ser4cpp::Settable<ser4cpp::rseq_t> pendingSecTx;

    Logger logger;
    const LinkLayerConfig config;
    ITransportSegment* pSegments;
    LinkTransmitMode txMode;

    const std::shared_ptr<exe4cpp::IExecutor> executor;

    exe4cpp::Timer rspTimeoutTimer;
    exe4cpp::Timer keepAliveTimer;
    bool nextReadFCB;
    bool isOnline;
    bool keepAliveTimeout;
    Timestamp lastMessageTimestamp;
    StackStatistics::Link statistics;

    ILinkTx* linktx = nullptr;

    PriStateBase* pPriState;
    SecStateBase* pSecState;

    const std::shared_ptr<ILinkListener> listener;
    const std::shared_ptr<IUpperLayer> upper;

    ILinkSession* pSession;

private:
    std::shared_ptr<void> lifetimeGuard = std::make_shared<char>();
};

} // namespace opendnp3

#endif
