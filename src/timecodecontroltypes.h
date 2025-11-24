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
    }
}
