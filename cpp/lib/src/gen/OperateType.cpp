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

#include "opendnp3/gen/OperateType.h"
#include <stdexcept>

namespace opendnp3 {

uint8_t OperateTypeToType(OperateType arg)
{
  return static_cast<uint8_t>(arg);
}
OperateType OperateTypeFromType(uint8_t arg)
{
  switch(arg)
  {
    case(0x0):
      return OperateType::SelectBeforeOperate;
    case(0x1):
      return OperateType::DirectOperate;
    case(0x2):
      return OperateType::DirectOperateNoAck;
    default:
      throw new std::invalid_argument("Unknown value");
  }
}
char const* OperateTypeToString(OperateType arg)
{
  switch(arg)
  {
    case(OperateType::SelectBeforeOperate):
      return "SelectBeforeOperate";
    case(OperateType::DirectOperate):
      return "DirectOperate";
    case(OperateType::DirectOperateNoAck):
      return "DirectOperateNoAck";
    default:
      return "UNDEFINED";
  }
}
OperateType OperateTypeFromString(const std::string& arg)
{
  if(arg == "SelectBeforeOperate") return OperateType::SelectBeforeOperate;
  if(arg == "DirectOperate") return OperateType::DirectOperate;
  if(arg == "DirectOperateNoAck") return OperateType::DirectOperateNoAck;
  else throw std::invalid_argument(std::string("Unknown value: ") + arg);
}

}
