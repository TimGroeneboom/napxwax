#include "playbackcontrolwindow.h"
#include "appguiservice.h"
#include "audio/component/playbackcomponent.h"
#include "imgui.h"

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::PlaybackControlWindow)
    RTTI_CONSTRUCTOR(nap::AppGUIService&)
    RTTI_PROPERTY("Scene", &nap::PlaybackControlWindow::mScene, nap::rtti::EPropertyMetaData::Required)
    RTTI_PROPERTY("PlaybackEntity", &nap::PlaybackControlWindow::mPlaybackEntity, nap::rtti::EPropertyMetaData::Required)
RTTI_END_CLASS

namespace nap
{
    PlaybackControlWindow::PlaybackControlWindow(AppGUIService& guiService)
        : AppGUIWindow(guiService)
    {
    }

    PlaybackControlWindow::~PlaybackControlWindow()
    {
    }

    bool PlaybackControlWindow::init(utility::ErrorState &errorState)
    {
        mPlaybackEntityInstance = mScene->findEntity(mPlaybackEntity->mID);
        if(!errorState.check(mPlaybackEntityInstance!= nullptr, "PlaybackEntity %s not found in scene", mPlaybackEntity->mID.c_str()))
            return false;

        if(!errorState.check(mPlaybackEntityInstance->hasComponent<audio::PlaybackComponentInstance>(), "PlaybackEntity %s has no PlaybackComponentInstance", mPlaybackEntity->mID.c_str()))
            return false;

        auto& playback_component = mPlaybackEntityInstance->getComponent<audio::PlaybackComponentInstance>();
        mPitch = playback_component.getPitch();
        mStartPosition = playback_component.getComponent<audio::PlaybackComponent>()->mStartPosition;

        return true;
    }

    void PlaybackControlWindow::drawContent(double deltaTime)
    {
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
    }
}