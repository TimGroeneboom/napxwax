#include "timecodecontroltypes.h"

RTTI_BEGIN_ENUM(nap::audio::ETimecodeContol)
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_2A,			"Serato 2A"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_2B,			"Serato 2B"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_CD,			"Serato CD"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::TRACTOR_A,			"Traktor 1A"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::TRACTOR_B,			"Traktor 1B"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::MIXVIBES_V2,		"Mixvibes V2"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::MIXVIBES_7INCH,	"Mixvibes 7\""),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::PIONEER_A,			"Pioneer A"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::PIONEER_B,			"Pioneer B")
RTTI_END_ENUM

RTTI_BEGIN_ENUM(nap::audio::ETimecodeMode)
	RTTI_ENUM_VALUE(nap::audio::ETimecodeMode::DVS,			"DVS"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeMode::PassThrough, "PassThrough")
RTTI_END_ENUM

RTTI_BEGIN_ENUM(nap::audio::ETimeCodeSpeed)
	RTTI_ENUM_VALUE(nap::audio::ETimeCodeSpeed::RPM33, "33 RPM"),
	RTTI_ENUM_VALUE(nap::audio::ETimeCodeSpeed::RPM45, "45 RPM"),
	RTTI_ENUM_VALUE(nap::audio::ETimeCodeSpeed::RPM78, "78 RPM")
RTTI_END_ENUM

namespace nap
{
	namespace audio
	{
		template<typename T>
		static const std::vector<std::string>& enumToList()
		{
			static std::vector<std::string> enum_names;
			static std::once_flag flag;
			std::call_once(flag, [&]()
				{
					auto range = RTTI_OF(T).get_enumeration().get_names();
					enum_names.reserve(range.size());
					for (const auto& name : range)
						enum_names.emplace_back(name.data());
				});
			return enum_names;
		}


		const std::vector<std::string>& nap::audio::tcControlToList()
		{
			return enumToList<ETimecodeContol>();
		}



		const std::vector<std::string>& nap::audio::tcModeToList()
		{
			return enumToList<ETimecodeMode>();
		}


		const std::vector<std::string>& nap::audio::tcRPMtoList()
		{
			return enumToList<ETimeCodeSpeed>();
		}
	}
}
