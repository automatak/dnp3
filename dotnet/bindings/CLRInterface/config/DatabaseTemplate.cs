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
    /// Point record type that is assigned an event class 
    /// </summary>
    public class EventPointRecord
    {
        public EventPointRecord(PointClass pointClass, System.Byte quality)
        {            
            this.pointClass = pointClass;
            this.quality = quality;
        }

        public EventPointRecord(PointClass pointClass) : this(pointClass, 0x02)
        {

        }

        public EventPointRecord() : this(PointClass.Class1)
        { 
        
        }

        public PointClass pointClass;
        public System.Byte quality;
    };

    /// <summary>
    /// Point record type that is assigned an event class and deadband tolerance
    /// </summary>
    public class DeadbandEventPointRecord<T> : EventPointRecord
    {
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="pointClass">Class that will be evented when the value changes</param>
        /// <param name="deadband">Value can change within this tolerance without producing an event</param>
        public DeadbandEventPointRecord(PointClass pointClass, T deadband) : base(pointClass)
        {
            this.deadband = deadband;
        }

        public DeadbandEventPointRecord(PointClass pointClass)
            : base(pointClass)
        {
            this.deadband = default(T);
        }

        /// <summary>
        /// Default constructor with Class 0 and 0.1 tolerance
        /// </summary>
        public DeadbandEventPointRecord(): base()
        {
            this.deadband = default(T);
        }

        /// <summary>
        /// Value can change within this tolerance without producing an event
        /// </summary>
        public T deadband;
    };    

    /// <summary>
    /// Defines the database layout for an outstation
    /// </summary>
    public class DatabaseTemplate
    {
        /// <summary>
        /// Constructor that sets up the size of the types
        /// </summary>
        /// <param name="numBinary">numer of binary values starting at index 0</param>
        /// <param name="numAnalog">numer of analog values starting at index 0</param>
        /// <param name="numCounter">numer of counter values starting at index 0</param>
        /// <param name="numCounter">numer of frozen counter values starting at index 0</param>
        /// <param name="numBinaryOutputStatus">numer of control status values starting at index 0</param>
        /// <param name="numAnalogOutputStatus">numer of setpoint status values starting at index 0</param>
        /// /// <param name="numTimeAndInterval">numer of TimeAndInterval values starting at index 0</param>
        public DatabaseTemplate(  System.UInt16 numBinary,
                                  System.UInt16 numDoubleBinary,
                                  System.UInt16 numAnalog,
                                  System.UInt16 numCounter,
                                  System.UInt16 numFrozenCounter,
                                  System.UInt16 numBinaryOutputStatus,
                                  System.UInt16 numAnalogOutputStatus,
                                  System.UInt16 numTimeAndInterval)
        {
            binaries = Enumerable.Range(0, numBinary).Select(i => new EventPointRecord(PointClass.Class1)).ToList();

            doubleBinaries = Enumerable.Range(0, numDoubleBinary).Select(i => new EventPointRecord(PointClass.Class1)).ToList();
            
            counters = Enumerable.Range(0, numCounter).Select(i => new DeadbandEventPointRecord<System.UInt32>(PointClass.Class1, 0)).ToList();
            
            frozenCounters = Enumerable.Range(0, numFrozenCounter).Select(i => new DeadbandEventPointRecord<System.UInt32>(PointClass.Class1, 0)).ToList();
            
            analogs = Enumerable.Range(0, numAnalog).Select(i => new DeadbandEventPointRecord<double>(PointClass.Class1, 0.0)).ToList();
            
            binaryOutputStatii = Enumerable.Range(0, numBinaryOutputStatus).Select(i => new EventPointRecord(PointClass.Class1)).ToList();
            
            analogOutputStatii = Enumerable.Range(0, numAnalogOutputStatus).Select(i => new DeadbandEventPointRecord<double>(PointClass.Class1, 0.0)).ToList();

            this.numTimeAndInterval = numTimeAndInterval;
        }

        /// <summary>
        /// Default constructor that sets every value to the same count
        /// </summary>
        public DatabaseTemplate(System.UInt16 count) : this(count, count, count, count, count, count, count, count)
        { }

        /// <summary>
        /// Default constructor that sets up 10 of every type
        /// </summary>
        public DatabaseTemplate() : this(10)
        { }

        /// <summary>
        /// Modify individual binary configuration here
        /// </summary>
        public IList<EventPointRecord> binaries;
        /// <summary>
        /// Modify individual double binary configuration here
        /// </summary>
        public IList<EventPointRecord> doubleBinaries;
        /// <summary>
        /// Modify individual analog configuration here
        /// </summary>
        public IList<DeadbandEventPointRecord<System.UInt32>> counters;
        /// <summary>
        /// Modify individual analog configuration here
        /// </summary>
        public IList<DeadbandEventPointRecord<System.UInt32>> frozenCounters;
        /// <summary>
        /// Modify individual counter configuration here
        /// </summary>
        public IList<DeadbandEventPointRecord<double>> analogs;
        /// <summary>
        /// Modify individual binary output status configuration here
        /// </summary>
        public IList<EventPointRecord> binaryOutputStatii;
        /// <summary>
        /// Modify individual analog output status configuration here
        /// </summary>
        public IList<DeadbandEventPointRecord<double>> analogOutputStatii;

        public readonly UInt16 numTimeAndInterval;
    };    
}
