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
#ifndef OPENDNP3_TASKBITMASK_H
#define OPENDNP3_TASKBITMASK_H

#include <cstdint>

namespace opendnp3 { namespace tasks {

enum TaskBitmask : int32_t
{
	DISABLE_UNSOLCITED = 0x01,
	CLEAR_RESTART_IIN = 0x02,
	STARTUP_INTEGRITY = 0x04,
	TIME_SYNC = 0x08,
	ENABLE_UNSOLCITED = 0x10,

	// aggregate masks for startup sequences
	STARTUP_TASK_SEQUENCE = DISABLE_UNSOLCITED | STARTUP_INTEGRITY | ENABLE_UNSOLCITED,
	CLEAR_RESTART_SEQUENCE = CLEAR_RESTART_IIN | STARTUP_INTEGRITY | ENABLE_UNSOLCITED


};

}}

#endif
