#pragma once

using namespace Platform;

namespace CSSLayoutEngine
{
    public enum class SpacingType
    {
        LEFT = 0,
        TOP = 1,
        RIGHT = 2,
        BOTTOM = 3,
        START = 4,
        END = 5,
        HORIZONTAL = 6,
        VERTICAL = 7,
        ALL = 8
    };

    public ref class Spacing sealed
    {
    public:
        Spacing();
        Spacing(float defaultValue);

        float Get(SpacingType spacingType);
        bool Set(SpacingType spacingType, float value);
        float GetRaw(SpacingType spacingType);
        void Reset();
    private:
        ~Spacing();
        float* mSpacing;
        int mValueFlags = 0;
        float mDefaultValue;
        bool mHasAliasSet;
    };
}
