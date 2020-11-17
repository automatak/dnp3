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
#include "LinkContext.h"

#include "link/ILinkTx.h"
#include "link/LinkFrame.h"
#include "link/PriLinkLayerStates.h"
#include "link/SecLinkLayerStates.h"

namespace opendnp3
{

LinkContext::LinkContext(const Logger& logger,
                         const std::shared_ptr<exe4cpp::IExecutor>& executor,
                         std::shared_ptr<IUpperLayer> upper,
                         std::shared_ptr<ILinkListener> listener,
                         ILinkSession& session,
                         const LinkLayerConfig& config)
    : logger(logger),
      config(config),
      pSegments(nullptr),
      txMode(LinkTransmitMode::Idle),
      executor(executor),
      nextReadFCB(false),
      isOnline(false),
      keepAliveTimeout(false),
      lastMessageTimestamp(executor->get_time()),
      pPriState(&PLLS_Idle::Instance()),
      pSecState(&SLLS_NotReset::Instance()),
      listener(std::move(listener)),
      upper(std::move(upper)),
      pSession(&session)
{
}

bool LinkContext::OnLowerLayerUp()
{
    if (this->isOnline)
    {
        SIMPLE_LOG_BLOCK(logger, flags::ERR, "Layer already online");
        return false;
    }

    const auto now = Timestamp(this->executor->get_time());

    this->isOnline = true;

    this->lastMessageTimestamp = now; // no reason to trigger a keep-alive until we've actually expired

    this->StartKeepAliveTimer(now + config.KeepAliveTimeout);

    listener->OnStateChange(LinkStatus::UNRESET);
    upper->OnLowerLayerUp();

    return true;
}

bool LinkContext::OnLowerLayerDown()
{
    if (!isOnline)
    {
        SIMPLE_LOG_BLOCK(logger, flags::ERR, "Layer is not online");
        return false;
    }

    isOnline = false;
    keepAliveTimeout = false;
    pSegments = nullptr;
    txMode = LinkTransmitMode::Idle;
    pendingPriTx.clear();
    pendingSecTx.clear();

    rspTimeoutTimer.cancel();
    keepAliveTimer.cancel();

    pPriState = &PLLS_Idle::Instance();
    pSecState = &SLLS_NotReset::Instance();

    listener->OnStateChange(LinkStatus::UNRESET);
    upper->OnLowerLayerDown();

    return true;
}

bool LinkContext::SetTxSegment(ITransportSegment& segments)
{
    if (!this->isOnline)
    {
        SIMPLE_LOG_BLOCK(this->logger, flags::ERR, "Layer is not online");
        return false;
    }

    if (this->pSegments)
    {
        SIMPLE_LOG_BLOCK(this->logger, flags::ERR, "Already transmitting a segment");
        return false;
    }

    this->pSegments = &segments;
    return true;
}

bool LinkContext::OnTxReady()
{
    if (this->txMode == LinkTransmitMode::Idle)
    {
        SIMPLE_LOG_BLOCK(this->logger, flags::ERR, "Unknown transmission callback");
        return false;
    }

    auto isPrimary = (this->txMode == LinkTransmitMode::Primary);
    this->txMode = LinkTransmitMode::Idle;

    // before we dispatch the transmit result, give any pending transmissions access first
    this->TryPendingTx(this->pendingSecTx, false);
    this->TryPendingTx(this->pendingPriTx, true);

    // now dispatch the completion event to the correct state handler
    if (isPrimary)
    {
        this->pPriState = &this->pPriState->OnTxReady(*this);
    }
    else
    {
        this->pSecState = &this->pSecState->OnTxReady(*this);
    }

    return true;
}

ser4cpp::rseq_t LinkContext::FormatPrimaryBufferWithUnconfirmed(const Addresses& addr, const ser4cpp::rseq_t& tpdu)
{
    auto buffer = this->priTxBuffer.as_wseq();
    auto output
        = LinkFrame::FormatUnconfirmedUserData(buffer, config.IsMaster, addr.destination, addr.source, tpdu, &logger);
    FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, output, 10, 18);
    return output;
}

