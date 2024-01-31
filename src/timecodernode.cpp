/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <timecodernode.h>
#include <audio/core/audionode.h>
#include <audio/core/audionodemanager.h>
#include <audio/core/audiopin.h>

#include "timecoder_wrapper.h"
#include "nap/logger.h"

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::audio::TimecoderNode)
RTTI_END_CLASS

namespace nap
{
    namespace audio
    {
        class TimecoderNode::Impl
        {
        public:
            Impl() = default;
            ~Impl()
            {
                if(inited)
                {
                    timecoder_clear(&tc);
                }
            }

            struct timecoder tc;
            struct timecode_def *def;
            bool inited = false;
        };


        TimecoderNode::TimecoderNode(NodeManager& nodeManager) : Node(nodeManager)
        {
            mImpl = std::make_unique<Impl>();
            mImpl->def = timecoder_find_definition("serato_2a");
            assert(mImpl->def != nullptr);

            timecoder_init(&mImpl->tc, mImpl->def, 1.0, static_cast<int>(nodeManager.getSampleRate()), false);
            mImpl->inited = true;
        }


        TimecoderNode::~TimecoderNode() = default;


        void TimecoderNode::process()
        {
            assert(audioLeft.isConnected());
            assert(audioRight.isConnected());

            mBuffers[0] = audioLeft.pull();
            mBuffers[1] = audioRight.pull();

            for (auto s = 0; s < getBufferSize(); ++s)
            {
                mSamples[0] = static_cast<short>(mBuffers[0]->at(s) * (1<<15));
                mSamples[1] = static_cast<short>(mBuffers[1]->at(s) * (1<<15));
                timecoder_submit(&mImpl->tc, mSamples, 1);
            }

            mPitch.store(timecoder_get_pitch(&mImpl->tc));
            mTime.store(static_cast<double>(timecoder_get_position(&mImpl->tc, &pos)) / 1000);
            mDirty.set();
        }


        bool TimecoderNode::consumeTimeAndPitch(double &time, double &pitch)
        {
            if(mDirty.check())
            {
                time = mTime.load();
                pitch = mPitch.load();
                return true;
            }
            return false;
        }


        void TimecoderNode::sampleRateChanged(float sampleRate)
        {
            mImpl = std::make_unique<Impl>();
            mImpl->def = timecoder_find_definition("serato_cd");
            timecoder_init(&mImpl->tc, mImpl->def, 1.0, static_cast<int>(getNodeManager().getSampleRate()), false);
            mImpl->inited = true;
        }
    }

}
