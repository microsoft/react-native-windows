#pragma once

#include "MeasureOutput.h"

namespace CssLayoutEngine
{
    public ref class CSSLayoutContext sealed
    {
    public:
        MeasureOutput^ measureOutput = ref new MeasureOutput();
        int CurrentGenerationCount;
    };
}
