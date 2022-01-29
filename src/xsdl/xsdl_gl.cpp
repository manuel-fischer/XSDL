#include "xsdl/xsdl_gl.hpp"

namespace xsdl
{
    gl_context::gl_context(window& window):
        ptr(err_check(SDL_GL_CreateContext(window.get()), "SDL_GL_CreateContext"))
    {}

    gl_context gl_context::make_common(window& window)
    {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        return gl_context(window);
    }
}