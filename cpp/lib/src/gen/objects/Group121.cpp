//
//  _   _         ______    _ _ _   _             _ _ _
// | \ | |       |  ____|  | (_) | (_)           | | | |
// |  \| | ___   | |__   __| |_| |_ _ _ __   __ _| | | |
// | . ` |/ _ \  |  __| / _` | | __| | '_ \ / _` | | | |
// | |\  | (_) | | |___| (_| | | |_| | | | | (_| |_|_|_|
// |_| \_|\___/  |______\__,_|_|\__|_|_| |_|\__, (_|_|_)
//                                           __/ |
//                                          |___/
// 
// This file is auto-generated. Do not edit manually
// 
// Copyright 2013-2019 Automatak, LLC
// 
// Licensed to Green Energy Corp (www.greenenergycorp.com) and Automatak
// LLC (www.automatak.com) under one or more contributor license agreements.
// See the NOTICE file distributed with this work for additional information
// regarding copyright ownership. Green Energy Corp and Automatak LLC license
// this file to you under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may obtain
// a copy of the License at:
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "Group121.h"

#include <ser4cpp/serialization/LittleEndian.h>
#include "app/MeasurementFactory.h"
#include "app/WriteConversions.h"

using namespace ser4cpp;

namespace opendnp3 {

// ------- Group121Var1 -------

Group121Var1::Group121Var1() : flags(0), assocId(0), value(0)
{}

bool Group121Var1::Read(rseq_t& buffer, Group121Var1& output)
{
  bool result = true;

  result &= UInt8::read_from(buffer, output.flags);
  result &= UInt16::read_from(buffer, output.assocId);
  result &= UInt32::read_from(buffer, output.value);

  return result;
}

bool Group121Var1::Write(const Group121Var1& arg, ser4cpp::wseq_t& buffer)
{
  bool result = true;

  result &= UInt8::write_to(buffer, arg.flags);
  result &= UInt16::write_to(buffer, arg.assocId);
  result &= UInt32::write_to(buffer, arg.value);

  return result;
}

bool Group121Var1::ReadTarget(rseq_t& buff, SecurityStat& output)
{
  Group121Var1 value;
  if(Read(buff, value))
  {
    output = SecurityStatFactory::From(value.flags, value.assocId, value.value);
    return true;
  }
  else
  {
    return false;
  }
}

bool Group121Var1::WriteTarget(const SecurityStat& value, ser4cpp::wseq_t& buff)
{
  return Group121Var1::Write(ConvertGroup121Var1::Apply(value), buff);
}


}
