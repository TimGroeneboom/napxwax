/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

// Nap includes
#include <nap/resourceptr.h>
#include <audio/utility/safeptr.h>

// Audio includes
#include <audio/component/audiocomponentbase.h>
#include <audio/resource/audiobufferresource.h>
#include <audio/node/inputnode.h>

#include "timecodernode.h"
#include "nap/timer.h"

namespace nap
{
    namespace audio
    {
        // Forward declares
        class TimecoderComponentInstance;

        class NAPAPI TimecoderComponent final : public AudioComponentBase
        {
            friend class TimecoderComponentInstance;
        RTTI_ENABLE(AudioComponentBase)
        DECLARE_COMPONENT(TimecoderComponent, TimecoderComponentInstance)
        public:
            // Properties
            nap::ComponentPtr<audio::AudioComponentBase> mInput; ///< property: 'Input' The component whose audio output will be send
            std::vector<int> mChannelRouting; ///< property: 'ChannelRouting' The component whose audio output will be send
            float mReferenceSpeed = 1.0f; ///< Property: 'ReferenceSpeed' The reference speed of the timecoder
            ETimecodeContol mControl = ETimecodeContol::SERATO_2A; ///< Property: 'Control' The control mode of the timecoder

            /**
             * Returns true if instance is set
             * @return true if instance is set
             */
            bool hasInstance() const;

            /**
             * Returns reference to instance, check with hasInstance() first. Asserts if instance is not set
             * @return reference to instance
             */
            TimecoderComponentInstance& getInstance() const;
        private:
            TimecoderComponentInstance* mInstance = nullptr;
        };

        class NAPAPI TimecoderComponentInstance final : public AudioComponentBaseInstance
        {
        RTTI_ENABLE(AudioComponentBaseInstance)
        public:
            /**
             * Constructor
             * @param entity entity
             * @param resource resource
             */
            TimecoderComponentInstance(EntityInstance& entity, Component& resource)
                : AudioComponentBaseInstance(entity, resource)
            {
            }
            ~TimecoderComponentInstance() override;

            /**
             * Initializes the instance, returns false on failure
             * @param errorState contains any error messages
             * @return false on failure
             */
            bool init(utility::ErrorState& errorState) override;

            /**
             * Called before deconstruction
             * Removes listener from VBANPacketReceiver
             */
            void onDestroy() override;

            /**
             * Returns amount of channels
             * @return amount of channels
             */
            int getChannelCount() const override { return mInput->getChannelCount(); }

            /**
             * Returns output pin for given channel, no bound checking, assert on out of bound
             * @param channel the channel
             * @return OutputPin for channel
             */
            OutputPin* getOutputForChannel(int channel) override { return mInput->getOutputForChannel(channel); }

            /**
             * Updates the time and pitch with current timecode and pitch computed by TimecoderNode
             * @param deltaTime time in seconds since last update
             */
            void update(double deltaTime) override;

            /**
             * Returns relative time, relative time is computed by pitch and delta time in update
             * You can reset the relative time by calling setRelativeTime
             * @return relative time
             */
            double getRelativeTime() const { return mRelativeTime; }

            /**
             * Sets relative time
             * @param time the new relative time
             */
            void setRelativeTime(double time) { mRelativeTime = time; }

            /**
             * Returns the pitch computed by the TimecoderNode
             * @return pitch
             */
            double getPitch() const{ return mPitch; }

            /**
             * Returns the absolute timecode in seconds computed by the TimecoderNode, will be -1.0 if no timecode is available or invalid
             * @return timecode
             */
            double getTimecode() const{ return mTimecode;}

            /**
             * Sets the control mode of the timecoder
             * @param control the control mode
             */
            void setControl(ETimecodeContol control);

            /**
             * Returns current control mode
             * @return current control mode
             */
            ETimecodeContol getControl() const { return mControl; }

            /**
             * Sets the reference speed of the timecoder, 1.0 is 33 1/3, 1.35 is 45 rpm
             * @param referenceSpeed
             */
            void setReferenceSpeed(float referenceSpeed);

            /**
             * Returns the reference speed of the timecoder
             * @return the reference speed of the timecoder
             */
            float getReferenceSpeed() const { return mReferenceSpeed; }

            /**
             * Returns reference to the TimecoderNode
             * @return reference to the TimecoderNode
             */
            audio::TimecoderNode& getTimecoderNode() { return *mTimecoderNode; }
        private:
            ComponentInstancePtr<audio::AudioComponentBase> mInput	= { this, &TimecoderComponent::mInput };
            audio::SafeOwner<audio::TimecoderNode> mTimecoderNode = nullptr;

            double mPitch = 0.0;
            double mTimecode = 0.0;
            double mRelativeTime = 0.0;
            float mReferenceSpeed = 1.0f;
            ETimecodeContol mControl = ETimecodeContol::SERATO_2A;
        };
    }
}
