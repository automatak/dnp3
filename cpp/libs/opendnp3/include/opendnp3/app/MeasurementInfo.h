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
#ifndef OPENDNP3_MEASUREMENTINFO_H
#define OPENDNP3_MEASUREMENTINFO_H

#include <ser4cpp/util/Uncopyable.h>

#include "opendnp3/app/EventType.h"
#include "opendnp3/app/MeasurementTypes.h"
#include "opendnp3/app/OctetString.h"
#include "opendnp3/app/SecurityStat.h"
#include "opendnp3/gen/BinaryQuality.h"
#include "opendnp3/gen/EventAnalogOutputStatusVariation.h"
#include "opendnp3/gen/EventAnalogVariation.h"
#include "opendnp3/gen/EventBinaryOutputStatusVariation.h"
#include "opendnp3/gen/EventBinaryVariation.h"
#include "opendnp3/gen/EventCounterVariation.h"
#include "opendnp3/gen/EventDoubleBinaryVariation.h"
#include "opendnp3/gen/EventFrozenCounterVariation.h"
#include "opendnp3/gen/EventOctetStringVariation.h"
#include "opendnp3/gen/EventSecurityStatVariation.h"
#include "opendnp3/gen/StaticAnalogOutputStatusVariation.h"
#include "opendnp3/gen/StaticAnalogVariation.h"
#include "opendnp3/gen/StaticBinaryOutputStatusVariation.h"
#include "opendnp3/gen/StaticBinaryVariation.h"
#include "opendnp3/gen/StaticCounterVariation.h"
#include "opendnp3/gen/StaticDoubleBinaryVariation.h"
#include "opendnp3/gen/StaticFrozenCounterVariation.h"
#include "opendnp3/gen/StaticOctetStringVariation.h"
#include "opendnp3/gen/StaticSecurityStatVariation.h"
#include "opendnp3/gen/StaticTimeAndIntervalVariation.h"
#include "opendnp3/gen/StaticTypeBitmask.h"

namespace opendnp3
{

struct BinaryInfo : private ser4cpp::StaticOnly
{
    typedef Binary meas_t;
    typedef bool value_t;
    typedef EventBinaryVariation event_variation_t;
    typedef StaticBinaryVariation static_variation_t;

    static const EventType EventTypeEnum = EventType::Binary;
    static const StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::BinaryInput;
    static const event_variation_t DefaultEventVariation = EventBinaryVariation::Group2Var1;
    static const static_variation_t DefaultStaticVariation = StaticBinaryVariation::Group1Var2;
};

struct DoubleBitBinaryInfo : private ser4cpp::StaticOnly
{
    typedef DoubleBitBinary meas_t;
    typedef DoubleBit value_t;
    typedef EventDoubleBinaryVariation event_variation_t;
    typedef StaticDoubleBinaryVariation static_variation_t;

    static const EventType EventTypeEnum = EventType::DoubleBitBinary;
    static const StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::DoubleBinaryInput;
    static const event_variation_t DefaultEventVariation = EventDoubleBinaryVariation::Group4Var1;
    static const static_variation_t DefaultStaticVariation = StaticDoubleBinaryVariation::Group3Var2;
};

class BinaryOutputStatusInfo : private ser4cpp::StaticOnly
{
public:
    typedef BinaryOutputStatus meas_t;
    typedef bool value_t;
    typedef EventBinaryOutputStatusVariation event_variation_t;
    typedef StaticBinaryOutputStatusVariation static_variation_t;

    static const EventType EventTypeEnum = EventType::BinaryOutputStatus;
    static const StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::BinaryOutputStatus;
    static const event_variation_t DefaultEventVariation = EventBinaryOutputStatusVariation::Group11Var1;
    static const static_variation_t DefaultStaticVariation = StaticBinaryOutputStatusVariation::Group10Var2;
};

struct AnalogInfo : private ser4cpp::StaticOnly
{
    typedef Analog meas_t;
    typedef double value_t;
    typedef EventAnalogVariation event_variation_t;
    typedef StaticAnalogVariation static_variation_t;

    static const EventType EventTypeEnum = EventType::Analog;
    static const StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::AnalogInput;
    static const event_variation_t DefaultEventVariation = EventAnalogVariation::Group32Var1;
    static const static_variation_t DefaultStaticVariation = StaticAnalogVariation::Group30Var1;
};

struct CounterInfo : private ser4cpp::StaticOnly
{
    typedef Counter meas_t;
    typedef uint32_t value_t;
    typedef EventCounterVariation event_variation_t;
    typedef StaticCounterVariation static_variation_t;

    static const EventType EventTypeEnum = EventType::Counter;
    static const StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::Counter;
    static const event_variation_t DefaultEventVariation = EventCounterVariation::Group22Var1;
    static const static_variation_t DefaultStaticVariation = StaticCounterVariation::Group20Var1;
};

struct FrozenCounterInfo : private ser4cpp::StaticOnly
{
    typedef FrozenCounter meas_t;
    typedef uint32_t value_t;
    typedef EventFrozenCounterVariation event_variation_t;
    typedef StaticFrozenCounterVariation static_variation_t;

    static const EventType EventTypeEnum = EventType::FrozenCounter;
    static const StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::FrozenCounter;
    static const event_variation_t DefaultEventVariation = EventFrozenCounterVariation::Group23Var1;
    static const static_variation_t DefaultStaticVariation = StaticFrozenCounterVariation::Group21Var1;
};

struct AnalogOutputStatusInfo : private ser4cpp::StaticOnly
{
    typedef AnalogOutputStatus meas_t;
    typedef double value_t;
    typedef EventAnalogOutputStatusVariation event_variation_t;
    typedef StaticAnalogOutputStatusVariation static_variation_t;

    static const EventType EventTypeEnum = EventType::AnalogOutputStatus;
    static const StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::AnalogOutputStatus;
    static const event_variation_t DefaultEventVariation = EventAnalogOutputStatusVariation::Group42Var1;
    static const static_variation_t DefaultStaticVariation = StaticAnalogOutputStatusVariation::Group40Var1;
};

struct OctetStringInfo : private ser4cpp::StaticOnly
{
    typedef OctetString meas_t;
    typedef EventOctetStringVariation event_variation_t;
    typedef StaticOctetStringVariation static_variation_t;

    static const EventType EventTypeEnum = EventType::OctetString;
    static const StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::OctetString;
    static const event_variation_t DefaultEventVariation = EventOctetStringVariation::Group111Var0;
    static const static_variation_t DefaultStaticVariation = StaticOctetStringVariation::Group110Var0;
};

struct TimeAndIntervalInfo : private ser4cpp::StaticOnly
{
    typedef TimeAndInterval meas_t;
    typedef StaticTimeAndIntervalVariation static_variation_t;

    const static StaticTypeBitmask StaticTypeEnum = StaticTypeBitmask::TimeAndInterval;
    const static StaticTimeAndIntervalVariation DefaultStaticVariation = StaticTimeAndIntervalVariation::Group50Var4;
};

struct SecurityStatInfo : private ser4cpp::StaticOnly
{
    typedef SecurityStat meas_t;
    typedef SecurityStat::Value value_t;
    typedef EventSecurityStatVariation event_variation_t;
    typedef StaticSecurityStatVariation static_variation_t;

    const static event_variation_t DefaultEventVariation = EventSecurityStatVariation::Group122Var1;
    const static static_variation_t DefaultStaticVariation = StaticSecurityStatVariation::Group121Var1;
};

} // namespace opendnp3

#endif
