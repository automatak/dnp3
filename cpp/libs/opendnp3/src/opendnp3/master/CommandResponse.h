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
#ifndef OPENDNP3_COMMANDRESPONSE_H
#define OPENDNP3_COMMANDRESPONSE_H

#include "opendnp3/gen/CommandStatus.h"
#include "opendnp3/gen/UserTaskResult.h"



namespace opendnp3
{

/**
* Represents the result of a command request
*/
class CommandResponse
{
public:

	static const CommandResponse Success;

	CommandResponse(UserTaskResult result = UserTaskResult::NO_COMMS, CommandStatus status = CommandStatus::UNDEFINED);

	static CommandResponse OK(CommandStatus status);
	static CommandResponse NoResponse(UserTaskResult result);


	///  The result of the operation, should be examined before looking at the status code
	UserTaskResult GetResult() const;

	/// The command status enumeration received from the outstation, if applicable
	CommandStatus GetStatus() const;

	bool operator==(const CommandResponse& arRHS) const;	

private:

	UserTaskResult mResult;
	CommandStatus mStatus;
};

}



#endif
