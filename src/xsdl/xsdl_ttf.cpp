#include "xsdl/xsdl_ttf.hpp"

namespace xsdl
{
    sdl_ttf::sdl_ttf()
    {
        int_check(TTF_Init(), "TTF_Init");
    }

    sdl_ttf::~sdl_ttf()
    {
        TTF_Quit();
    }

    
    font font::load(const char* filename, int ptsize)
    {
        return font(err_check(TTF_OpenFont(filename, ptsize), "TTF_OpenFont"));
    }

    surface font::fast(const char* text, color fg, color bg) const
    {
        return surface(err_check(TTF_RenderUTF8(ptr, text, fg, bg), "TTF_RenderUTF8"));
    }
    
    surface font::solid(const char* text, color fg) const
    {
        return surface(err_check(TTF_RenderUTF8_Solid(ptr, text, fg), "TTF_RenderUTF8_Solid"));
    }
    
    surface font::shaded(const char* text, color fg, color bg) const
    {
        return surface(err_check(TTF_RenderUTF8_Shaded(ptr, text, fg, bg), "TTF_RenderUTF8_Shaded"));
    }
    
    surface font::blended(const char* text, color fg) const
    {
        return surface(err_check(TTF_RenderUTF8_Blended(ptr, text, fg), "TTF_RenderUTF8_Blended"));
    }
    
    surface font::blended_wrapped(const char* text, color fg, int wrap_length) const
    {
        return surface(err_check(TTF_RenderUTF8_Blended_Wrapped(ptr, text, fg, wrap_length), "TTF_RenderUTF8_Blended_Wrapped"));
    }

    SDL_Point font::size(const char* text) const
    {
        SDL_Point ret;
        TTF_SizeUTF8(ptr, text, &ret.x, &ret.y);
        return ret;
    }
}