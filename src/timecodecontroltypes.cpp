#include "timecodecontroltypes.h"

RTTI_BEGIN_ENUM(nap::audio::ETimecodeContol)
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_2A,			"serato_2a"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_2B,			"serato_2b"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::SERATO_CD,			"serato_cd"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::TRACTOR_A,			"traktor_a"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::TRACTOR_B,			"traktor_b"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::MIXVIBES_V2,		"mixvibes_v2"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::MIXVIBES_7INCH,	"mixvibes_7inch"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::PIONEER_A,			"pioneer_a"),
	RTTI_ENUM_VALUE(nap::audio::ETimecodeContol::PIONEER_B,			"pioneer_b")
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

