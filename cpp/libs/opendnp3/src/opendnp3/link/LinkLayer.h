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
#ifndef OPENDNP3_LINKLAYER_H
#define OPENDNP3_LINKLAYER_H

#include <openpal/executor/IExecutor.h>
#include <openpal/logging/LogRoot.h>
#include <openpal/container/Settable.h>
#include <opendnp3/gen/LinkStatus.h>
#include <opendnp3/link/ILinkStatusListener.h>

#include "opendnp3/link/ILinkLayer.h"
#include "opendnp3/link/ILinkSession.h"
#include "opendnp3/link/LinkLayerConstants.h"
#include "opendnp3/link/LinkConfig.h"

namespace opendnp3
{

class ILinkRouter;
class PriStateBase;
class SecStateBase;

//	@section desc Implements the contextual state of DNP3 Data Link Layer
class LinkLayer : public ILinkLayer, public ILinkSession, public HasUpperLayer
{
	enum class TransmitMode : uint8_t
	{
		Idle,
		Primary,
		Secondary
	};	

public:

	LinkLayer(openpal::LogRoot&, openpal::IExecutor*, const LinkConfig&);

	void SetRouter(ILinkRouter&);

	// ILinkSession interface
	virtual void OnLowerLayerUp() override final;
	virtual void OnLowerLayerDown() override final;
	virtual void OnTransmitResult(bool success) override final;

	// IFrameSink interface
	virtual void Ack(bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc) override final;
	virtual void Nack(bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc) override final;
	virtual void LinkStatus(bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc) override final;
	virtual void NotSupported(bool aIsMaster, bool aIsRcvBuffFull, uint16_t aDest, uint16_t aSrc) override final;
	virtual void TestLinkStatus(bool aIsMaster, bool aFcb, uint16_t aDest, uint16_t aSrc) override final;
	virtual void ResetLinkStates(bool aIsMaster, uint16_t aDest, uint16_t aSrc) override final;
	virtual void RequestLinkStatus(bool aIsMaster, uint16_t aDest, uint16_t aSrc) override final;
	virtual void ConfirmedUserData(bool aIsMaster, bool aFcb, uint16_t aDest, uint16_t aSrc, const openpal::ReadBufferView& arBuffer) override final;
	virtual void UnconfirmedUserData(bool aIsMaster, uint16_t aDest, uint16_t aSrc, const openpal::ReadBufferView& arBuffer) override final;
    
	// ------------- ILinkLayer --------------------
	virtual void Send(ITransportSegment& segments) override final;

	// Functions called by the primary and secondary station states
	void ChangeState(PriStateBase*);
	void ChangeState(SecStateBase*);
	void CallStatusCallback(opendnp3::LinkStatus status);

	// For registering a callback to listen to link status
	void SetLinkStatusListener(opendnp3::ILinkStatusListener* Listener);

	openpal::Logger& GetLogger()
	{
		return logger;
	}

	void DoDataUp(const openpal::ReadBufferView& arBuffer)
	{
		if (pUpperLayer)
		{
			pUpperLayer->OnReceive(arBuffer);
		}
	}

	void DoSendResult(bool isSuccess)
	{
		if (pUpperLayer)
		{
			pUpperLayer->OnSendResult(isSuccess);
		}
	}

	bool DoLowerSend()
	{
		if (pUpperLayer)
		{
			return pUpperLayer->OnLowerSend();
		}
		return true;
	}

	void PostSendResult(bool isSuccess);

	void ResetReadFCB()
	{
		nextReadFCB = true;
	}

	void ToggleReadFCB()
	{
		nextReadFCB = !nextReadFCB;
	}

	bool NextReadFCB()
	{
		return nextReadFCB;
	}

	void ResetWriteFCB()
	{
		nextWriteFCB = true;
	}

	void ToggleWriteFCB()
	{
		nextWriteFCB = !nextWriteFCB;
	}

	bool NextWriteFCB() const
	{
		return nextWriteFCB;
	}

	// Helpers for sending frames
	void QueueAck();
	void QueueRequestLinkStatus();
	void QueueLinkStatus();
	void QueueResetLinks();

	void StartTimer();
	void CancelTimer();

	openpal::Logger logger;
	const LinkConfig config;

	//Retry Count
	void ResetRetry();
	bool Retry();

	uint32_t RetryRemaining() const
	{
		return numRetryRemaining;
	}
    
	void ResetKeepAlive();
	void CancelKeepAlive();
    
	void QueueTransmit(const openpal::ReadBufferView& buffer, bool primary);

	// buffers used for primary and secondary requests	
	uint8_t priTxBuffer[LPDU_MAX_FRAME_SIZE];
	uint8_t secTxBuffer[LPDU_HEADER_SIZE];
	

	openpal::ReadBufferView FormatPrimaryBufferWithUnconfirmed(const openpal::ReadBufferView& tpdu);

	openpal::ReadBufferView FormatPrimaryBufferWithConfirmed(const openpal::ReadBufferView& tpdu, bool FCB);

	ITransportSegment* pSegments;

private:

	TransmitMode txMode;
	openpal::Settable<openpal::ReadBufferView> pendingPriTx;
	openpal::Settable<openpal::ReadBufferView> pendingSecTx;

	void CheckPendingTx(openpal::Settable<openpal::ReadBufferView>& pending, bool primary);

	uint32_t numRetryRemaining;

	openpal::IExecutor* pExecutor;
	openpal::ITimer* pTimer;
    openpal::ITimer* pKeepAliveTimer;

	// callback from the active timer
	void OnTimeout();

	bool nextReadFCB;
	bool nextWriteFCB;
	bool isOnline;
	bool TimedOut;

	bool Validate(bool isMaster, uint16_t src, uint16_t dest);	

	ILinkRouter* pRouter;
	PriStateBase* pPriState;
	SecStateBase* pSecState;

	/// callback for listening to the status of the link
	ILinkStatusListener* pStatusCallback;
};

}

#endif

