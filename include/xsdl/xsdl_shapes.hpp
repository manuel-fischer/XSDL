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


    inline pen pen::create_pen(class renderer& renderer, color color, int width, bool premultiply)
    {
        std::vector<class color> line_pixels(width+2, color.premultiply_if(premultiply));
        line_pixels.front() = line_pixels.back() = (class color){color.r, color.g, color.b, 0}.premultiply_if(premultiply);

        return pen{
            .the_renderer = &renderer,
            .internal_width = width+2,
            .line_crosssection = texture(renderer, xsdl::surface::from_data(1, line_pixels.size(), line_pixels.data()))
        };
    }

    
    inline pen pen::create_smooth_pen(class renderer& renderer, color color, int width, bool premultiply)
    {
        // 1+x^4 - 2x^2
        long long max = (width)*(width);
        auto fn = [&](int x)
        {
            long long x2 = x*x;
            return max*max + x2*x2 - 2*x2*max;
        };

        std::vector<class color> line_pixels(width+2);
        for(int i = 1; i < width+1; ++i)
        {
            int alpha = fn(i*2-(width+2)+1)*color.a / (max*max);
            line_pixels[i] = (class color){color.r, color.g, color.b, uint8_t(alpha)}.premultiply_if(premultiply);
        }
        line_pixels.front() = line_pixels.back() = (class color){color.r, color.g, color.b, 0}.premultiply_if(premultiply);

        return pen{
            .the_renderer = &renderer,
            .internal_width = width+2,
            .line_crosssection = texture(renderer, xsdl::surface::from_data(1, line_pixels.size(), line_pixels.data()))
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