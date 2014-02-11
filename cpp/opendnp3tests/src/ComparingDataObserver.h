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
#ifndef __COMPARING_DATA_OBSERVER_H_
#define __COMPARING_DATA_OBSERVER_H_

#include <opendnp3/IDataObserver.h>

#include <opendnp3/MeasurementHelpers.h>

#include <openpal/TimeDuration.h>

#include "FlexibleDataObserver.h"
#include "MeasurementComparisons.h"

#include <map>
#include <mutex>
#include <iostream>
#include <condition_variable>

namespace opendnp3
{

class ComparingDataObserver : public IDataObserver, public IMeasurementHandler
{
public:

	ComparingDataObserver(FlexibleDataObserver* apObserver);

	void Reset();

	bool WaitForSameData(openpal::TimeDuration aTimeout);

	void DescribeMissingData();

	void Load(const IMeasurementUpdate& arUpdate);

	void Update(const Binary& arPoint, uint16_t aIndex) final;
	void Update(const Analog& arPoint, uint16_t aIndex) final;
	void Update(const Counter& arPoint, uint16_t aIndex) final;
	void Update(const FrozenCounter& arPoint, uint16_t aIndex) final;
	void Update(const ControlStatus& arPoint, uint16_t aIndex) final;
	void Update(const SetpointStatus& arPoint, uint16_t aIndex) final;

private:

	bool mSameData;

	bool IsSameData();

	std::mutex mMutex;
	std::condition_variable mCondition;
	FlexibleDataObserver* mpObserver;

	typedef std::map<size_t, bool> CompareMap;

	CompareMap mBinaryMap;
	CompareMap mAnalogMap;
	CompareMap mCounterMap;
	CompareMap mFrozenCounterMap;
	CompareMap mControlStatusMap;
	CompareMap mSetpointStatusMap;

	void Start();
	void End();	

	template <class T>
	void UpdateAny(const T& arPoint, size_t aIndex, const typename PointMap<T>::Type& arMap, CompareMap& arCompareMap);

	template <class T>
	void DescribeAny(const typename PointMap<T>::Type& arMap, const CompareMap& arCompareMap);
};


template <class T>
void ComparingDataObserver::DescribeAny(const typename PointMap<T>::Type& arMap, const CompareMap& arCompareMap)
{
	for(auto pair: arMap) {
		CompareMap::const_iterator j = arCompareMap.find(pair.first);
		if(j == arCompareMap.end()) {
			std::cout << "Missing: " << pair.first << " - " << ToString(pair.second) << std::endl;
		}
	}
}

template <class T>
void ComparingDataObserver::UpdateAny(const T& arPoint, size_t aIndex, const typename PointMap<T>::Type& arMap, CompareMap& arCompareMap)
{
	typename PointMap<T>::Type::const_iterator i = arMap.find(aIndex);
	if(i == arMap.end()) {
		std::cout << "Unexpected index: " << aIndex << " - " << ToString(arPoint) << std::endl;
	}
	else {
		if(i->second == arPoint) {
			arCompareMap[aIndex] = true;
		}
		else {
			//std::cout << ToString(arPoint) << " not equal to " << ToString(i->second) << std::endl;
			arCompareMap.erase(aIndex);
		}
	}
}

}

#endif