void LinkContext::QueueTransmit(const ser4cpp::rseq_t& buffer, bool primary)
{
    if (txMode == LinkTransmitMode::Idle)
    {
        txMode = primary ? LinkTransmitMode::Primary : LinkTransmitMode::Secondary;
        linktx->BeginTransmit(buffer, *pSession);
    }
    else
    {
        if (primary)
        {
            pendingPriTx.set(buffer);
        }
        else
        {
            pendingSecTx.set(buffer);
        }
    }
}

void LinkContext::QueueAck(uint16_t destination)
{
    auto dest = secTxBuffer.as_wseq();
    auto buffer = LinkFrame::FormatAck(dest, config.IsMaster, false, destination, this->config.LocalAddr, &logger);
    FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, buffer, 10, 18);
    this->QueueTransmit(buffer, false);
}

void LinkContext::QueueLinkStatus(uint16_t destination)
{
    auto dest = secTxBuffer.as_wseq();
    auto buffer
        = LinkFrame::FormatLinkStatus(dest, config.IsMaster, false, destination, this->config.LocalAddr, &logger);
    FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, buffer, 10, 18);
    this->QueueTransmit(buffer, false);
}

void LinkContext::QueueRequestLinkStatus(uint16_t destination)
{
    auto dest = priTxBuffer.as_wseq();
    auto buffer
        = LinkFrame::FormatRequestLinkStatus(dest, config.IsMaster, destination, this->config.LocalAddr, &logger);
    FORMAT_HEX_BLOCK(logger, flags::LINK_TX_HEX, buffer, 10, 18);
    this->QueueTransmit(buffer, true);
}

void LinkContext::PushDataUp(const Message& message)
{
    upper->OnReceive(message);
}

void LinkContext::CompleteSendOperation()
{
    this->pSegments = nullptr;

    auto callback = [upper = upper]() { upper->OnTxReady(); };

    this->executor->post(callback);
}

void LinkContext::TryStartTransmission()
{
    if (this->keepAliveTimeout)
    {
        this->pPriState = &pPriState->TrySendRequestLinkStatus(*this);
    }

    if (this->pSegments)
    {
        this->pPriState = &pPriState->TrySendUnconfirmed(*this, *pSegments);
    }
}

void LinkContext::OnKeepAliveTimeout()
{
    const auto now = Timestamp(this->executor->get_time());

    auto elapsed = now - this->lastMessageTimestamp;

    if (elapsed >= this->config.KeepAliveTimeout)
    {
        this->lastMessageTimestamp = now;
        this->keepAliveTimeout = true;
    }

    this->StartKeepAliveTimer(this->lastMessageTimestamp + config.KeepAliveTimeout);

    this->TryStartTransmission();
}

void LinkContext::OnResponseTimeout()
{
    this->pPriState = &(this->pPriState->OnTimeout(*this));

    this->TryStartTransmission();
}

void LinkContext::StartResponseTimer()
{
    std::weak_ptr<void> guard = this->lifetimeGuard;
    rspTimeoutTimer = executor->start(config.Timeout.value, [this, guard]() {
        if (guard.lock())
            this->OnResponseTimeout();
    });
}

void LinkContext::StartKeepAliveTimer(const Timestamp& expiration)
{
    std::weak_ptr<void> guard = this->lifetimeGuard;
    this->keepAliveTimer = executor->start(expiration.value, [this, guard]() {
        if (guard.lock())
            this->OnKeepAliveTimeout();
    });
}

void LinkContext::CancelTimer()
{
    rspTimeoutTimer.cancel();
}

void LinkContext::FailKeepAlive(bool timeout)
{
    if (timeout)
    {
        this->listener->OnKeepAliveFailure();
    }
}

void LinkContext::CompleteKeepAlive()
{
    this->listener->OnKeepAliveSuccess();
}

