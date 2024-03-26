#pragma once

#include <audio/core/audionode.h>
#include <audio/service/audioservice.h>
#include "audio/utility/dirtyflag.h"
#include <concurrentqueue.h>

namespace nap
{
namespace audio
{
    enum NAPAPI ETimecodeContol
    {
        SERATO_2A,
        SERATO_2B,
        SERATO_CD,
        TRACTOR_A,
        TRACTOR_B,
        MIXVIBES_V2,
        MIXVIBES_7INCH,
        PIONEER_A,
        PIONEER_B
    };

    /**
     *
     */
    class NAPAPI TimecoderNode final : public audio::Node
    {
        friend class TimecoderAudioObject;
        friend class TimecoderComponentInstance;
    RTTI_ENABLE(audio::Process)
    public:
        TimecoderNode(NodeManager& nodeManager);

        /**
         * Constructor
         * @param nodeManager reference to nodeManager
         */
        TimecoderNode(NodeManager& nodeManager, float referenceSpeed, ETimecodeContol control);

        /**
         * Destructor
         */
        ~TimecoderNode() override;

        /**
         * Pulls buffers from left and right input pins,
         * Converts samples to 16 bit PCM and submits them to the timecoder.
         * Computes pitch and time and sets the dirty flag.
         */
        void process() override;

        /**
         * Reinits the timecoder with the new sample rate.
         * @param sampleRate the new samplerate
         */
        void sampleRateChanged(float sampleRate) override;

        /**
         * Gets the current pitch. Thread-safe.
         * @return current pitch
         */
        float getPitch();
    private:
        // TimecoderComponentInstance interface methods and members

        /**
         * Gets time in seconds and pitch when the dirty flag is set, returns true if the dirty flag was set, false otherwise.
         * @param time will be set to current time in seconds
         * @param pitch will be set to current pitch
         * @return true if the dirty flag was set, false otherwise. Given values will only be updated when dirty flag was set.
         */
        bool consumeTimeAndPitch(double &time, double &pitch);

        /**
         * Change control, re-initializes the timecoder.
         * @param control control to change to
         */
        void changeControl(ETimecodeContol control);

        /**
         * Change reference speed, re-initializes the timecoder.
         * @param referenceSpeed the new reference speed
         */
        void changeReferenceSpeed(float referenceSpeed);

        // these input pins are connected by the TimecoderComponentInstance init method
        InputPin audioLeft  = {this};
        InputPin audioRight = {this};
    private:
        /**
         * Implementation in .cpp file
         * Implementation wraps the timecoder struct from xwax
         */
        class Impl;
        std::unique_ptr<Impl> mImpl;
        double pos = 0.0;

        short mSamples[2] = { 0, 0 };
        SampleBuffer* mBuffers[2] = {nullptr, nullptr};
        std::atomic<double> mTime{0.0};
        std::atomic<double> mPitch{0.0};
        DirtyFlag mDirty;

        float mReferenceSpeed = 1.0f;
        ETimecodeContol mControl;

        /**
         * Creates a new timecoder, creation will be queued and executed in the process method.
         */
        void createTimecoder();
        moodycamel::ConcurrentQueue<std::function<void()>> mCreationQueue;
    };
}
}