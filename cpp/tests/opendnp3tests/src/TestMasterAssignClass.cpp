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
#include <catch.hpp>

#include "MasterTestObject.h"
#include "APDUHexBuilders.h"
#include "HexConversions.h"

using namespace std;
using namespace opendnp3;
using namespace openpal;

#define SUITE(name) "MasterAssignClassTestSuite - " name

TEST_CASE(SUITE("AssignsClassAfterConnect"))
{
	MasterTestObject t(NoStartupTasks());

	// configure the mock application to do assign class on startup
	t.application.assignClass = [](HeaderWriter& writer) {
		writer.WriteHeader(GroupVariationID(60,2), QualifierCode::ALL_OBJECTS);
		writer.WriteHeader(GroupVariationID(3,0), QualifierCode::ALL_OBJECTS);
	};
		
	t.master.OnLowerLayerUp();

	REQUIRE(t.exe.RunMany() > 0);

	REQUIRE(t.lower.PopWriteAsHex() == "C0 16 3C 02 06 03 00 06");
	t.master.OnSendResult(true);
	t.SendToMaster("C0 81 00 00");
	
	t.exe.RunMany();
	REQUIRE(!t.exe.AdvanceToNextTimer());
	REQUIRE(t.lower.PopWriteAsHex() == "");

	REQUIRE(t.application.taskEvents.size() == 2);
	REQUIRE(t.application.taskEvents[0].first.id == (int) TaskIds::ASSIGN_CLASS);
	REQUIRE(t.application.taskEvents[0].first.isUserAssigned == false);
	REQUIRE(t.application.taskEvents[0].second == TaskState::RUNNING);
	REQUIRE(t.application.taskEvents[1].second == TaskState::SUCCESS);
}

TEST_CASE(SUITE("DisableUnsolBeforeAssignClass"))
{
	auto params = NoStartupTasks();
	params.disableUnsolOnStartup = true;
	MasterTestObject t(params);

	// configure the mock application to do assign class on startup
	t.application.assignClass = [](HeaderWriter& writer) {
		writer.WriteHeader(GroupVariationID(60, 2), QualifierCode::ALL_OBJECTS);
		writer.WriteHeader(GroupVariationID(3, 0), QualifierCode::ALL_OBJECTS);
	};

	t.master.OnLowerLayerUp();

	REQUIRE(t.exe.RunMany() > 0);

	REQUIRE(t.lower.PopWriteAsHex() == hex::ClassTask(FunctionCode::DISABLE_UNSOLICITED, 0, ClassField::AllEventClasses()));

}