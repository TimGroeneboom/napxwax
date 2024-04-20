#pragma once

#include <audio/core/audionode.h>
#include <audio/service/audioservice.h>
#include "audio/utility/dirtyflag.h"
#include <concurrentqueue.h>

namespace nap
{
namespace audio
{
    /**
     * Enum for the different timecode control signals
     */
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
     * Timecoder node performs timecode analysis on node input.
     * Input must be stereo audio.
     * Output just passes through the input. This allows for buffer players to be controlled by analysis of the timecoder.
     * Use the consumeTimeAndPitch method to get the current time and pitch from the main thread
     * Use the getPitch and getTime methods to get the current pitch and time from the audio thread
     */
    class NAPAPI TimecoderNode final : public audio::Node
    {
        friend class TimecoderComponentInstance;
    RTTI_ENABLE(audio::Process)
    public:
        /**
         * Constructor
         * @param nodeManager reference to nodeManager
         */
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
         * Sets time in seconds and pitch when the dirty flag is set, returns true if the dirty flag was set, false otherwise.
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

        /**
         * Returns the current pitch, should only be called from audio thread, so either a node or another process attached to the audio thread
         * @return the current pitch
         */
        double getPitch() const { return mPitch; }

        /**
         * Returns the current time in seconds, should only be called from audio thread, so either a node or another process attached to the audio thread
         * @return the current time in seconds
         */
        double getTime() const { return mTime; }

        /**
         * Returns the reference speed, should only be called from audio thread, so either a node or another process attached to the audio thread
         * @return the reference speed
         */
        float getReferenceSpeed() const { return mReferenceSpeed; }

        // these input pins are connected by the TimecoderComponentInstance init method
        InputPin audioLeft = { this };
        InputPin audioRight = { this };

        OutputPin audioOutputRight = { this };
        OutputPin audioOutputLeft = { this };
    private:
        /**
         * Implementation in .cpp file
         * Implementation wraps the timecoder struct from xwax
         */
        class Impl;
        std::unique_ptr<Impl> mImpl;

        double mPosition = 0.0;

        short mSamples[2] = { 0, 0 };
        SampleBuffer* mBuffers[2] = {nullptr, nullptr};
        std::atomic<double> mTime{0.0};
        std::atomic<double> mPitch{0.0};
        DirtyFlag mDirty;

        // accessed only from update / main thread
        double mConsumedPitch = 0.0f;
        double mConsumedTime = 0.0;

        float mReferenceSpeed = 1.0f;
        ETimecodeContol mControl;

        /**
         * Creates a new timecoder, creation will be queued and executed in the process method.
         */
        void createTimecoder();
        moodycamel::ConcurrentQueue<std::function<void()>> mTaskQueue;
    };
}
}