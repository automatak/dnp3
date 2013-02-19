
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the GNU Affero General Public License Version 3.0
// (the "Additional License"). You may not use these modifications except in
// compliance with the additional license. You may obtain a copy of the Additional
// License at
//
// http://www.gnu.org/licenses/agpl.html
//
// Contact Automatak, LLC for a commercial license to these modifications
//
#ifndef __I_CHANNEL_H_
#define __I_CHANNEL_H_

#include "MasterStackConfig.h"
#include "SlaveStackConfig.h"
#include "LogTypes.h"
#include "ChannelStates.h"
#include "DestructorHook.h"

#include <functional>

namespace opendnp3
{

class IDataObserver;
class IMaster;
class IOutstation;
class ICommandHandler;

/**
* Represents a communication channel upon which masters and outstations can be bound.
* To add a master to a communication channel, where pClient is a channel pointer IChannel*\n
* Returns a pointer to the running master
\code
	IMaster* pMaster = pClient->AddMaster(
		"master",						// stack name
		LOG_LEVEL,						// log filter level
		PrintingDataObserver::Inst(),	// callback for data processing
		stackConfig						// stack configuration
	);
\endcode
* To add an outstation to a communication channel, where pServer is a channel pointer IChannel*\n
* Returns a pointer to the running outstation
\code
	IOutstation* pOutstation = pServer->AddOutstation(
		"outstation",								// stack name
		LOG_LEVEL,									// log filter level
		SuccessCommandHandler::Inst(),				// callback for command requests
		stackConfig									// stack configuration
	);
\endcode
*/
class IChannel : public DestructorHook
{
	public:

		virtual ~IChannel() {}
		
		/**
		* Synchronously shutdown the channel.
		*/
		virtual void Shutdown() = 0;

		/**
		* Add a listener for changes to the channel state. All callbacks come from the thread pool.
		* An immediate callback will be made with the current state.
		*
		* @param aListener Functor to callback with the state enumeration 
		*/
		virtual void AddStateListener(std::function<void (ChannelState)> aListener) = 0;

		/**
		* Add a master to the channel
		*
		* @param arLoggerId Name that will be used in all log messages
		* @param aLevel Lowest log level that will be recorded
		* @param apPublisher Callback object for all received measurements
		* @param arCfg Configuration object that controls how the master behaves
		* @return interface representing the running master
		*/
		virtual IMaster* AddMaster(	const std::string& arLoggerId,
									FilterLevel aLevel,
									IDataObserver* apPublisher,
									const MasterStackConfig& arCfg) = 0;

		/**
		* Add an outstation to the channel
		*
		* @param arLoggerId Name that will be used in all log messages
		* @param aLevel Lowest log level that will be recorded
		* @param apCmdHandler Callback object for handling command requests
		* @param arCfg Configuration object that controls how the outstation behaves
		* @return interface representing the running outstations
		*/
		virtual IOutstation* AddOutstation(	const std::string& arLoggerId,
											FilterLevel aLevel,
											ICommandHandler* apCmdHandler,
											const SlaveStackConfig& arCfg) = 0;
};

}

#endif
