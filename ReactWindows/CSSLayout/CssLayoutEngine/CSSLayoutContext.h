#pragma once

#include "MeasureOutput.h"

namespace CSSLayoutEngine
{
    public ref class CSSLayoutContext sealed
    {
    public:
        CSSLayoutContext()
        {
            mMeasureOutput = ref new MeasureOutput();
        }

        property MeasureOutput^ MeasureOutputField
        {
            MeasureOutput^ get() { return mMeasureOutput; }
            void set(MeasureOutput^ output) { mMeasureOutput = output; }
        }
        property int CurrentGenerationCount;

    private:
        MeasureOutput^ mMeasureOutput;
    };
}
