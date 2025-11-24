/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "timecodercomponent.h"
#include "nap/logger.h"

#include <nap/core.h>
#include <entity.h>
#include <audio/service/audioservice.h>
#include <audio/node/outputnode.h>
#include <unordered_set>
#include <mathutils.h>

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
        RTTI_PROPERTY("Input",				&nap::audio::TimecoderComponent::mInput,			nap::rtti::EPropertyMetaData::Required)
        RTTI_PROPERTY("ChannelRouting",		&nap::audio::TimecoderComponent::mChannelRouting,	nap::rtti::EPropertyMetaData::Default)
        RTTI_PROPERTY("Control",			&nap::audio::TimecoderComponent::mControl,			nap::rtti::EPropertyMetaData::Default)
		RTTI_PROPERTY("Mode",				&nap::audio::TimecoderComponent::mMode,				nap::rtti::EPropertyMetaData::Default)
        RTTI_PROPERTY("ReferenceSpeed",		&nap::audio::TimecoderComponent::mReferenceSpeed,	nap::rtti::EPropertyMetaData::Default)
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
		// Ensure input is at least stereo
		auto* resource = getComponent<TimecoderComponent>();
		if (!errorState.check(mInput->getChannelCount() >= 2,
			"Input must be stereo and have 2 channels, got %d instead", mInput->getChannelCount()))
			return false;

		// Copy required routing, stereo if none provided
		mChannelRouting = resource->mChannelRouting.empty() ?
			std::vector<int>({0,1}) : resource->mChannelRouting;

		// Ensure routing is stereo
		if (!errorState.check(mChannelRouting.size() == 2,
			"Invalid routing, must be stereo and have 2 channels, got %d instead", mChannelRouting.size()))
			return false;

		// Ensure routing is valid, either 0-1 or 1-0
		std::unordered_set<int> unique(2);
		for (const auto& channel : mChannelRouting)
		{
			if (!errorState.check(unique.find(channel) == unique.end(),
				"Duplicate channel assignment, %d", channel))
				return false;

			if (!errorState.check(channel < mInput->getChannelCount(),
				"Channel %d exceeds input channel count of %d", channel, mInput->getChannelCount()))
				return false;

			unique.emplace(channel);
		}

		// Create DSP
		createGraph();

		// Initialize
		setMode(resource->mMode);
		setControl(resource->mControl);
		setReferenceSpeed(resource->mReferenceSpeed);

        return true;
    }


	void audio::TimecoderComponentInstance::createGraph()
	{
		// acquire audio service and node manager
		auto audio_service = getEntityInstance()->getCore()->getService<AudioService>();
		auto& node_manager = audio_service->getNodeManager();

		// Create xwax time decode node
		mTimecoderNode = node_manager.makeSafe<TimecoderNode>(node_manager, mReferenceSpeed, mControl);

		// Connect xwax to audio input
		assert(mChannelRouting.size() == 2);
		mTimecoderNode->audioLeft.connect(*mInput->getOutputForChannel(mChannelRouting[0]));
		mTimecoderNode->audioRight.connect(*mInput->getOutputForChannel(mChannelRouting[1]));

		// Create gain for each xwax output and connect
		for (auto i = 0; i < mChannelRouting.size(); i++)
		{
			// Create gain node and connect input
			auto gain = node_manager.makeSafe<GainNode>(node_manager);
			gain->audioInput.connect(i == 0 ?
				mTimecoderNode->audioOutputLeft :
				mTimecoderNode->audioOutputRight);

			// Store
			mGainNodes[i] = std::move(gain);
		}
	}


    void TimecoderComponentInstance::update(double deltaTime)
    {
        bool timecode_valid = false;
        mTimecoderNode->consumeTimeAndPitch(mTimecode, mPitch, timecode_valid);
        mRelativeTime += mPitch * deltaTime;
    }


    void TimecoderComponentInstance::setControl(ETimecodeContol control)
    {
		mTimecoderNode->changeControl(control);
	}


    void TimecoderComponentInstance::setReferenceSpeed(float referenceSpeed)
    {
		mTimecoderNode->changeReferenceSpeed(mReferenceSpeed);
    }


	nap::audio::OutputPin* audio::TimecoderComponentInstance::getOutputForChannel(int channel)
	{
		assert(channel < mChannelRouting.size());
		return &mGainNodes[channel]->audioOutput;
	}


	void audio::TimecoderComponentInstance::setMode(ETimecodeMode mode)
	{
		switch (mMode)
		{
			case ETimecodeMode::DVS:
			{
				mGainNodes[0]->setGain(0.0f, 1);
				mGainNodes[1]->setGain(0.0f, 1);
				break;
			}
			case ETimecodeMode::PassThrough:
			{
				mGainNodes[0]->setGain(1.0f, 1);
				mGainNodes[1]->setGain(1.0f, 1);
				break;
			}
			default:
			{
				assert(false);
				break;
			}
		}
	}
}
