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
#include <opendnp3/SimpleDataObserver.h>

#include "MeasurementHelpers.h"

#include <sstream>

namespace opendnp3
{

SimpleDataObserver::SimpleDataObserver(std::function<void (const std::string&)> aOutputFunc) :
	mOutputFunc(aOutputFunc)
{}

void SimpleDataObserver::Start()
{}

void SimpleDataObserver::End()
{}

void SimpleDataObserver::Update(const Binary& arPoint, uint16_t aIndex)
{
	std::ostringstream oss;
	oss << ToString(arPoint) << " : " << aIndex;
	mOutputFunc(oss.str());
}

void SimpleDataObserver::Update(const Analog& arPoint, uint16_t aIndex)
{
	std::ostringstream oss;
	oss << ToString(arPoint) << " : " << aIndex;
	mOutputFunc(oss.str());
}

void SimpleDataObserver::Update(const Counter& arPoint, uint16_t aIndex)
{
	std::ostringstream oss;
	oss << ToString(arPoint) << " : " << aIndex;
	mOutputFunc(oss.str());
}

void SimpleDataObserver::Update(const FrozenCounter& arPoint, uint16_t aIndex)
{
        std::ostringstream oss;
        oss << ToString(arPoint) << " : " << aIndex;
        mOutputFunc(oss.str());
}

void SimpleDataObserver::Update(const ControlStatus& arPoint, uint16_t aIndex)
{
	std::ostringstream oss;
	oss << ToString(arPoint) << " : " << aIndex;
	mOutputFunc(oss.str());
}

void SimpleDataObserver::Update(const SetpointStatus& arPoint, uint16_t aIndex)
{
	std::ostringstream oss;
	oss << ToString(arPoint) << " : " << aIndex;
	mOutputFunc(oss.str());
}

NullDataObserver NullDataObserver::mInstance;

PrintingDataObserver PrintingDataObserver::mInstance;

}
