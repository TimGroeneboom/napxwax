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

        class NAPAPI TimecoderComponent : public AudioComponentBase
        {
        RTTI_ENABLE(AudioComponentBase)
        DECLARE_COMPONENT(TimecoderComponent, TimecoderComponentInstance)
        public:
            // Properties
            nap::ComponentPtr<audio::AudioComponentBase> mInput; ///< property: 'Input' The component whose audio output will be send
            std::vector<int> mChannelRouting; ///< property: 'ChannelRouting' The component whose audio output will be send

        };

        class NAPAPI TimecoderComponentInstance : public AudioComponentBaseInstance
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

            void update(double deltaTime) override;

            double getRelativeTime() const { return mRelativeTime; }
        private:
            ComponentInstancePtr<audio::AudioComponentBase> mInput	= {this, &TimecoderComponent::mInput};
            audio::SafeOwner<audio::TimecoderNode> mTimecoderNode = nullptr;

            double mPitch = 0.0;
            double mTime = 0.0;
            double mRelativeTime = 0.0;
            SteadyTimer mTimer;
        };
    }
}
