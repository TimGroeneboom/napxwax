#pragma once

#include <entity.h>
#include <appgui.h>
#include <scene.h>

namespace nap
{
    namespace audio
    {
        class AudioDeviceSettingsGui;
    }


    class NAPAPI AudioControlWindow : public AppGUIWindow
    {
    RTTI_ENABLE(AppGUIWindow)
    public:
        AudioControlWindow(AppGUIService& guiService);
        ~AudioControlWindow() override;

        bool init(utility::ErrorState &errorState) override;

        void drawContent(double deltaTime) override;

        ResourcePtr<Scene> mScene;
        ResourcePtr<Entity> mPlaybackEntity;
    private:
        rtti::ObjectPtr<EntityInstance> mPlaybackEntityInstance;
        std::unique_ptr<audio::AudioDeviceSettingsGui> mAudioDeviceSettingsGui;

        double mPitch = 1.0;
        double mStartPosition = 0.0;
    };

    using PlaybackControlWindowObjectCreator = rtti::ObjectCreator<AudioControlWindow, AppGUIService&>;
}