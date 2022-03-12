#pragma once

#include "xsdl/xsdl.hpp"
#include <tuple>
#include <GL/gl.h>

namespace xsdl
{
    struct gl_context
    {
        XSDL_DECLARE_MANAGED4(gl_context, SDL_GLContext, SDL_GLContext, SDL_GL_DeleteContext)

        gl_context(window&);

        static gl_context make_common(window&);
    };
}


// Implementation

namespace xsdl
{
    
    inline gl_context::gl_context(window& window):
        ptr(err_check(SDL_GL_CreateContext(window.get()), "SDL_GL_CreateContext"))
    {}

    inline gl_context gl_context::make_common(window& window)
    {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        return gl_context(window);
    }
}