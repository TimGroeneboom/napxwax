/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "timecodercomponent.h"
#include "nap/logger.h"

#include <nap/core.h>
#include <entity.h>
#include <audio/service/audioservice.h>
#include <audio/node/outputnode.h>

RTTI_BEGIN_ENUM(nap::audio::ETimecodeContol)
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_2A, "serato_2a"),
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_2B, "serato_2b"),
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_CD, "serato_cd"),
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::TRACTOR_A, "traktor_a"),
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::TRACTOR_B, "traktor_b"),
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::MIXVIBES_V2, "mixvibes_v2"),
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::MIXVIBES_7INCH, "mixvibes_7inch"),
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::PIONEER_A, "pioneer_a"),
RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::PIONEER_B, "pioneer_b")
RTTI_END_ENUM

RTTI_BEGIN_CLASS(nap::audio::TimecoderComponent)
        RTTI_PROPERTY("Input", &nap::audio::TimecoderComponent::mInput, nap::rtti::EPropertyMetaData::Required)
        RTTI_PROPERTY("ChannelRouting", &nap::audio::TimecoderComponent::mChannelRouting, nap::rtti::EPropertyMetaData::Default)
        RTTI_PROPERTY("Control", &nap::audio::TimecoderComponent::mControl, nap::rtti::EPropertyMetaData::Default)
        RTTI_PROPERTY("ReferenceSpeed", &nap::audio::TimecoderComponent::mReferenceSpeed, nap::rtti::EPropertyMetaData::Default)
RTTI_END_CLASS

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::audio::TimecoderComponentInstance)
        RTTI_CONSTRUCTOR(nap::EntityInstance&, nap::Component &)
RTTI_END_CLASS

using namespace nap::audio;

namespace nap
{
    bool TimecoderComponent::hasInstance() const
    {
        return mInstance != nullptr;
    }

    TimecoderComponentInstance& TimecoderComponent::getInstance() const
    {
        assert(mInstance);
        return *mInstance;
    }


    TimecoderComponentInstance::~TimecoderComponentInstance()
    {
    }

    void TimecoderComponentInstance::onDestroy()
    {
        auto audioService = getEntityInstance()->getCore()->getService<AudioService>();
        auto& nodeManager = audioService->getNodeManager();

    }


    bool TimecoderComponentInstance::init(utility::ErrorState& errorState)
    {
        // acquire audio service and node manager
        auto audioService = getEntityInstance()->getCore()->getService<AudioService>();
        auto& nodeManager = audioService->getNodeManager();

        // acquire resources
        auto* resource = getComponent<TimecoderComponent>();
        resource->mInstance = this;
        auto& channelRouting = resource->mChannelRouting;
        mControl = resource->mControl;
        mReferenceSpeed = resource->mReferenceSpeed;

        if(!errorState.check(mInput->getChannelCount() == 2, "%s: Input must have 2 channels.", resource->mID.c_str()))
            return false;

        if(!errorState.check(channelRouting.size() == 2, "%s: Channel routing must have 2 channels.", resource->mID.c_str()))
            return false;

        // configure channel routing
        if (channelRouting.empty())
        {
            for (auto channel = 0; channel < mInput->getChannelCount(); ++channel)
                channelRouting.emplace_back(channel);
        }
        for (auto channel = 0; channel < channelRouting.size(); ++channel)
        {
            if (channelRouting[channel] >= mInput->getChannelCount())
            {
                errorState.fail("%s: Trying to route input channel that is out of bounds.", resource->mID.c_str());
                return false;
            }
        }

        //
        mTimecoderNode = nodeManager.makeSafe<TimecoderNode>(nodeManager, mReferenceSpeed, mControl);

        return true;
    }

    void TimecoderComponentInstance::update(double deltaTime)
    {
        mTimecoderNode->consumeTimeAndPitch(mTimecode, mPitch);
        mRelativeTime += mPitch * deltaTime;
    }

    void TimecoderComponentInstance::setControl(ETimecodeContol control)
    {
        if(mControl != control)
        {
            mControl = control;
            mTimecoderNode->changeControl(mControl);
        }
    }


    void TimecoderComponentInstance::setReferenceSpeed(float referenceSpeed)
    {
        if(mReferenceSpeed != referenceSpeed)
        {
            mReferenceSpeed = referenceSpeed;
            mTimecoderNode->changeReferenceSpeed(mReferenceSpeed);
        }
    }
}