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
#include <opendnp3/LogLevels.h>
#include <opendnp3/app/ITransactable.h>
#include <outstation/Database.h>
#include <outstation/OutstationContext.h>

#include <dnp3mocks/MockCommandHandler.h>
#include <dnp3mocks/MockLowerLayer.h>
#include <dnp3mocks/MockOutstationApplication.h>

#include <exe4cpp/MockExecutor.h>
#include <log4cpp/MockLogHandler.h>

#include <functional>

class OutstationTestObject
{
public:
    OutstationTestObject(const opendnp3::OutstationConfig& config,
                         const opendnp3::DatabaseSizes& dbSizes = opendnp3::DatabaseSizes::Empty())
        : log("test"),
          exe(std::make_shared<exe4cpp::MockExecutor>()),
          lower(std::make_shared<MockLowerLayer>()),
          cmdHandler(std::make_shared<MockCommandHandler>(opendnp3::CommandStatus::SUCCESS)),
          application(std::make_shared<MockOutstationApplication>()),
          context(opendnp3::Addresses(), config, dbSizes, log.logger, exe, lower, cmdHandler, application)
    {
        lower->SetUpperLayer(context);
    }

    size_t SendToOutstation(const ser4cpp::rseq_t& buffer)
    {
        context.OnReceive(opendnp3::Message(opendnp3::Addresses(), buffer));
        return exe->run_many();
    }

    size_t LowerLayerUp()
    {
        context.OnLowerLayerUp();
        return exe->run_many();
    }

private:
    log4cpp::MockLogHandler log;
    const std::shared_ptr<exe4cpp::MockExecutor> exe;
    const std::shared_ptr<MockLowerLayer> lower;
    const std::shared_ptr<MockCommandHandler> cmdHandler;
    const std::shared_ptr<MockOutstationApplication> application;
    opendnp3::OContext context;
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
    ser4cpp::rseq_t buffer(Data, static_cast<uint32_t>(Size));

    opendnp3::OutstationConfig config;
    OutstationTestObject t(config);
    t.LowerLayerUp();
    t.SendToOutstation(buffer);
    return 0;
}
