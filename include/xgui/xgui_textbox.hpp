#pragma once

#include "xgui/xgui_component.hpp"

namespace xgui
{
    struct textbox_style
    {
        xsdl::font* font;
        xsdl::color foreground,
                    background;

        struct { int left; } padding;
    };

    struct textbox : component
    {
        lazy_ref<stylesheet, textbox_style> style = nullptr;
        std::string text;
        xsdl::texture text_texture = nullptr;

        textbox(composite* parent, SDL_Rect rect, std::string text,
              lazy_ref<stylesheet, flex_style> flex,
              lazy_ref<stylesheet, textbox_style> style)
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
            textbox_style& s = style(stylesheet);
            

            xsdl::color c = s.background;

            renderer.draw_color(c);
            renderer.fill(rect);

            renderer.draw_color({255, 255, 255, 48});
            renderer.frame(rect, 1);

            if(!text_texture)
            {
                text_texture = xsdl::texture(renderer, s.font->blended(text.c_str(), s.foreground));
            }

            int mx2 = rect.x*2+rect.w;
            int my2 = rect.y*2+rect.h;
            auto[tw, th] = text_texture.size();
            SDL_Rect r {
                .x = rect.x + s.padding.left,
                .y = (my2-th)/2,
                .w = tw,
                .h = th,
            };
            renderer.paint(text_texture).at(r);

            r.x += s.font->size(text.c_str()).x;
            r.w = 1;
            renderer.draw_color(s.foreground);
            renderer.fill(r);
        }

        void reset_render()
        {
            text_texture = nullptr;
        }

        void on_event(const SDL_Event& event)
        {}
    };
}