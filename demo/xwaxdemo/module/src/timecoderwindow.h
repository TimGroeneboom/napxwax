#pragma once

#include <timecodercomponent.h>
#include <appgui.h>

namespace nap
{
    class NAPAPI TimecoderWindow : public AppGUIWindow
    {
        RTTI_ENABLE(AppGUIWindow)
    public:
        TimecoderWindow(AppGUIService& guiService);
        ~TimecoderWindow() override;

        void drawContent(double deltaTime) override;

        ResourcePtr<audio::TimecoderComponent> mTimecoderComponent;
    };

    using TimecoderWindowObjectCreator = rtti::ObjectCreator<TimecoderWindow, AppGUIService&>;
}