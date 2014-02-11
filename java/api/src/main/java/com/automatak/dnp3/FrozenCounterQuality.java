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

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Quality enumeration for frozen counters.
 *
 * Created by jstone on 2/11/14.
 */
public enum FrozenCounterQuality implements QualityField {
    /**
     * Set when the data is "good", meaning that rest of the system can trust the value
     */
    ONLINE((byte) 0x01),

    /**
     * the quality all points get before we have established communication (or populated) the point in a remote database
     */
    RESTART((byte) 0x02),

    /**
     *  Communication has been lost with the source of the data (after establishing contact)
     */
    COMM_LOST((byte) 0x04),

    /**
     *   the value is being forced to a "fake" value somewhere in the system
     */
    REMOTE_FORCED_DATA((byte) 0x08),

    /**
     * the value is being forced to a "fake" value on the original device
     */
    LOCAL_FORCED_DATA((byte) 0x10),

    /**
     * used to indicate that the counter filled up and rolled over, cleared automatically after reading
     */
    ROLLOVER((byte) 0x20),

    /**
     * indicates an unusual change in value
     */
    DISCONTINUITY((byte) 0x40),


    RESERVED ((byte) 0x80);

    private final byte id;

    private FrozenCounterQuality(byte id)
    {
        this.id = id;
    }

    public static List<FrozenCounterQuality> getValuesInBitField(byte bitfield)
    {
        List<FrozenCounterQuality> list = new ArrayList<FrozenCounterQuality>();
        if((bitfield & ONLINE.toByte()) != 0)  list.add(ONLINE);
        if((bitfield & RESTART.toByte()) != 0)  list.add(RESTART);
        if((bitfield & COMM_LOST.toByte()) != 0)  list.add(COMM_LOST);
        if((bitfield & REMOTE_FORCED_DATA.toByte()) != 0) list.add(REMOTE_FORCED_DATA);
        if((bitfield & LOCAL_FORCED_DATA.toByte()) != 0) list.add(LOCAL_FORCED_DATA);
        if((bitfield & ROLLOVER.toByte()) != 0) list.add(ROLLOVER);
        if((bitfield & DISCONTINUITY.toByte()) != 0) list.add(DISCONTINUITY);
        if((bitfield & RESERVED.toByte()) != 0) list.add(RESERVED);
        return Collections.unmodifiableList(list);
    }

    /**
     * @return The underlying representation of the enum
     */
    @Override
    public byte toByte()
    {
        return id;
    }
}
