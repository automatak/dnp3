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
#include "CountParser.h"

#include "opendnp3/LogLevels.h"

#include <log4cpp/LogMacros.h>

namespace opendnp3
{

CountParser::CountParser(uint16_t count, uint32_t requiredSize, HandleFun handler)
    : count(count), requiredSize(requiredSize), handler(handler)
{
}

ParseResult CountParser::Process(const HeaderRecord& record,
                                 ser4cpp::rseq_t& buffer,
                                 IAPDUHandler* pHandler,
                                 log4cpp::Logger* pLogger) const
{
    if (buffer.length() < requiredSize)
    {
        SIMPLE_LOGGER_BLOCK(pLogger, flags::WARN, "Not enough data for specified objects");
        return ParseResult::NOT_ENOUGH_DATA_FOR_OBJECTS;
    }

    if (pHandler != nullptr)
    {
        handler(record, count, buffer, *pHandler);
    }
    buffer.advance(requiredSize);
    return ParseResult::OK;
}

ParseResult CountParser::ParseHeader(ser4cpp::rseq_t& buffer,
                                     const NumParser& numParser,
                                     const ParserSettings& settings,
                                     const HeaderRecord& record,
                                     log4cpp::Logger* pLogger,
                                     IAPDUHandler* pHandler)
{
    uint16_t count;
    auto result = numParser.ParseCount(buffer, count, pLogger);
    if (result == ParseResult::OK)
    {
        FORMAT_LOGGER_BLOCK(pLogger, settings.LogLevel(), "%03u,%03u %s, %s [%u]", record.group, record.variation,
                            GroupVariationToString(record.enumeration),
                            QualifierCodeToString(record.GetQualifierCode()), count);

        if (settings.ExpectsContents())
        {
            return ParseCountOfObjects(buffer, record, count, pLogger, pHandler);
        }

        if (pHandler != nullptr)
        {
            pHandler->OnHeader(CountHeader(record, count));
        }

        return ParseResult::OK;
    }
    else
    {
        return result;
    }
}

ParseResult CountParser::ParseCountOfObjects(ser4cpp::rseq_t& buffer,
                                             const HeaderRecord& record,
                                             uint16_t count,
                                             log4cpp::Logger* pLogger,
                                             IAPDUHandler* pHandler)
{
    switch (record.enumeration)
    {
    case (GroupVariation::Group50Var1):
        return CountParser::From<Group50Var1>(count).Process(record, buffer, pHandler, pLogger);

    case (GroupVariation::Group50Var3):
        return CountParser::From<Group50Var3>(count).Process(record, buffer, pHandler, pLogger);

    case (GroupVariation::Group51Var1):
        return CountParser::From<Group51Var1>(count).Process(record, buffer, pHandler, pLogger);

    case (GroupVariation::Group51Var2):
        return CountParser::From<Group51Var2>(count).Process(record, buffer, pHandler, pLogger);

    case (GroupVariation::Group52Var1):
        return CountParser::From<Group52Var1>(count).Process(record, buffer, pHandler, pLogger);

    case (GroupVariation::Group52Var2):
        return CountParser::From<Group52Var2>(count).Process(record, buffer, pHandler, pLogger);

    case (GroupVariation::Group120Var3):
        return CountParser::From<Group120Var3>(count).Process(record, buffer, pHandler, pLogger);

    case (GroupVariation::Group120Var4):
        return CountParser::From<Group120Var4>(count).Process(record, buffer, pHandler, pLogger);

    default:
        FORMAT_LOGGER_BLOCK(pLogger, flags::WARN, "Unsupported qualifier/object - %s - %i / %i",
                            QualifierCodeToString(record.GetQualifierCode()), record.group, record.variation);

        return ParseResult::INVALID_OBJECT_QUALIFIER;
    }
}

} // namespace opendnp3
