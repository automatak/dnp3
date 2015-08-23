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
#ifndef OPENDNP3_PRILINKLAYERSTATES_H
#define OPENDNP3_PRILINKLAYERSTATES_H

#include "opendnp3/link/Singleton.h"
#include "opendnp3/link/LinkLayer.h"
#include "opendnp3/LogLevels.h"

namespace opendnp3
{

class LinkLayer;



class PriStateBase
{
public:

	// Incoming messages for primary station
	virtual void Ack(LinkLayer*, bool receiveBuffFull);
	virtual void Nack(LinkLayer*, bool receiveBuffFull);
	virtual void LinkStatus(LinkLayer*, bool receiveBuffFull);
	virtual void NotSupported(LinkLayer*, bool receiveBuffFull);
	virtual void OtherFunction(LinkLayer*);

	virtual void OnTransmitResult(LinkLayer*, bool success);

	virtual void OnTimeout(LinkLayer*);

	/*Upper layer events to handle */
	virtual void SendConfirmed(LinkLayer*, ITransportSegment& segments);
	virtual void SendUnconfirmed(LinkLayer*, ITransportSegment& segments);

	//every concrete state implements this for logging purposes
	virtual char const* Name() const = 0;
};

//	@section desc Entry state for primary station
class PLLS_SecNotReset : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_SecNotReset);
	virtual void SendUnconfirmed(LinkLayer*, ITransportSegment& segments) override final;
	virtual void SendConfirmed(LinkLayer*, ITransportSegment& segments) override final;
};


/////////////////////////////////////////////////////////////////////////////
//  template wait state for send unconfirmed data
/////////////////////////////////////////////////////////////////////////////

template <class ReturnToState>
class PLLS_SendUnconfirmedTransmitWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_SendUnconfirmedTransmitWait<ReturnToState>);

	virtual void OnTransmitResult(LinkLayer* apLL, bool success);
};

template <class ReturnToState>
PLLS_SendUnconfirmedTransmitWait<ReturnToState> PLLS_SendUnconfirmedTransmitWait<ReturnToState>::instance;

template <class ReturnToState>
void PLLS_SendUnconfirmedTransmitWait<ReturnToState>::OnTransmitResult(LinkLayer* apLL, bool success)
{
	if (apLL->pSegments->Advance())
	{
		auto output = apLL->FormatPrimaryBufferWithUnconfirmed(apLL->pSegments->GetSegment());
		apLL->QueueTransmit(output, true);		
	}
	else // we're done
	{
		apLL->ChangeState(ReturnToState::Inst());
		apLL->DoSendResult(success);
	}	
}

/////////////////////////////////////////////////////////////////////////////
//  Wait for the link layer to transmit the reset links
/////////////////////////////////////////////////////////////////////////////

class PLLS_LinkResetTransmitWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_LinkResetTransmitWait);

	virtual void OnTransmitResult(LinkLayer* apLL, bool success);
};



/////////////////////////////////////////////////////////////////////////////
//  Wait for the link layer to transmit the request link status, and wait for response
/////////////////////////////////////////////////////////////////////////////

template <class ReturnToState>
class PLLS_RequestLinkStatusTransmitWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_RequestLinkStatusTransmitWait<ReturnToState>);

	virtual void OnTransmitResult(LinkLayer* apLL, bool success);
};

//	@section desc As soon as we get a link status, return to base state
template <class ReturnToState>
class PLLS_LinkStatusWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_LinkStatusWait<ReturnToState>);

	void Ack(LinkLayer* apLL, bool)
	{
		Failure(apLL);
	}
	void Nack(LinkLayer*  apLL, bool)
	{
		Failure(apLL);
	}
	void LinkStatus(LinkLayer* apLL, bool);
	void NotSupported (LinkLayer*  apLL, bool)
	{
		Failure(apLL);
	}
	void OtherFunction (LinkLayer*  apLL, bool)
	{
		Failure(apLL);
	}

	void OnTimeout(LinkLayer*);

private:
	void Failure(LinkLayer*);
};

template <class ReturnToState>
PLLS_RequestLinkStatusTransmitWait<ReturnToState> PLLS_RequestLinkStatusTransmitWait<ReturnToState>::instance;

template <class ReturnToState>
PLLS_LinkStatusWait<ReturnToState> PLLS_LinkStatusWait<ReturnToState>::instance;

