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

// Implementation

namespace xsdl
{
    inline sdl_ttf::sdl_ttf()
    {
        int_check(TTF_Init(), "TTF_Init");
    }

    inline sdl_ttf::~sdl_ttf()
    {
        TTF_Quit();
    }

    
    inline font font::load(const char* filename, int ptsize)
    {
        return font(err_check(TTF_OpenFont(filename, ptsize), "TTF_OpenFont"));
    }

    inline surface font::fast(const char* text, color fg, color bg) const
    {
        return surface(err_check(TTF_RenderUTF8(ptr, text, fg, bg), "TTF_RenderUTF8"));
    }
    
    inline surface font::solid(const char* text, color fg) const
    {
        return surface(err_check(TTF_RenderUTF8_Solid(ptr, text, fg), "TTF_RenderUTF8_Solid"));
    }
    
    inline surface font::shaded(const char* text, color fg, color bg) const
    {
        return surface(err_check(TTF_RenderUTF8_Shaded(ptr, text, fg, bg), "TTF_RenderUTF8_Shaded"));
    }
    
    inline surface font::blended(const char* text, color fg) const
    {
        return surface(err_check(TTF_RenderUTF8_Blended(ptr, text, fg), "TTF_RenderUTF8_Blended"));
    }
    
    inline surface font::blended_wrapped(const char* text, color fg, int wrap_length) const
    {
        return surface(err_check(TTF_RenderUTF8_Blended_Wrapped(ptr, text, fg, wrap_length), "TTF_RenderUTF8_Blended_Wrapped"));
    }

    inline SDL_Point font::size(const char* text) const
    {
        SDL_Point ret;
        TTF_SizeUTF8(ptr, text, &ret.x, &ret.y);
        return ret;
    }
}