#pragma once

#include <audio/core/audionode.h>
#include <audio/service/audioservice.h>
#include "audio/utility/dirtyflag.h"

namespace nap
{
namespace audio
{
    class NAPAPI TimecoderNode final : public audio::Node
    {
    RTTI_ENABLE(audio::Process)
    public:
        TimecoderNode(NodeManager& nodeManager);
        ~TimecoderNode() override;

        void process() override;

        InputPin audioLeft  = {this};
        InputPin audioRight = {this};

        void sampleRateChanged(float sampleRate) override;

        /**
         * Sets time in seconds and pitch when the dirty flag is set, returns true if the dirty flag was set, false otherwise.
         * @param time will be set to current time in seconds
         * @param pitch will be set to current pitch
         * @return true if the dirty flag was set, false otherwise. Given values will only be updated when dirty flag was set.
         */
        bool consumeTimeAndPitch(double &time, double &pitch);
    private:
        class Impl;
        std::unique_ptr<Impl> mImpl;
        double pos = 0.0;

        short mSamples[2] = { 0, 0 };
        SampleBuffer* mBuffers[2] = {nullptr, nullptr};
        std::atomic<double> mTime{0.0};
        std::atomic<double> mPitch{0.0};
        DirtyFlag mDirty;
    };
}
}