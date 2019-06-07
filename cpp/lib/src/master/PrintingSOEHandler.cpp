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
#include "opendnp3/master/PrintingSOEHandler.h"

namespace opendnp3
{

std::ostream& operator<<(std::ostream& os, const ResponseInfo& info)
{
    os << "unsolicited: " << info.unsolicited << " fir: " << info.fir << " fin: " << info.fin;    
    return os;
}

void PrintingSOEHandler::begin_fragment(const ResponseInfo& info)
{
    std::cout << "begin response: " << info << std::endl;
}

void PrintingSOEHandler::end_fragment(const ResponseInfo& info)
{
    std::cout << "end response: " << info << std::endl;
}

void PrintingSOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<Binary>>& values)
{
    return PrintAll(info, values);
}

void PrintingSOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<DoubleBitBinary>>& values)
{
    return PrintAll(info, values);
}

void PrintingSOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<Analog>>& values)
{
    return PrintAll(info, values);
}

void PrintingSOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<Counter>>& values)
{
    return PrintAll(info, values);
}

void PrintingSOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<FrozenCounter>>& values)
{
    return PrintAll(info, values);
}

void PrintingSOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<BinaryOutputStatus>>& values)
{
    return PrintAll(info, values);
}

void PrintingSOEHandler::Process(const HeaderInfo& info, const ICollection<Indexed<AnalogOutputStatus>>& values)
{
    return PrintAll(info, values);
}

void PrintingSOEHandler::Process(const HeaderInfo& /*info*/, const ICollection<Indexed<OctetString>>& values)
{
    auto print = [](const Indexed<OctetString>& pair) {
        std::cout << "OctetString "
                  << " [" << pair.index << "] : Size : " << pair.value.Size() << std::endl;
    };

    values.ForeachItem(print);
}

void PrintingSOEHandler::Process(const HeaderInfo& /*info*/, const ICollection<Indexed<TimeAndInterval>>& values)
{
    auto print = [](const Indexed<TimeAndInterval>& pair) {
        std::cout << "TimeAndInterval: "
                  << "[" << pair.index << "] : " << pair.value.time.value << " : " << pair.value.interval << " : "
                  << IntervalUnitsToString(pair.value.GetUnitsEnum()) << std::endl;
    };

    values.ForeachItem(print);
}

void PrintingSOEHandler::Process(const HeaderInfo& /*info*/, const ICollection<Indexed<BinaryCommandEvent>>& values)
{
    auto print = [](const Indexed<BinaryCommandEvent>& pair) {
        std::cout << "BinaryCommandEvent: "
                  << "[" << pair.index << "] : " << pair.value.time.value << " : " << pair.value.value << " : "
                  << CommandStatusToString(pair.value.status) << std::endl;
    };

    values.ForeachItem(print);
}

void PrintingSOEHandler::Process(const HeaderInfo& /*info*/, const ICollection<Indexed<AnalogCommandEvent>>& values)
{
    auto print = [](const Indexed<AnalogCommandEvent>& pair) {
        std::cout << "AnalogCommandEvent: "
                  << "[" << pair.index << "] : " << pair.value.time.value << " : " << pair.value.value << " : "
                  << CommandStatusToString(pair.value.status) << std::endl;
    };

    values.ForeachItem(print);
}

void PrintingSOEHandler::Process(const HeaderInfo& /*info*/, const ICollection<Indexed<SecurityStat>>& values)
{
    auto print = [](const Indexed<SecurityStat>& pair) {
        std::cout << "SecurityStat: "
                  << "[" << pair.index << "] : " << pair.value.time.value << " : " << pair.value.value.count << " : "
                  << static_cast<int>(pair.value.quality) << " : " << pair.value.value.assocId << std::endl;
    };

    values.ForeachItem(print);
}

void PrintingSOEHandler::Process(const HeaderInfo& /*info*/, const ICollection<DNPTime>& values)
{
    auto print = [](const DNPTime& value) { std::cout << "DNPTime: " << value.value << std::endl; };

    values.ForeachItem(print);
}

} // namespace opendnp3
