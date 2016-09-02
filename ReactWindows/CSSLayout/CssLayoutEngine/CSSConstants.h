#pragma once

#include <cmath>

namespace CSSLayoutEngine
{
    public ref class CSSConstants sealed
    {
    public:
        static property float Undefined
        {
            float get() { return NAN; }
        }
        static bool IsUndefined(float value);
    };
}
