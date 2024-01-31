// Local Includes
#include "xwaxservice.h"

// External Includes
#include <nap/core.h>
#include <nap/resourcemanager.h>
#include <nap/logger.h>
#include <iostream>

#include "timecoder_wrapper.h"

RTTI_BEGIN_CLASS_NO_DEFAULT_CONSTRUCTOR(nap::XWaxService)
	RTTI_CONSTRUCTOR(nap::ServiceConfiguration*)
RTTI_END_CLASS

namespace nap
{
	bool XWaxService::init(nap::utility::ErrorState& errorState)

    {
		return true;
	}


	void XWaxService::update(double deltaTime)
	{
	}
	

	void XWaxService::getDependentServices(std::vector<rtti::TypeInfo>& dependencies)
	{
	}
	

	void XWaxService::shutdown()
	{
        timecoder_free_lookup();
	}
}
