#pragma once
#include "CSSMeasureMode.h"

using namespace Platform;

namespace CSSLayoutEngine
{
    public ref class CSSCachedMeasurement sealed
    {
    public:
        property float AvailableWidth;
        property float AvailableHeight;
        property IBox<CSSMeasureMode>^ WidthMeasureMode;
        property IBox<CSSMeasureMode>^ HeightMeasureMode;
        property float ComputedWidth;
        property float ComputedHeight;
    };
}
