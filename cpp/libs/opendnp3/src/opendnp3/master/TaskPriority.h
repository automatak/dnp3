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
#ifndef OPENDNP3_TASKPRIORITY_H
#define OPENDNP3_TASKPRIORITY_H

namespace opendnp3
{

	namespace priority
	{
		const int COMMAND = 0;
		
		const int CLEAR_RESTART = 10;
		
		const int DISABLE_UNSOLICITED = 20;

		const int ASSIGN_CLASS = 30;
				
		const int INTEGRITY_POLL = 40;

		const int TIME_SYNC = 50;		

		const int ENABLE_UNSOLICITED = 60;

		const int USER_POLL = 70;

	}

}

#endif
