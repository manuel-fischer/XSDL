#pragma once

#include "xsdl/xsdl.hpp"
#include <SDL2/SDL_ttf.h>

namespace xsdl
{
    struct sdl_ttf
    {
        sdl_ttf();
        ~sdl_ttf() noexcept;
    };


    struct font
    {
        XSDL_DECLARE_MANAGED(font, TTF_Font*, TTF_CloseFont)

    	static font load(const char* filename, int size);

        surface fast(const char* text, color fg, color bg) const;
        surface solid(const char* text, color fg) const;
        surface shaded(const char* text, color fg, color bg) const;
        surface blended(const char* text, color fg) const;
        surface blended_wrapped(const char* text, color fg, int) const;

        SDL_Point size(const char* text) const;
    };
}