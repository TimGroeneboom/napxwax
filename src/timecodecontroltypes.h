#pragma once

#include <nap/core.h>

namespace nap
{
    namespace audio
    {
        /**
         * Enum for the different timecode control signals
         */
        enum NAPAPI ETimecodeContol
        {
            SERATO_2A,
            SERATO_2B,
            SERATO_CD,
            TRACTOR_A,
            TRACTOR_B,
            MIXVIBES_V2,
            MIXVIBES_7INCH,
            PIONEER_A,
            PIONEER_B
        };
    }
}