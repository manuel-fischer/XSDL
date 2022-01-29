#pragma once

#include "xsdl/xsdl.hpp"

namespace xsdl
{
    struct pen
    {
        renderer* renderer;
        int internal_width;
        texture line_crosssection;

        static pen create_pen(class renderer&, color, int width, bool premultiply=false);
        static pen create_smooth_pen(class renderer&, color, int width, bool premultiply=false);

        void draw_line(float x0, float y0, float x1, float y1);
    };
}