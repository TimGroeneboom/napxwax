#include "audiocontrolwindow.h"
#include "appguiservice.h"
#include "audio/component/playbackcomponent.h"
#include "imgui.h"
#include "audiodevicesettingsgui.h"
#include "audio/service/audioservice.h"
#include "imguiservice.h"
#include <nap/core.h>

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::AudioControlWindow)
    RTTI_CONSTRUCTOR(nap::AppGUIService&)
    RTTI_PROPERTY("Scene", &nap::AudioControlWindow::mScene, nap::rtti::EPropertyMetaData::Required)
    RTTI_PROPERTY("PlaybackEntity", &nap::AudioControlWindow::mPlaybackEntity, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

namespace nap
{
    AudioControlWindow::AudioControlWindow(AppGUIService& guiService)
        : AppGUIWindow(guiService)
    {
    }

    AudioControlWindow::~AudioControlWindow()
    {
    }

    bool AudioControlWindow::init(utility::ErrorState &errorState)
    {
        mPlaybackEntityInstance = mScene->findEntity(mPlaybackEntity->mID);
        if(!errorState.check(mPlaybackEntityInstance!= nullptr, "PlaybackEntity %s not found in scene", mPlaybackEntity->mID.c_str()))
            return false;

        if(!errorState.check(mPlaybackEntityInstance->hasComponent<audio::PlaybackComponentInstance>(), "PlaybackEntity %s has no PlaybackComponentInstance", mPlaybackEntity->mID.c_str()))
            return false;

        auto* audio_service = mAppGUIService.getCore().getService<audio::AudioService>();
        mAudioDeviceSettingsGui = std::make_unique<audio::AudioDeviceSettingsGui>(*audio_service, true);

        auto& playback_component = mPlaybackEntityInstance->getComponent<audio::PlaybackComponentInstance>();
        mPitch = playback_component.getPitch();
        mStartPosition = playback_component.getComponent<audio::PlaybackComponent>()->mStartPosition;

        return true;
    }

    void AudioControlWindow::drawContent(double deltaTime)
    {
        auto* imgui_service = mAppGUIService.getCore().getService<IMGuiService>();

        ImGui::TextColored(imgui_service->getPalette().mHighlightColor2, "Playback Control");
        auto& playback_component = mPlaybackEntityInstance->getComponent<audio::PlaybackComponentInstance>();
        if(playback_component.isPlaying())
        {
            if(ImGui::Button("Pause"))
            {
                playback_component.stop();
            }
        }
        else
        {
            if(ImGui::Button("Play"))
            {
                playback_component.start(mStartPosition);
            }
        }
        if(ImGui::InputDouble("Start Position", &mStartPosition))
        {
        }
        if(ImGui::InputDouble("Pitch", &mPitch))
        {
            playback_component.setPitch(mPitch);
        }

        ImGui::Separator();
        ImGui::TextColored(imgui_service->getPalette().mHighlightColor2, "Audio Device Settings");
        mAudioDeviceSettingsGui->drawGui();
    }
}