bool LinkContext::OnFrame(const LinkHeaderFields& header, const ser4cpp::rseq_t& userdata)
{
    if (!isOnline)
    {
        SIMPLE_LOG_BLOCK(logger, flags::ERR, "Layer is not online");
        return false;
    }

    if (header.isFromMaster == config.IsMaster)
    {
        ++statistics.numBadMasterBit;
        SIMPLE_LOG_BLOCK(
            logger, flags::WARN,
            (header.isFromMaster ? "Master frame received for master" : "Outstation frame received for outstation"));
        return false;
    }

    if (header.addresses.destination != config.LocalAddr && !header.addresses.IsBroadcast())
    {
        ++statistics.numUnknownDestination;
        this->listener->OnUnknownDestinationAddress(header.addresses.destination);
        return false;
    }

    if (header.addresses.source != config.RemoteAddr && !config.respondToAnySource)
    {
        ++statistics.numUnknownSource;
        this->listener->OnUnknownSourceAddress(header.addresses.source);
        return false;
    }

    if (header.addresses.IsBroadcast())
    {
        // Broadcast addresses can only be used for sending data.
        // If confirmed data is used, no response is sent back.
        if (header.func == LinkFunction::PRI_UNCONFIRMED_USER_DATA)
        {
            this->PushDataUp(Message(header.addresses, userdata));
            return true;
        }
        else if (header.func == LinkFunction::PRI_CONFIRMED_USER_DATA)
        {
            pSecState = &pSecState->OnConfirmedUserData(*this, header.addresses.source, header.fcb, true,
                                                        Message(header.addresses, userdata));
        }
        else
        {
            FORMAT_LOG_BLOCK(logger, flags::WARN, "Received invalid function (%s) with broadcast destination address",
                             LinkFunctionSpec::to_string(header.func));
            ++statistics.numUnexpectedFrame;
            return false;
        }
    }

    // reset the keep-alive timestamp
    this->lastMessageTimestamp = Timestamp(this->executor->get_time());

    switch (header.func)
    {
    case (LinkFunction::SEC_ACK):
        pPriState = &pPriState->OnAck(*this, header.fcvdfc);
        break;
    case (LinkFunction::SEC_NACK):
        pPriState = &pPriState->OnNack(*this, header.fcvdfc);
        break;
    case (LinkFunction::SEC_LINK_STATUS):
        pPriState = &pPriState->OnLinkStatus(*this, header.fcvdfc);
        break;
    case (LinkFunction::SEC_NOT_SUPPORTED):
        pPriState = &pPriState->OnNotSupported(*this, header.fcvdfc);
        break;
    case (LinkFunction::PRI_TEST_LINK_STATES):
        pSecState = &pSecState->OnTestLinkStatus(*this, header.addresses.source, header.fcb);
        break;
    case (LinkFunction::PRI_RESET_LINK_STATES):
        pSecState = &pSecState->OnResetLinkStates(*this, header.addresses.source);
        break;
    case (LinkFunction::PRI_REQUEST_LINK_STATUS):
        pSecState = &pSecState->OnRequestLinkStatus(*this, header.addresses.source);
        break;
    case (LinkFunction::PRI_CONFIRMED_USER_DATA):
        pSecState = &pSecState->OnConfirmedUserData(*this, header.addresses.source, header.fcb, false,
                                                    Message(header.addresses, userdata));
        break;
    case (LinkFunction::PRI_UNCONFIRMED_USER_DATA):
        this->PushDataUp(Message(header.addresses, userdata));
        break;
    default:
        break;
    }

    return true;
}

bool LinkContext::TryPendingTx(ser4cpp::Settable<ser4cpp::rseq_t>& pending, bool primary)
{
    if (this->txMode == LinkTransmitMode::Idle && pending.is_set())
    {
        this->linktx->BeginTransmit(pending.get(), *pSession);
        pending.clear();
        this->txMode = primary ? LinkTransmitMode::Primary : LinkTransmitMode::Secondary;
        return true;
    }

    return false;
}

} // namespace opendnp3
