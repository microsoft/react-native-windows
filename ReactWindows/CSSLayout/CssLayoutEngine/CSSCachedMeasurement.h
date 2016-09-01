#pragma once
#include "CSSMeasureMode.h"

namespace CssLayoutEngine
{
    public ref class CSSCachedMeasurement sealed
    {
    public:
        float AvailableWidth;
        float AvailableHeight;
        CSSMeasureMode WidthMeasureMode;
        CSSMeasureMode HeightMeasureMode;
        float ComputedWidth;
        float ComputedHeight;
    };
}
