/**
 * Copyright 2013 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
package com.automatak.dnp3;

import java.util.LinkedList;
import java.util.List;

/**
 * Configuration class for a master station
 */
public class MasterConfig {
    /**
     * Constructor with reasonable defaults
     */
    public MasterConfig()
    {

    }

    /**
     * Maximum fragment size to use for requests
     */
    public int maxRequestFragmentSize = 2048;

    /**
     * The number of objects to store in the VtoWriter queue.
     */
    public int vtoWriterQueueSize = 1024;

    /**
     * Using FC_WRITE is a problem with vto because the spec won't allow it to retry
     */
    public boolean useNonStandardVtoFunction = false;

    /**
     * If true, the master will do time syncs when it sees the time IIN bit from the slave
     */
    public boolean allowTimeSync = true;

    /**
     * If true, the master will enable/disable unsol on startup
     */
    public boolean doUnsolOnStartup = true;

    /**
     * If DoUnsolOnStartup == true, the master will use this bit to decide wether to enable (true) or disable (false)
     */
    public boolean enableUnsol = false;

    /**
     * Bitwise mask used determine which classes are enabled/disabled for unsol
     */
    public int unsolClassMask = PointClass.ALL_EVENTS.toInt();

    /**
     * Period for integrity scans (class 0), -1 for non periodic
     */
    public long integrityRateMs = 5000;

    /**
     * Time delay between task retries
     */
    public long taskRetryRateMs = 5000;

    /**
     * vector that holds exception scans, defaults to empty list
     */
    public final List<ExceptionScan> scans = new LinkedList<ExceptionScan>();
}
