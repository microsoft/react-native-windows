#pragma once

namespace CssLayoutEngine
{
    public ref class Spacing sealed
    {
    public:
        Spacing();

        const int LEFT = 0;
        const int TOP = 1;
        const int RIGHT = 2;
        const int BOTTOM = 3;
        const int START = 4;
        const int END = 5;
        const int HORIZONTAL = 6;
        const int VERTICAL = 7;
        const int ALL = 8;

    private:
    };
}
