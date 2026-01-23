#pragma once

#include <nap/core.h>

namespace nap
{
    namespace audio
    {
		//////////////////////////////////////////////////////////////////////////
		// Timecode Control
		//////////////////////////////////////////////////////////////////////////

        /**
         * Enum for the different timecode control signals
         */
        enum NAPAPI ETimecodeContol : uint8
        {
            SERATO_2A		= 0,
            SERATO_2B		= 1,
            SERATO_CD		= 2,
            TRACTOR_A		= 3,
            TRACTOR_B		= 4,
            MIXVIBES_V2		= 5,
            MIXVIBES_7INCH	= 6,
            PIONEER_A		= 7,
            PIONEER_B		= 8
        };

		// Vinyl control options to list
		NAPAPI const std::vector<std::string>& tcControlToList();

		/**
		 * Enum for the different modes
		 */
		enum NAPAPI ETimecodeMode : uint8
		{
			PassThrough		= 0,			///< Signal pass-through
			DVS								///< Signal as DVS
		};

		// Timecode mode to list
		NAPAPI const std::vector<std::string>& tcModeToList();


		//////////////////////////////////////////////////////////////////////////
		// RPM
		//////////////////////////////////////////////////////////////////////////

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
		static constexpr float tcGetReferenceSpeed(ETimeCodeSpeed speed)
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

		// Return RPM options as list of strings
		NAPAPI const std::vector<std::string>& tcRPMtoList();
    }
}
