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
#ifndef OPENDNP3_OUTSTATIONCONTEXT_H
#define OPENDNP3_OUTSTATIONCONTEXT_H

#include "opendnp3/LayerInterfaces.h"

#include "opendnp3/gen/SecurityStatIndex.h"

#include "opendnp3/outstation/OutstationConfig.h"
#include "opendnp3/outstation/RequestHistory.h"
#include "opendnp3/outstation/DeferredRequest.h"
#include "opendnp3/outstation/OutstationChannelStates.h"
#include "opendnp3/outstation/ControlState.h"
#include "opendnp3/outstation/OutstationSeqNum.h"
#include "opendnp3/outstation/Database.h"
#include "opendnp3/outstation/EventBuffer.h"
#include "opendnp3/outstation/ResponseContext.h"
#include "opendnp3/outstation/ICommandHandler.h"
#include "opendnp3/outstation/IOutstationApplication.h"

#include <openpal/executor/TimerRef.h>
#include <openpal/logging/LogRoot.h>
#include <openpal/container/Pair.h>

namespace opendnp3
{

///
/// Represent all of the mutable state in an outstation
///
class OContext : public IUpperLayer
{

public:

	OContext(	const OutstationConfig& config,
	            const DatabaseSizes& dbSizes,
	            openpal::Logger logger,
	            openpal::IExecutor& executor,
	            ILowerLayer& lower,
	            ICommandHandler& commandHandler,
	            IOutstationApplication& application);

public:

	/// ----- Implement IUpperLayer ------

	virtual bool OnLowerLayerUp() override;

	virtual bool OnLowerLayerDown() override;

	virtual bool OnSendResult(bool isSuccess) override final;

	virtual bool OnReceive(const openpal::RSlice& fragment) override final;

	/// ---- Helper functions that operate on the current solicited state, and may return a new solicited state ----

	OutstationSolicitedStateBase* ContinueMultiFragResponse(const AppSeqNum& seq);

	OutstationSolicitedStateBase* RespondToNonReadRequest(const APDUHeader& header, const openpal::RSlice& objects);

	OutstationSolicitedStateBase* RespondToReadRequest(const APDUHeader& header, const openpal::RSlice& objects);

	OutstationSolicitedStateBase* ProcessNewRequest(const APDUHeader& header, const openpal::RSlice& objects);

	OutstationSolicitedStateBase* OnReceiveSolRequest(const APDUHeader& header, const openpal::RSlice& objects);

	/// ----- method overridable for implementing SA or other extensions ----

	virtual void ReceiveParsedHeader(const openpal::RSlice& apdu, const APDUHeader& header, const openpal::RSlice& objects);

	virtual void CheckForTaskStart();

	virtual void Increment(SecurityStatIndex index) {}

	/// ---- External helpers ----

	void SetRestartIIN();

	IUpdateHandler& GetUpdateHanlder();

	DatabaseConfigView GetConfigView();

	/// ---- Processing functions --------



	void ProcessAPDU(const openpal::RSlice& apdu, const APDUHeader& header, const openpal::RSlice& objects);

	void ProcessRequest(const APDUHeader& header, const openpal::RSlice& objects);

	void ProcessConfirm(const APDUHeader& header);

	/// ---- common helper methods ----

	void ParseHeader(const openpal::RSlice& apdu);

	void BeginResponseTx(const openpal::RSlice& response);

	void BeginUnsolTx(const openpal::RSlice& response);

	void BeginTx(const openpal::RSlice& response);

	void CheckForDeferredRequest();

	bool ProcessDeferredRequest(APDUHeader header, openpal::RSlice objects);

	bool StartSolicitedConfirmTimer();

	bool StartUnsolicitedConfirmTimer();

	void CheckForUnsolicited();

	bool CanTransmit() const;

	IINField GetResponseIIN();

	IINField GetDynamicIIN();

	/// --- methods for handling app-layer functions ---

	/// Handles non-read function codes that require a response. builds the response using the supplied writer.
	/// @return An IIN field indicating the validity of the request, and to be returned in the response.
	IINField HandleNonReadResponse(const APDUHeader& header, const openpal::RSlice& objects, HeaderWriter& writer);

	/// Handles read function codes. May trigger an unsolicited response
	/// @return an IIN field and a partial AppControlField (missing sequence info)
	openpal::Pair<IINField, AppControlField> HandleRead(const openpal::RSlice& objects, HeaderWriter& writer);

	/// Handles no-response function codes.
	void ProcessRequestNoAck(const APDUHeader& header, const openpal::RSlice& objects);

	// ------ Function Handlers ------

	IINField HandleWrite(const openpal::RSlice& objects);
	IINField HandleSelect(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleOperate(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleDirectOperate(const openpal::RSlice& objects, OperateType opType, HeaderWriter* pWriter);
	IINField HandleDelayMeasure(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleRestart(const openpal::RSlice& objects, bool isWarmRestart, HeaderWriter* pWriter);
	IINField HandleAssignClass(const openpal::RSlice& objects);
	IINField HandleDisableUnsolicited(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleEnableUnsolicited(const openpal::RSlice& objects, HeaderWriter& writer);
	IINField HandleCommandWithConstant(const openpal::RSlice& objects, HeaderWriter& writer, CommandStatus status);

	// ------ resources --------
	openpal::Logger logger;
	openpal::IExecutor* const pExecutor;
	ILowerLayer* const pLower;
	ICommandHandler* const pCommandHandler;
	IOutstationApplication* const pApplication;

	// ------ Database, event buffer, and response tracking
	EventBuffer eventBuffer;
	Database database;
	ResponseContext rspContext;

	// ------ Static configuration -------
	OutstationParams params;

	// ------ Shared dynamic state --------
	bool isOnline;
	bool isTransmitting;
	IINField staticIIN;
	openpal::TimerRef confirmTimer;
	RequestHistory history;
	DeferredRequest deferred;

	// ------ Dynamic state related to controls ------
	ControlState control;

	// ------ Dynamic state related to solicited and unsolicited modes ------
	OutstationSolState  sol;
	OutstationUnsolState unsol;
};


}

#endif
