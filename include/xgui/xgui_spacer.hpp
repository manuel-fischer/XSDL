#pragma once

#include "xgui/xgui_component.hpp"

namespace xgui
{
    struct spacer : component
    {
        spacer(composite* parent, SDL_Rect rect,
               lazy_ref<stylesheet, flex_style> flex)
        {
            if(parent) parent->add_child(this);
            this->flex = flex;
            this->rect = rect;
        }

        void render(xsdl::renderer& renderer, stylesheet* stylesheet)
        {}

        void reset_render()
        {}


        void on_event(const SDL_Event& event)
        {}
    };
}