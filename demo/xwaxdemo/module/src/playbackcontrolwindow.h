#pragma once

#include <entity.h>
#include <appgui.h>
#include <scene.h>

namespace nap
{
    class NAPAPI PlaybackControlWindow : public AppGUIWindow
    {
    RTTI_ENABLE(AppGUIWindow)
    public:
        PlaybackControlWindow(AppGUIService& guiService);
        ~PlaybackControlWindow() override;

        bool init(utility::ErrorState &errorState) override;

        void drawContent(double deltaTime) override;

        ResourcePtr<Scene> mScene;
        ResourcePtr<Entity> mPlaybackEntity;
    private:
        rtti::ObjectPtr<EntityInstance> mPlaybackEntityInstance;

        double mPitch = 1.0;
        double mStartPosition = 0.0;
    };

    using PlaybackControlWindowObjectCreator = rtti::ObjectCreator<PlaybackControlWindow, AppGUIService&>;
}