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
#include "MasterContext.h"


#include "opendnp3/LogLevels.h"
#include "opendnp3/app/APDULogging.h"
#include "opendnp3/app/parsing/APDUHeaderParser.h"
#include "opendnp3/app/APDUBuilders.h"
#include "opendnp3/master/MeasurementHandler.h"
#include "opendnp3/master/EmptyResponseTask.h"
#include "opendnp3/master/RestartOperationTask.h"
#include "opendnp3/objects/Group12.h"
#include "opendnp3/objects/Group41.h"

#include <openpal/logging/LogMacros.h>


using namespace openpal;

namespace opendnp3
{
MContext::MContext(
    IExecutor& executor,
    openpal::Logger logger,
    ILowerLayer& lower,
    ISOEHandler& SOEHandler,
    opendnp3::IMasterApplication& application,
    const MasterParams& params_,
    ITaskLock& taskLock
) :
	logger(logger),
	pExecutor(&executor),
	pLower(&lower),
	params(params_),
	pSOEHandler(&SOEHandler),
	pTaskLock(&taskLock),
	pApplication(&application),
	isOnline(false),
	isSending(false),
	responseTimer(executor),
	scheduleTimer(executor),
	taskStartTimeoutTimer(executor),
	tasks(params, logger, application, SOEHandler, application),
	scheduler(*this),
	txBuffer(params.maxTxFragSize),
	tstate(TaskState::IDLE)
{}

bool MContext::OnLowerLayerUp()
{
	if (isOnline)
	{
		return false;
	}

	isOnline = true;
	tasks.Initialize(scheduler);
	this->PostCheckForTask();
	return true;
}

bool MContext::OnLowerLayerDown()
{
	if (!isOnline)
	{
		return false;
	}

	auto now = pExecutor->GetTime();
	scheduler.Shutdown(now);

	if (pActiveTask.IsDefined())
	{
		pActiveTask->OnLowerLayerClose(now);
		pActiveTask.Release();
	}

	tstate = TaskState::IDLE;

	pTaskLock->Release(*this);

	responseTimer.Cancel();
	taskStartTimeoutTimer.Cancel();
	scheduleTimer.Cancel();

	solSeq = unsolSeq = 0;
	isOnline = isSending = false;

	return true;
}

bool MContext::OnReceive(const openpal::RSlice& apdu)
{
	if (!this->isOnline)
	{
		SIMPLE_LOG_BLOCK(this->logger, flags::ERR, "Ignorning rx data while offline");
		return false;
	}

	APDUResponseHeader header;
	if (!APDUHeaderParser::ParseResponse(apdu, header, &this->logger))
	{
		return true;
	}


	FORMAT_LOG_BLOCK(this->logger, flags::APP_HEADER_RX,
	                 "FIR: %i FIN: %i CON: %i UNS: %i SEQ: %i FUNC: %s IIN: [0x%02x, 0x%02x]",
	                 header.control.FIR,
	                 header.control.FIN,
	                 header.control.CON,
	                 header.control.UNS,
	                 header.control.SEQ,
	                 FunctionCodeToString(header.function),
	                 header.IIN.LSB,
	                 header.IIN.MSB);

	this->OnParsedHeader(apdu, header, apdu.Skip(APDU_RESPONSE_HEADER_SIZE));
	return true;
}

bool MContext::OnSendResult(bool isSucccess)
{
	if (!this->isOnline || !this->isSending)
	{
		return false;
	}

	this->isSending = false;
	this->CheckConfirmTransmit();
	this->CheckForTask();
	return true;
}

void MContext::CheckForTask()
{
	if (isOnline)
	{
		this->tstate = this->OnStartEvent();
	}
}

void MContext::OnResponseTimeout()
{
	if (isOnline)
	{
		this->tstate = this->OnResponseTimeoutEvent();
	}
}

void MContext::CompleteActiveTask()
{
	if (this->pActiveTask.IsDefined())
	{
		if (this->pActiveTask->IsRecurring())
		{
			this->scheduler.Schedule(std::move(this->pActiveTask));
		}
		else
		{
			this->pActiveTask.Release();
		}

		pTaskLock->Release(*this);
		this->PostCheckForTask();
	}
}

void MContext::OnParsedHeader(const RSlice& apdu, const APDUResponseHeader& header, const RSlice& objects)
{
	// Note: this looks silly, but OnParsedHeader() is virtual and can be overriden to do SA
	this->ProcessAPDU(header, objects);
}

/// --- command handlers ----

void MContext::DirectOperate(CommandSet&& commands, const CommandCallbackT& callback, const TaskConfig& config)
{
	this->ScheduleAdhocTask(CommandTask::FDirectOperate(std::move(commands), *pApplication, callback, config, logger));
}

void MContext::SelectAndOperate(CommandSet&& commands, const CommandCallbackT& callback, const TaskConfig& config)
{
	this->ScheduleAdhocTask(CommandTask::FSelectAndOperate(std::move(commands), *pApplication, callback, config, logger));
}

void MContext::ProcessAPDU(const APDUResponseHeader& header, const RSlice& objects)
{
	switch (header.function)
	{
	case(FunctionCode::UNSOLICITED_RESPONSE) :
		this->ProcessUnsolicitedResponse(header, objects);
		break;
	case(FunctionCode::RESPONSE) :
		this->ProcessResponse(header, objects);
		break;
	default:
		FORMAT_LOG_BLOCK(this->logger, flags::WARN, "Ignoring unsupported function code: %s", FunctionCodeToString(header.function));
		break;
	}
}

void MContext::ProcessIIN(const IINField& iin)
{
	if (iin.IsSet(IINBit::DEVICE_RESTART) && !this->params.ignoreRestartIIN)
	{
		this->tasks.clearRestart.Demand();
		this->tasks.assignClass.Demand();
		this->tasks.startupIntegrity.Demand();
		this->tasks.enableUnsol.Demand();
	}

	if (iin.IsSet(IINBit::EVENT_BUFFER_OVERFLOW) && this->params.integrityOnEventOverflowIIN)
	{
		this->tasks.startupIntegrity.Demand();
	}

	if (iin.IsSet(IINBit::NEED_TIME))
	{
		switch (this->params.timeSyncMode)
		{
		case(TimeSyncMode::SerialTimeSync):
			this->tasks.timeSync.Demand();
			break;
		default:
			break;
		}
	}

	if ((iin.IsSet(IINBit::CLASS1_EVENTS) && this->params.eventScanOnEventsAvailableClassMask.HasClass1()) ||
	        (iin.IsSet(IINBit::CLASS2_EVENTS) && this->params.eventScanOnEventsAvailableClassMask.HasClass2()) ||
	        (iin.IsSet(IINBit::CLASS3_EVENTS) && this->params.eventScanOnEventsAvailableClassMask.HasClass3()))
	{
		this->tasks.eventScan.Demand();
	}

	this->pApplication->OnReceiveIIN(iin);
}

void MContext::ProcessUnsolicitedResponse(const APDUResponseHeader& header, const RSlice& objects)
{
	if (!header.control.UNS)
	{
		SIMPLE_LOG_BLOCK(logger, flags::WARN, "Ignoring unsolicited response without UNS bit set");
		return;
	}

	auto result = MeasurementHandler::ProcessMeasurements(objects, logger, pSOEHandler);

	if ((result == ParseResult::OK) && header.control.CON)
	{
		this->QueueConfirm(APDUHeader::UnsolicitedConfirm(header.control.SEQ));
	}

	this->ProcessIIN(header.IIN);
}

void MContext::ProcessResponse(const APDUResponseHeader& header, const RSlice& objects)
{
	this->tstate = this->OnResponseEvent(header, objects);
	this->ProcessIIN(header.IIN); // TODO - should we process IIN bits for unexpected responses?
}

void MContext::QueueConfirm(const APDUHeader& header)
{
	this->confirmQueue.push_back(header);
	this->CheckConfirmTransmit();
}

bool MContext::CheckConfirmTransmit()
{
	if (this->isSending || this->confirmQueue.empty())
	{
		return false;
	}

	auto confirm = this->confirmQueue.front();
	APDUWrapper wrapper(this->txBuffer.GetWSlice());
	wrapper.SetFunction(confirm.function);
	wrapper.SetControl(confirm.control);
	this->Transmit(wrapper.ToRSlice());
	this->confirmQueue.pop_front();
	return true;
}

void MContext::Transmit(const RSlice& data)
{
	logging::ParseAndLogRequestTx(this->logger, data);
	assert(!this->isSending);
	this->isSending = true;
	this->pLower->BeginTransmit(data);
}

void MContext::StartResponseTimer()
{
	auto timeout = [this]()
	{
		this->OnResponseTimeout();
	};
	this->responseTimer.Start(this->params.responseTimeout, timeout);
}

void MContext::PostCheckForTask()
{
	auto callback = [this]()
	{
		this->CheckForTask();
	};
	this->pExecutor->Post(callback);
}

MasterScan MContext::AddScan(openpal::TimeDuration period, const HeaderBuilderT& builder, TaskConfig config)
{
	auto pTask = new UserPollTask(builder, true, period, params.taskRetryPeriod, *pApplication, *pSOEHandler, logger, config);
	this->ScheduleRecurringPollTask(pTask);
	auto callback = [this]()
	{
		this->PostCheckForTask();
	};
	return MasterScan(*pExecutor, pTask, callback);
}

MasterScan MContext::AddClassScan(const ClassField& field, openpal::TimeDuration period, TaskConfig config)
{
	auto build = [field](HeaderWriter & writer) -> bool
	{
		return build::WriteClassHeaders(writer, field);
	};
	return this->AddScan(period, build, config);
}

MasterScan MContext::AddAllObjectsScan(GroupVariationID gvId, openpal::TimeDuration period, TaskConfig config)
{
	auto build = [gvId](HeaderWriter & writer) -> bool
	{
		return writer.WriteHeader(gvId, QualifierCode::ALL_OBJECTS);
	};
	return this->AddScan(period, build, config);
}

MasterScan MContext::AddRangeScan(GroupVariationID gvId, uint16_t start, uint16_t stop, openpal::TimeDuration period, TaskConfig config)
{
	auto build = [gvId, start, stop](HeaderWriter & writer) -> bool
	{
		return writer.WriteRangeHeader<openpal::UInt16>(QualifierCode::UINT16_START_STOP, gvId, start, stop);
	};
	return this->AddScan(period, build, config);
}

void MContext::Scan(const HeaderBuilderT& builder, TaskConfig config)
{
	auto pTask = new UserPollTask(builder, false, TimeDuration::Max(), params.taskRetryPeriod, *pApplication, *pSOEHandler, logger, config);
	this->ScheduleAdhocTask(pTask);
}

void MContext::ScanClasses(const ClassField& field, TaskConfig config)
{
	auto configure = [field](HeaderWriter & writer) -> bool
	{
		return build::WriteClassHeaders(writer, field);
	};
	this->Scan(configure, config);
}

void MContext::ScanAllObjects(GroupVariationID gvId, TaskConfig config)
{
	auto configure = [gvId](HeaderWriter & writer) -> bool
	{
		return writer.WriteHeader(gvId, QualifierCode::ALL_OBJECTS);
	};
	this->Scan(configure, config);
}

void MContext::ScanRange(GroupVariationID gvId, uint16_t start, uint16_t stop, TaskConfig config)
{
	auto configure = [gvId, start, stop](HeaderWriter & writer) -> bool
	{
		return writer.WriteRangeHeader<openpal::UInt16>(QualifierCode::UINT16_START_STOP, gvId, start, stop);
	};
	this->Scan(configure, config);
}

void MContext::Write(const TimeAndInterval& value, uint16_t index, TaskConfig config)
{
	auto builder = [value, index](HeaderWriter & writer) -> bool
	{
		return writer.WriteSingleIndexedValue<UInt16, TimeAndInterval>(QualifierCode::UINT16_CNT_UINT16_INDEX, Group50Var4::Inst(), value, index);
	};

	auto pTask = new EmptyResponseTask(*this->pApplication, "WRITE TimeAndInterval", FunctionCode::WRITE, builder, this->logger, config);
	this->ScheduleAdhocTask(pTask);
}

void MContext::Restart(RestartType op, const RestartOperationCallbackT& callback, TaskConfig config)
{
	auto pTask = new RestartOperationTask(*this->pApplication, op, callback, this->logger, config);
	this->ScheduleAdhocTask(pTask);
}

void MContext::PerformFunction(const std::string& name, opendnp3::FunctionCode func, const HeaderBuilderT& builder, TaskConfig config)
{
	auto pTask = new EmptyResponseTask(*this->pApplication, name, func, builder, this->logger, config);
	this->ScheduleAdhocTask(pTask);
}

void MContext::SetTaskStartTimeout(const openpal::MonotonicTimestamp& time)
{
	auto action = [this]()
	{
		this->scheduler.CheckTaskStartTimeout(pExecutor->GetTime());
	};

	this->taskStartTimeoutTimer.Restart(time, action);
}

/// ------ private helpers ----------

void MContext::ScheduleRecurringPollTask(IMasterTask* pTask)
{
	this->tasks.BindTask(pTask);

	if (this->isOnline)
	{
		this->scheduler.Schedule(ManagedPtr<IMasterTask>::WrapperOnly(pTask));
		this->PostCheckForTask();
	}
}

void MContext::ScheduleAdhocTask(IMasterTask* pTask)
{
	const auto NOW = this->pExecutor->GetTime();

	pTask->ConfigureStartExpiration(NOW.Add(params.taskStartTimeout));

	auto task = ManagedPtr<IMasterTask>::Deleted(pTask);
	if (this->isOnline)
	{
		if (this->MeetsUserRequirements(*pTask))
		{
			this->scheduler.Schedule(std::move(task));
			this->CheckForTask();
		}
		else
		{
			// task is failed because an SA user doesn't exist
			pTask->OnNoUser(NOW);
		}
	}
	else
	{
		// can't run this task since we're offline so fail it immediately
		pTask->OnLowerLayerClose(NOW);
	}
}

MContext::TaskState MContext::BeginNewTask(ManagedPtr<IMasterTask>& task)
{
	this->pActiveTask = std::move(task);
	this->pActiveTask->OnStart();
	FORMAT_LOG_BLOCK(logger, flags::INFO, "Begining task: %s", this->pActiveTask->Name());
	return this->ResumeActiveTask();
}

MContext::TaskState MContext::ResumeActiveTask()
{
	if (!this->pTaskLock->Acquire(*this))
	{
		return TaskState::TASK_READY;
	}

	APDURequest request(this->txBuffer.GetWSlice());

	/// try to build a requst for the task
	if (!this->pActiveTask->BuildRequest(request, this->solSeq))
	{
		pActiveTask->OnInternalError(pExecutor->GetTime());
		this->CompleteActiveTask();
		return TaskState::IDLE;
	}

	this->StartResponseTimer();
	auto apdu = request.ToRSlice();
	this->RecordLastRequest(apdu);
	this->Transmit(apdu);

	return TaskState::WAIT_FOR_RESPONSE;
}

//// --- State tables ---

MContext::TaskState MContext::OnResponseEvent(const APDUResponseHeader& header, const RSlice& objects)
{
	switch (tstate)
	{
	case(TaskState::WAIT_FOR_RESPONSE) :
		return OnResponse_WaitForResponse(header, objects);
	default:
		FORMAT_LOG_BLOCK(logger, flags::WARN, "Not expecting a response, sequence: %u", header.control.SEQ);
		return tstate;
	}
}

MContext::TaskState MContext::OnStartEvent()
{
	switch (tstate)
	{
	case(TaskState::IDLE) :
		return StartTask_Idle();
	case(TaskState::TASK_READY) :
		return StartTask_TaskReady();
	default:
		return tstate;
	}
}

MContext::TaskState MContext::OnResponseTimeoutEvent()
{
	switch (tstate)
	{
	case(TaskState::WAIT_FOR_RESPONSE) :
		return OnResponseTimeout_WaitForResponse();
	default:
		SIMPLE_LOG_BLOCK(logger, flags::ERR, "Unexpected response timeout");
		return tstate;
	}
}

//// --- State actions ----

MContext::TaskState MContext::StartTask_Idle()
{
	if (this->isSending)
	{
		return TaskState::IDLE;
	}

	MonotonicTimestamp next;
	auto task = this->scheduler.GetNext(pExecutor->GetTime(), next);

	if (task.IsDefined())
	{
		return this->BeginNewTask(task);
	}
	else
	{
		// restart the task timer
		if (!next.IsMax())
		{
			scheduleTimer.Restart(next, [this]()
			{
				this->CheckForTask();
			});
		}
		return TaskState::IDLE;
	}
}

MContext::TaskState MContext::StartTask_TaskReady()
{
	if (this->isSending)
	{
		return TaskState::TASK_READY;
	}

	return this->ResumeActiveTask();
}

MContext::TaskState MContext::OnResponse_WaitForResponse(const APDUResponseHeader& header, const RSlice& objects)
{
	if (header.control.SEQ != this->solSeq)
	{
		FORMAT_LOG_BLOCK(this->logger, flags::WARN, "Response with bad sequence: %u", header.control.SEQ);
		return TaskState::WAIT_FOR_RESPONSE;
	}

	this->responseTimer.Cancel();

	this->solSeq.Increment();

	auto now = this->pExecutor->GetTime();

	auto result = this->pActiveTask->OnResponse(header, objects, now);

	if (header.control.CON)
	{
		this->QueueConfirm(APDUHeader::SolicitedConfirm(header.control.SEQ));
	}

	switch (result)
	{
	case(IMasterTask::ResponseResult::OK_CONTINUE) :
		this->StartResponseTimer();
		return TaskState::WAIT_FOR_RESPONSE;
	case(IMasterTask::ResponseResult::OK_REPEAT) :
		return StartTask_TaskReady();
	default:
		// task completed or failed, either way go back to idle
		this->CompleteActiveTask();
		return TaskState::IDLE;
	}
}

MContext::TaskState MContext::OnResponseTimeout_WaitForResponse()
{
	auto now = this->pExecutor->GetTime();
	this->pActiveTask->OnResponseTimeout(now);
	this->solSeq.Increment();
	this->CompleteActiveTask();
	return TaskState::IDLE;
}
}


