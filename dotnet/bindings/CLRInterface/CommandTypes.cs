﻿
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This file was forked on 01/01/2013 by Automatak, LLC and modifications
// have been made to this file. Automatak, LLC licenses these modifications to
// you under the terms of the License.
//

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Automatak.DNP3.Interface
{   

    /// <summary>
    /// Aggregate result of command operation.  Check the Result before checking the status.
    /// Status is only valid when Result == RESPONSE_OK
    /// </summary>
    public struct CommandResponse
    {
        public CommandResponse(UserTaskResult result, CommandStatus status)
        {
            this.result = result;
            this.status = status;
        }

        public override string ToString() 
        {
            if (result == UserTaskResult.RESPONSE_OK) return "Response(" + status + ")";
            else return "Failure(" + result + ")";
        }

        public UserTaskResult Result
        {
            get { return result; }
        }

        public CommandStatus Status
        {
            get { return status; }
        }

        private readonly UserTaskResult result;
        private readonly CommandStatus status;
    }   

    /// <summary>
    /// Command request to control a relay output.
    /// 
    /// Has lots of possible modes. Consult the dnp3 standard for all valid combinations.
    /// </summary>
    public class ControlRelayOutputBlock
    {
        /// <summary>
        /// Primary constructor
        /// </summary>
        /// <param name="code">Enumeration that controls that behavior of the command</param>
        /// <param name="count">How many times to repeat the operation</param>
        /// <param name="onTime">How long to set the output active></param>
        /// <param name="offTime">How long to set the output inactive></param>
        public ControlRelayOutputBlock(ControlCode code, byte count, System.UInt16 onTime, System.UInt16 offTime)
        {
            this.code = code;
            this.count = count;
            this.onTime = onTime;
            this.offTime = offTime;
        }
    
        public readonly ControlCode code;
        public readonly byte count;
        public readonly System.UInt16 onTime;
        public readonly System.UInt16 offTime;
    }

    /// <summary>
    /// Base class for all AnalogOutputs
    /// </summary>
    /// <typeparam name="T">Type of value sent to the outstation</typeparam>
    public class AnalogOutput<T>
    {	
    	
	    public AnalogOutput(T value)
        {
            this.value = value;            
        }	   	    

	    public readonly T value;        
    }
   
    /// <summary>
    /// A set-point command request with short integer representation.
    /// The underlying DNP3 serialization is Group41, Variation 1
    /// </summary>
    public class AnalogOutputInt32 : AnalogOutput<System.Int32>
    {
        public AnalogOutputInt32(System.Int32 value) : base(value)
        {}
    }

    /// <summary>
    /// A set-point command request with short integer representation.
    /// The underlying DNP3 serialization is Group41, Variation 2
    /// </summary>
    public class AnalogOutputInt16 : AnalogOutput<System.Int16>
    {
        public AnalogOutputInt16(System.Int16 value)
            : base(value)
        { }
    }

    /// <summary>
    /// A set-point command request with single precision floating point representation.
    /// The underlying DNP3 serialization is Group41, Variation 3
    /// </summary>
    public class AnalogOutputFloat32 : AnalogOutput<float>
    {
        public AnalogOutputFloat32(float value)
            : base(value)
        { }
    }

    /// <summary>
    /// A set-point command request with double precision floating point representation.
    /// The underlying DNP3 serialization is Group41, Variation 4
    /// </summary>
    public class AnalogOutputDouble64 : AnalogOutput<double>
    {
        public AnalogOutputDouble64(System.Double value)
            : base(value)
        { }
    }

}
