#include "xwaxdemoservice.h"
#include "timecoderwindow.h"
#include "appguiservice.h"

// External Includes
#include <nap/core.h>
#include <nap/resourcemanager.h>
#include <nap/logger.h>
#include <iostream>
#include <audiocontrolwindow.h>

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::XWaxDemoService)
        RTTI_CONSTRUCTOR(nap::ServiceConfiguration*)
RTTI_END_CLASS

namespace nap
{
    bool XWaxDemoService::init(nap::utility::ErrorState& errorState)
    {
        return true;
    }


    void XWaxDemoService::update(double deltaTime)
    {
    }


    void XWaxDemoService::getDependentServices(std::vector<rtti::TypeInfo>& dependencies)
    {
    }


    void XWaxDemoService::shutdown()
    {
    }


    void XWaxDemoService::registerObjectCreators(rtti::Factory &factory)
    {
        factory.addObjectCreator(std::make_unique<TimecoderWindowObjectCreator>(*getCore().getService<AppGUIService>()));
        factory.addObjectCreator(std::make_unique<PlaybackControlWindowObjectCreator>(*getCore().getService<AppGUIService>()));
    }
}
