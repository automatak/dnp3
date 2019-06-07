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
#include "MeasurementHandler.h"

#include "app/parsing/APDUParser.h"

#include <log4cpp/LogMacros.h>

namespace opendnp3
{

ParseResult MeasurementHandler::ProcessMeasurements(ResponseInfo info, const ser4cpp::rseq_t& objects,
                                                    log4cpp::Logger& logger,
                                                    ISOEHandler* pHandler)
{
    MeasurementHandler handler(info, logger, pHandler);
    return APDUParser::Parse(objects, handler, &logger);
}

MeasurementHandler::MeasurementHandler(ResponseInfo info, const log4cpp::Logger& logger, ISOEHandler* pSOEHandler)
    : info(info),
	  logger(logger),
      txInitiated(false),
      pSOEHandler(pSOEHandler),
      ctoMode(TimestampMode::INVALID),
      commonTimeOccurence(0)
{
}

MeasurementHandler::~MeasurementHandler()
{
    if (txInitiated && pSOEHandler)
    {
        this->pSOEHandler->end_fragment(this->info);        
    }
}

TimestampMode MeasurementHandler::ModeFromType(GroupVariation gv)
{
    return HasAbsoluteTime(gv) ? TimestampMode::SYNCHRONIZED : TimestampMode::INVALID;
}

void MeasurementHandler::CheckForTxStart()
{
    if (!txInitiated && pSOEHandler)
    {
        txInitiated = true;
        this->pSOEHandler->begin_fragment(this->info);        
    }
}

IINField MeasurementHandler::ProcessHeader(const CountHeader& header, const ICollection<Group50Var1>& values)
{
    this->CheckForTxStart();

    auto transform = [](const Group50Var1& input) -> DNPTime { return input.time; };

    auto collection = Map<Group50Var1, DNPTime>(values, transform);

    HeaderInfo info(header.enumeration, header.GetQualifierCode(), TimestampMode::INVALID, header.headerIndex);
    this->pSOEHandler->Process(info, collection);

    return IINField();
}

IINField MeasurementHandler::ProcessHeader(const CountHeader& /*header*/, const ICollection<Group51Var1>& values)
{
    Group51Var1 cto;
    if (values.ReadOnlyValue(cto))
    {
        ctoMode = TimestampMode::SYNCHRONIZED;
        commonTimeOccurence = cto.time;
    }
    return IINField::Empty();
}

IINField MeasurementHandler::ProcessHeader(const CountHeader& /*header*/, const ICollection<Group51Var2>& values)
{
    Group51Var2 cto;
    if (values.ReadOnlyValue(cto))
    {
        ctoMode = TimestampMode::UNSYNCHRONIZED;
        commonTimeOccurence = cto.time;
    }

    return IINField::Empty();
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Binary>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header,
                                           const ICollection<Indexed<DoubleBitBinary>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header,
                                           const ICollection<Indexed<BinaryOutputStatus>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Counter>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<FrozenCounter>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Analog>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header,
                                           const ICollection<Indexed<AnalogOutputStatus>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<OctetString>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header,
                                           const ICollection<Indexed<TimeAndInterval>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const RangeHeader& header, const ICollection<Indexed<Group121Var1>>& values)
{
    return this->LoadValuesWithTransformTo<SecurityStat>(header, values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Binary>>& values)
{
    if (header.enumeration == GroupVariation::Group2Var3)
    {
        return this->ProcessWithCTO(header, values);
    }

    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header,
                                           const ICollection<Indexed<BinaryOutputStatus>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header,
                                           const ICollection<Indexed<DoubleBitBinary>>& values)
{
    if (header.enumeration == GroupVariation::Group4Var3)
    {
        return this->ProcessWithCTO(header, values);
    }

    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Counter>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header,
                                           const ICollection<Indexed<FrozenCounter>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Analog>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header,
                                           const ICollection<Indexed<AnalogOutputStatus>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<OctetString>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header,
                                           const ICollection<Indexed<BinaryCommandEvent>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header,
                                           const ICollection<Indexed<AnalogCommandEvent>>& values)
{
    return this->LoadValues(header, ModeFromType(header.enumeration), values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var1>>& values)
{
    return this->LoadValuesWithTransformTo<SecurityStat>(header, values);
}

IINField MeasurementHandler::ProcessHeader(const PrefixHeader& header, const ICollection<Indexed<Group122Var2>>& values)
{
    return this->LoadValuesWithTransformTo<SecurityStat>(header, values);
}

// ----------- Conversion routines from DNP3 types to API types -----------

SecurityStat MeasurementHandler::Convert(const Group121Var1& meas)
{
    return SecurityStat(meas.flags, meas.assocId, meas.value);
}

SecurityStat MeasurementHandler::Convert(const Group122Var1& meas)
{
    return SecurityStat(meas.flags, meas.assocId, meas.value);
}

SecurityStat MeasurementHandler::Convert(const Group122Var2& meas)
{
    return SecurityStat(meas.flags, meas.assocId, meas.value, meas.time);
}

} // namespace opendnp3
