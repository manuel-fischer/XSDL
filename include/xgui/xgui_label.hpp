#pragma once

#include "xgui/xgui_component.hpp"

namespace xgui
{
    struct label_style
    {
        xsdl::font* font;
        xsdl::color foreground;
    };

    struct label : component
    {
        lazy_ref<stylesheet, label_style> style = nullptr;
        std::string text;
        xsdl::texture text_texture = nullptr;

        label(composite* parent, SDL_Rect rect, std::string text,
              lazy_ref<stylesheet, flex_style> flex,
              lazy_ref<stylesheet, label_style> style)
        {
            if(parent) parent->add_child(this);
            this->flex = flex;
            this->style = style;
            this->rect = rect;
            this->text = std::move(text);
            this->text_texture = nullptr;
        }

        void render(xsdl::renderer& renderer, stylesheet* stylesheet)
        {
            label_style& s = style(stylesheet);
            
            if(!text_texture)
            {
                text_texture = xsdl::texture(renderer, s.font->blended(text.c_str(), s.foreground));
            }

            int mx2 = rect.x*2+rect.w;
            int my2 = rect.y*2+rect.h;
            auto[tw, th] = text_texture.size();
            SDL_Rect r {
                .x = (mx2-tw)/2,
                .y = (my2-th)/2,
                .w = tw,
                .h = th,
            };
            renderer.paint(text_texture).at(r);
        }

        void reset_render()
        {
            text_texture = nullptr;
        }

        void on_event(const SDL_Event& event, behavior* behavior) override
        {}
    };
}