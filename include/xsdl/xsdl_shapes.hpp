#pragma once

#include "xsdl/xsdl.hpp"

namespace xsdl
{
    struct pen
    {
        renderer* the_renderer;
        int internal_width;
        texture line_crosssection;

        static pen create_pen(class renderer&, color, int width, bool premultiply=false);
        static pen create_smooth_pen(class renderer&, color, int width, bool premultiply=false);

        void draw_line(float x0, float y0, float x1, float y1);
    };
}


// Implementation

#include <vector>
#include <algorithm>
#include <cmath>

namespace xsdl
{
    /*static const xsdl::color line_pixels[] = { {255,255,255,0}, {255,255,255,255}, {255,255,255,255}, {255,255,255,0} };
    auto line = xsdl::texture(renderer, xsdl::surface::from_data(1, std::size(line_pixels), line_pixels));
    auto draw_line = [&](xsdl::renderer& renderer, float x0, float y0, float x1, float y1)
    {
        float length = std::hypotf(y1-y0, x1-x0);
        float angle  = atan2f(y1-y0, x1-x0);

        SDL_FRect dst = { (x0+x1)/2.f-length/2.f, (y0+y1)/2.f-std::size(line_pixels)/2.f, length, std::size(line_pixels)};
        renderer.paint(line).atf(dst, angle/M_PI*180.f);
    };*/


    inline pen pen::create_pen(class renderer& renderer, color pen_color, int width, bool premultiply)
    {
        xsdl::surface s = surface::create(1, width+2);
        xsdl::pixel_view v = s.pixels32();

        v.fill(pen_color.premultiply_if(premultiply), {0, 1, 0, width+1});
        v(0, 0) = v(0, width+1) = color{pen_color.r, pen_color.g, pen_color.b, 0}.premultiply_if(premultiply);

        return pen{
            .the_renderer = &renderer,
            .internal_width = width+2,
            .line_crosssection = texture(renderer, s)
        };
    }

    
    inline pen pen::create_smooth_pen(class renderer& renderer, color pen_color, int width, bool premultiply)
    {
        // 1+x^4 - 2x^2
        long long max = (width)*(width);
        auto fn = [&](int x)
        {
            long long x2 = x*x;
            return max*max + x2*x2 - 2*x2*max;
        };

        
        xsdl::surface s = surface::create(1, width+2);
        xsdl::pixel_view v = s.pixels32();

        std::vector<class color> line_pixels(width+2);
        for(int y = 1; y < width+1; ++y)
        {
            int alpha = fn(y*2-(width+2)+1)*pen_color.a / (max*max);
            v(0, y) = color{pen_color.r, pen_color.g, pen_color.b, uint8_t(alpha)}.premultiply_if(premultiply);
        }
        v(0, 0) = v(0, width+1) = color{pen_color.r, pen_color.g, pen_color.b, 0}.premultiply_if(premultiply);

        return pen{
            .the_renderer = &renderer,
            .internal_width = width+2,
            .line_crosssection = texture(renderer, s)
        };
    }

    
    inline void pen::draw_line(float x0, float y0, float x1, float y1)
    {
        float length = std::hypotf(y1-y0, x1-x0);
        float angle  = atan2f(y1-y0, x1-x0);

        //SDL_FRect dst = { (x0+x1)/2.f-length/2.f, (y0+y1)/2.f-internal_width/2.f, length, internal_width};
        SDL_FRect dst = { (x0+x1-length)/2.f, (y0+y1-internal_width)/2.f, length, float(internal_width)};
        the_renderer->paint(line_crosssection).atf(dst, angle/M_PI*180.f);
    }
}