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
        static std::unordered_map<ETimecodeContol, const char*> timecoderLookup =
        {
            { ETimecodeContol::SERATO_2A,       "serato_2a"     },
            { ETimecodeContol::SERATO_2B,       "serato_2b"     },
            { ETimecodeContol::SERATO_CD,       "serato_cd"     },
            { ETimecodeContol::TRACTOR_A,       "traktor_a"     },
            { ETimecodeContol::TRACTOR_B,       "traktor_b"     },
            { ETimecodeContol::MIXVIBES_V2,     "mixvibes_v2"   },
            { ETimecodeContol::MIXVIBES_7INCH,  "mixvibes_7inch"},
            { ETimecodeContol::PIONEER_A,       "pioneer_a"     },
            { ETimecodeContol::PIONEER_B,       "pioneer_b"     }
        };


        class TimecoderNode::Impl
        {
        public:
            Impl(ETimecodeContol control, float referenceSpeed, int sampleRate)
            {
                assert(timecoderLookup.find(control) != timecoderLookup.end());
                auto* definition = timecoder_find_definition(timecoderLookup[control]);
                assert(definition != nullptr);
                timecoder_init(&mTimeCoder, definition, referenceSpeed, sampleRate, false);
            }

            virtual ~Impl() = default;

            timecoder mTimeCoder;
        };


        TimecoderNode::TimecoderNode(NodeManager& nodeManager) : Node(nodeManager)
        {
            createTimecoder();
        }


        TimecoderNode::TimecoderNode(NodeManager& nodeManager,
                                     float referenceSpeed,
                                     ETimecodeContol control) : Node(nodeManager)
        {
            mReferenceSpeed = referenceSpeed;
            mControl = control;
            createTimecoder();
        }


        TimecoderNode::~TimecoderNode() = default;


        void TimecoderNode::createTimecoder()
        {
            // first dequeue any previous creation tasks, just in case
            while(mCreationQueue.size_approx() > 0)
            {
                std::function<void()> task;
                while(mCreationQueue.try_dequeue(task)){}
            }

            // move the creation to the task queue
            auto control = mControl;
            int sample_rate = static_cast<int>(getNodeManager().getSampleRate());
            auto reference_speed = mReferenceSpeed;
            mCreationQueue.enqueue([this, control, sample_rate, reference_speed]()
            {
                mImpl = std::make_unique<Impl>(control, reference_speed, sample_rate);
            });
        }


        void TimecoderNode::process()
        {
            std::function<void()> task;
            while(mCreationQueue.try_dequeue(task))
                task();

            assert(mImpl != nullptr);
            assert(audioLeft.isConnected());
            assert(audioRight.isConnected());

            mBuffers[0] = audioLeft.pull();
            mBuffers[1] = audioRight.pull();

            for (auto s = 0; s < getBufferSize(); ++s)
            {
                mSamples[0] = static_cast<short>(mBuffers[0]->at(s) * (1<<15));
                mSamples[1] = static_cast<short>(mBuffers[1]->at(s) * (1<<15));
                timecoder_submit(&mImpl->mTimeCoder, mSamples, 1);
            }

            mPitch.store(timecoder_get_pitch(&mImpl->mTimeCoder));
            mTime.store(static_cast<double>(timecoder_get_position(&mImpl->mTimeCoder, &pos)) / 1000);
            mDirty.set();
        }


        float TimecoderNode::getPitch()
        {
            return mPitch.load();
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


        void TimecoderNode::changeControl(ETimecodeContol control)
        {
            if(mControl != control)
            {
                mControl = control;
                createTimecoder();
            }
        }


        void TimecoderNode::changeReferenceSpeed(float referenceSpeed)
        {
            if(mReferenceSpeed != referenceSpeed)
            {
                mReferenceSpeed = referenceSpeed;
                createTimecoder();
            }
        }


        void TimecoderNode::sampleRateChanged(float sampleRate)
        {
            createTimecoder();
        }
    }

}
