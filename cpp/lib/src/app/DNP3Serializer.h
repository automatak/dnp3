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
#ifndef OPENDNP3_DNP3SERIALIZER_H
#define OPENDNP3_DNP3SERIALIZER_H

#include "opendnp3/app/GroupVariationID.h"

#include <ser4cpp/serialization/Serializer.h>

namespace opendnp3
{

template<class T> class DNP3Serializer : public ser4cpp::Serializer<T>
{
public:
    DNP3Serializer(GroupVariationID id_,
                   uint32_t size_,
                   typename ser4cpp::Serializer<T>::ReadFunc pReadFunc_,
                   typename ser4cpp::Serializer<T>::WriteFunc pWriteFunc_)
        : ser4cpp::Serializer<T>(size_, pReadFunc_, pWriteFunc_), id(id_)
    {
    }

    GroupVariationID ID() const
    {
        return id;
    }

private:
    GroupVariationID id;
};

} // namespace opendnp3

#endif
