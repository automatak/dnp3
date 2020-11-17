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
#include "utils/LinkHex.h"
#include "utils/LinkLayerTest.h"
#include "utils/MockTransportSegment.h"

#include <ser4cpp/container/Buffer.h>
#include <ser4cpp/util/HexConversions.h>

#include <catch.hpp>
#include <link/LinkFrame.h>

#include <iostream>

using namespace opendnp3;
using namespace ser4cpp;

#define SUITE(name) "LinkLayerKeepAliveTestSuite - " name

TEST_CASE(SUITE("Timers activated and canceled in response to layer up/down"))
{
    LinkLayerTest t;
    REQUIRE(t.exe->num_pending_timers() == 0);
    t.link.OnLowerLayerUp();
    REQUIRE(t.exe->num_pending_timers() == 1);
    t.link.OnLowerLayerDown();
    REQUIRE(t.exe->num_pending_timers() == 0);
}

TEST_CASE(SUITE("ForwardsKeepAliveTimeouts"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveTransmissions == 0);

    REQUIRE(t.exe->advance_to_next_timer());
    REQUIRE(t.exe->run_many() > 0);
    REQUIRE(t.listener->numKeepAliveTransmissions == 1);
}

TEST_CASE(SUITE("KeepAliveStopsOnAck"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveTransmissions == 0);

    REQUIRE(t.exe->advance_to_next_timer());
    REQUIRE(t.exe->run_many() > 0);

    REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
    REQUIRE(t.exe->num_pending_timers() == 2);
    t.link.OnTxReady();
    REQUIRE(t.exe->num_pending_timers() == 2);
    t.OnFrame(LinkFunction::SEC_ACK, false, false, false, 1, 1024);
    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveFailure == 0); // Not reported as a failure, because there is activity on the link
}

TEST_CASE(SUITE("KeepAliveStopsOnNack"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveTransmissions == 0);

    REQUIRE(t.exe->advance_to_next_timer());
    REQUIRE(t.exe->run_many() > 0);

    REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
    REQUIRE(t.exe->num_pending_timers() == 2);
    t.link.OnTxReady();
    REQUIRE(t.exe->num_pending_timers() == 2);
    t.OnFrame(LinkFunction::SEC_NACK, false, false, false, 1, 1024);
    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveFailure == 0); // Not reported as a failure, because there is activity on the link
}

TEST_CASE(SUITE("KeepAliveFailureCallbackIsInvokedOnTimeout"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveTransmissions == 0);

    REQUIRE(t.exe->advance_to_next_timer());
    REQUIRE(t.exe->run_many() > 0);

    REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
    REQUIRE(t.exe->num_pending_timers() == 2);
    t.link.OnTxReady();
    REQUIRE(t.exe->num_pending_timers() == 2);
    t.exe->advance_time(config.Timeout.value);
    REQUIRE(t.exe->run_many() > 0);
    REQUIRE(t.listener->numKeepAliveFailure == 1);
}

TEST_CASE(SUITE("KeepAliveSuccessCallbackIsInvokedWhenLinkStatusReceived"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveTransmissions == 0);

    REQUIRE(t.exe->advance_to_next_timer());
    REQUIRE(t.exe->run_many() > 0);

    REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
    REQUIRE(t.exe->num_pending_timers() == 2);
    t.link.OnTxReady();
    REQUIRE(t.exe->num_pending_timers() == 2);
    t.OnFrame(LinkFunction::SEC_LINK_STATUS, false, false, false, 1, 1024);
    REQUIRE(t.listener->numKeepAliveReplys == 1);
    REQUIRE(t.exe->num_pending_timers() == 1);
}

TEST_CASE(SUITE("KeepAliveIsPeriodicOnFailure"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    for (int count = 0; count < 3; ++count)
    {
        REQUIRE(t.exe->num_pending_timers() == 1);
        REQUIRE(t.listener->numKeepAliveTransmissions == count);

        REQUIRE(t.exe->advance_to_next_timer());
        REQUIRE(t.exe->run_many() > 0);

        REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
        REQUIRE(t.exe->num_pending_timers() == 2);
        t.link.OnTxReady();
        REQUIRE(t.exe->num_pending_timers() == 2);

        t.exe->advance_time(config.Timeout.value);
        REQUIRE(t.exe->run_many() > 0);
        REQUIRE(t.listener->numKeepAliveFailure == (count + 1));
    }
}

TEST_CASE(SUITE("KeepAliveIsPeriodicOnSuccess"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    for (int count = 0; count < 3; ++count)
    {
        REQUIRE(t.exe->num_pending_timers() == 1);
        REQUIRE(t.listener->numKeepAliveTransmissions == count);

        REQUIRE(t.exe->advance_to_next_timer());
        REQUIRE(t.exe->run_many() > 0);

        REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
        REQUIRE(t.exe->num_pending_timers() == 2);
        t.link.OnTxReady();
        REQUIRE(t.exe->num_pending_timers() == 2);
        t.OnFrame(LinkFunction::SEC_LINK_STATUS, false, false, false, 1, 1024);
        REQUIRE(t.listener->numKeepAliveReplys == (count + 1));
    }
}

TEST_CASE(SUITE("KeepAliveSuccessCallbackIsInvokedWhenLinkStatusReceivedBeforeTransmitComplete"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveTransmissions == 0);

    REQUIRE(t.exe->advance_to_next_timer());
    REQUIRE(t.exe->run_many() > 0);

    REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
    t.OnFrame(LinkFunction::SEC_LINK_STATUS, false, false, false, 1, 1024);
    t.link.OnTxReady();
    REQUIRE(t.listener->numKeepAliveReplys == 1);
    REQUIRE(t.exe->num_pending_timers() == 1);
}

TEST_CASE(SUITE("KeepAliveTimerRestartsWhenFrameIsReceived"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Seconds(5);
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveTransmissions == 0);

    t.exe->advance_time(std::chrono::seconds(2));
    t.OnFrame(LinkFunction::SEC_ACK, false, false, false, 1, 1024); // Some activity on the link

    t.exe->advance_time(std::chrono::seconds(3)); // The original timer should now expire
    REQUIRE(t.exe->run_many() > 0);
    REQUIRE(t.PopLastWriteAsHex() == ""); // Nothing sent

    t.exe->advance_time(std::chrono::seconds(2)); // The keep-alive should now be sent
    REQUIRE(t.exe->run_many() > 0);
    REQUIRE(t.PopLastWriteAsHex() == LinkHex::RequestLinkStatus(true, 1024, 1));
}

TEST_CASE(SUITE("KeepAliveDisabled"))
{
    LinkConfig config(true, false);
    config.KeepAliveTimeout = TimeDuration::Max();
    LinkLayerTest t(config);

    t.link.OnLowerLayerUp();

    REQUIRE(t.exe->num_pending_timers() == 1);
    REQUIRE(t.listener->numKeepAliveTransmissions == 0);

    t.exe->advance_time(std::chrono::seconds(10));
    t.OnFrame(LinkFunction::SEC_ACK, false, false, false, 1, 1024);

    t.exe->advance_time(std::chrono::hours(24));
    REQUIRE(t.exe->run_many() == 0);
}