template <class ReturnToState>
void PLLS_RequestLinkStatusTransmitWait<ReturnToState>::OnTransmitResult(LinkLayer* pLinkLayer, bool success)
{
	if (success)
	{
		// now we're waiting for a link status response
		pLinkLayer->StartTimer();
		pLinkLayer->ChangeState(PLLS_LinkStatusWait<ReturnToState>::Inst());
	}
	else
	{
		pLinkLayer->ChangeState(ReturnToState::Inst());
		pLinkLayer->DoSendResult(success);
	}
}

template <class ReturnToState>
void PLLS_LinkStatusWait<ReturnToState>::LinkStatus(LinkLayer* pLinkLayer, bool receiveBuffFull)
{
	pLinkLayer->CancelTimer();
	pLinkLayer->ChangeState(ReturnToState::Inst());
	pLinkLayer->DoSendResult(true);
}

template <class ReturnToState>
void PLLS_LinkStatusWait<ReturnToState>::Failure(LinkLayer* pLinkLayer)
{
	pLinkLayer->CancelTimer();
	pLinkLayer->ChangeState(ReturnToState::Inst());
	pLinkLayer->DoSendResult(false);
}

template <class ReturnToState>
void PLLS_LinkStatusWait<ReturnToState>::OnTimeout(LinkLayer* pLinkLayer)
{
	if (pLinkLayer->Retry())
	{
		FORMAT_LOG_BLOCK(pLinkLayer->GetLogger(), flags::WARN, "%s: Request link status timeout, retrying %i remaining", Name(), pLinkLayer->RetryRemaining());
		pLinkLayer->QueueRequestLinkStatus();
		pLinkLayer->ChangeState(PLLS_RequestLinkStatusTransmitWait<ReturnToState>::Inst());
	}
	else
	{
		pLinkLayer->CallStatusCallback(opendnp3::LinkStatus::TIMEOUT);
		FORMAT_LOG_BLOCK(pLinkLayer->GetLogger(), flags::WARN, "%s: Request link status final timeout, no retries remain", Name());
		pLinkLayer->ChangeState(PLLS_SecNotReset::Inst());
		pLinkLayer->DoSendResult(false);
	}
}

/////////////////////////////////////////////////////////////////////////////
//  Wait for the link layer to transmit confirmed user data
/////////////////////////////////////////////////////////////////////////////

class PLLS_ConfUserDataTransmitWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_ConfUserDataTransmitWait);

	virtual void OnTransmitResult(LinkLayer* apLL, bool success);
};

/////////////////////////////////////////////////////////////////////////////
//  idle state when the secondary is reset
/////////////////////////////////////////////////////////////////////////////

//	@section desc for reset state
class PLLS_SecReset : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_SecReset);

	virtual void SendUnconfirmed(LinkLayer*, ITransportSegment& segments) override final;
	virtual void SendConfirmed(LinkLayer*, ITransportSegment& segments) override final;
};

//	@section desc As soon as we get an ACK, send the delayed pri frame
class PLLS_ResetLinkWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_ResetLinkWait);

	void Ack(LinkLayer*, bool aIsRcvBuffFull);
	void Nack(LinkLayer*  apLL, bool)
	{
		Failure(apLL);
	}
	void LinkStatus(LinkLayer* apLL, bool)
	{
		Failure(apLL);
	}
	void NotSupported (LinkLayer*  apLL, bool)
	{
		Failure(apLL);
	}
	void OtherFunction (LinkLayer*  apLL, bool)
	{
		Failure(apLL);
	}

	void OnTimeout(LinkLayer*);

private:
	void Failure(LinkLayer*);
};

//	@section desc As soon as we get an ACK, send the delayed pri frame
class PLLS_ConfDataWait : public PriStateBase
{
	MACRO_STATE_SINGLETON_INSTANCE(PLLS_ConfDataWait);

	void Ack(LinkLayer*, bool aIsRcvBuffFull);
	void Nack(LinkLayer* apLL, bool);
	void LinkStatus(LinkLayer* apLL, bool)
	{
		Failure(apLL);
	}
	void NotSupported (LinkLayer* apLL, bool)
	{
		Failure(apLL);
	}
	void OtherFunction(LinkLayer* pLinkLayer)
	{
		Failure(pLinkLayer);
	}
	void OnTimeout(LinkLayer*);

private:
	void Failure(LinkLayer*);
};

} //end namepsace

#endif

