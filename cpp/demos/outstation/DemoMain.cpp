
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


#include <opendnp3/LogToStdio.h>
#include <opendnp3/DNP3Manager.h>
#include <opendnp3/SlaveStackConfig.h>
#include <opendnp3/IChannel.h>
#include <opendnp3/IOutstation.h>
#include <opendnp3/SimpleCommandHandler.h>

#include <string>
#include <iostream>

using namespace std;
using namespace opendnp3;

/*
 * Command line syntax:
 *
 *    ./outstationdemo [remote-dnp3] [local-dnp3] [local-ip] [local-port]
 *
 * Defaults:
 *
 *    remote-dnp3    100
 *    local-dnp3     1
 *    local-ip       127.0.0.1
 *    local-port     4999
 */
int main(int argc, char* argv[])
{
	// Default values
	unsigned remote_dnp3 = 100;
	unsigned local_dnp3  = 1;
	string   local_ip    = "127.0.0.1";
	unsigned local_port  = 4999;

	// Parse the command line arguments using a "fall-through"
	// switch statement.
	if (argc > 1 && std::strcmp("help", argv[1]) == 0) {
		cout << argv[0] << " [remote-dnp3] [local-dnp3] [local-ip] [local-port]" << endl;
		return -1;
	}

	switch (argc) {
	case 5:
		{
			istringstream iss(argv[4]);
			iss >> local_port;
		}
	case 4:
		local_ip = argv[3];
	case 3:
		{
			istringstream iss(argv[2]);
			iss >> local_dnp3;
		}
	case 2:
		{
			istringstream iss(argv[1]);
			iss >> remote_dnp3;
		}
	}	

	// Specify a FilterLevel for the stack/physical layer to use.
	// Log statements with a lower priority will not be logged.
	const FilterLevel LOG_LEVEL = LEV_INFO;	

	// This is the main point of interaction with the stack
	DNP3Manager mgr(1); // only 1 thread is needed for a single stack

	mgr.AddLogSubscriber(LogToStdio::Inst());

	// Add a TCPServer to the manager with the name "tcpserver".
	// The server will wait 3000 ms in between failed bind calls.
	auto pServer = mgr.AddTCPServer("tcpserver", LOG_LEVEL, 3000, local_ip, local_port);

	// The master config object for a slave. The default are
	// useable, but understanding the options are important.
	SlaveStackConfig stackConfig;

	// Override the default link addressing
	stackConfig.link.LocalAddr  = local_dnp3;
	stackConfig.link.RemoteAddr = remote_dnp3;

	// The DeviceTemplate struct specifies the structure of the
	// slave's database
	DeviceTemplate device(5, 5, 5, 5, 5);
	stackConfig.device = device;

	// Create a new slave with a log level, command handler, and 
	// config info this	returns a thread-safe interface used for 
	// updating the slave's database.
	auto pOutstation = pServer->AddOutstation("outstation", LOG_LEVEL, SuccessCommandHandler::Inst(), stackConfig);
	auto pDataObserver = pOutstation->GetDataObserver();

	std::string input;
	uint32_t count = 0;
	do
	{
		std::cout << "Enter something to increment a counter or type exit" << std::endl;
		std::cin >> input;
		if(input == "exit") break;
		else {
			Transaction t(pDataObserver); //automatically calls Start()/End()
			pDataObserver->Update(Counter(count), 0);
			++count;
		}
	}
	while(true);

	return 0;
}
