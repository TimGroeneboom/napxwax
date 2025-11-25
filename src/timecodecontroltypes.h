#pragma once

#include <nap/core.h>

namespace nap
{
    namespace audio
    {
        /**
         * Enum for the different timecode control signals
         */
        enum NAPAPI ETimecodeContol : uint8
        {
            SERATO_2A		= 0,
            SERATO_2B,
            SERATO_CD,
            TRACTOR_A,
            TRACTOR_B,
            MIXVIBES_V2,
            MIXVIBES_7INCH,
            PIONEER_A,
            PIONEER_B
        };

		/**
		 * Enum for the different modes
		 */
		enum NAPAPI ETimecodeMode : uint8
		{
			PassThrough		= 0,			///< Signal pass-through
			DVS								///< Signal as DVS
		};

		/**
		 * RPM enum
		 */
		enum NAPAPI ETimeCodeSpeed : uint8
		{
			RPM33 = 0,
			RPM45 = 1,
			RPM78 = 2
		};

		/**
		 * @return reference speed for RPM
		 */
		static constexpr float getReferenceSpeed(ETimeCodeSpeed speed)
		{
			switch (speed)
			{
			case RPM33:
				return 1.0f;
			case RPM45:
				return 45 / 33.33f;
			case RPM78:
				return 78 / 33.33f;
			default:
				assert(false);
				return 1.0f;
			}
		}
    }
}
