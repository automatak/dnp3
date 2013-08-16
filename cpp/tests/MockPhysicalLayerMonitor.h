
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

#include <boost/test/unit_test.hpp>

#include <opendnp3/PhysicalLayerMonitor.h>
#include <opendnp3/CopyableBuffer.h>

#include <queue>

namespace opendnp3
{

class MockPhysicalLayerMonitor : public PhysicalLayerMonitor
{
public:
	MockPhysicalLayerMonitor(
	        Logger* apLogger,
	        IPhysicalLayerAsync* apPhys,
	        timer_clock::duration aMinOpenRetry,
	        timer_clock::duration aMaxOpenRetry
	);

	size_t mOpens;
	size_t mCloses;

	size_t mNumReads;
	size_t mBytesRead;
	size_t mBytesWritten;
	size_t mLastWriteSize;

	CopyableBuffer mReadBuffer;
	CopyableBuffer mWriteBuffer;
	CopyableBuffer mExpectReadBuffer;

	void OnPhysicalLayerOpenSuccessCallback();
	void OnPhysicalLayerOpenFailureCallback() {}
	void OnPhysicalLayerCloseCallback();

	void _OnReceive(const uint8_t* apData, size_t aNumBytes);
	void _OnSendSuccess(void);
	void _OnSendFailure(void);

	void WriteData(const CopyableBuffer& arData);
	void ExpectData(const CopyableBuffer& arData);

	bool NextStateIs(ChannelState aState);
	bool AllExpectedDataHasBeenReceived();

protected:
	void OnStateChange(ChannelState);
	std::queue< ChannelState > mState;

private:

	void TransmitNext();

};

}
