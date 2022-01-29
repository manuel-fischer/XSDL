#pragma once

#include "xsdl/xsdl.hpp"
#include <tuple>
#include <GL/gl.h>

namespace xsdl
{
    struct gl_context
    {
        XSDL_DECLARE_MANAGED(gl_context, SDL_GLContext, SDL_GL_DeleteContext)

        gl_context(window&);

        static gl_context make_common(window&);
    };
}