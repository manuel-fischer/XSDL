#pragma once

#include "xgui/xgui_component.hpp"

namespace xgui
{
    struct button_style
    {
        xsdl::font* font;
        xsdl::color foreground,
                    background,

                    //foreground_hover = foreground | xsdl::color {255, 255, 255, 100},
                    background_hover = foreground | xsdl::color {255, 255, 255, 100},

                    //foreground_down = foreground | xsdl::color {0, 0, 0, 100},
                    background_down = foreground | xsdl::color {0, 0, 0, 100};
    };

    enum class button_state
    {
        normal,
        hover,
        down,
    };

    struct button : component
    {
        lazy_ref<stylesheet, button_style> style = nullptr;
        std::string text;
        xsdl::texture text_texture = nullptr;
        button_state state;

        button(composite* parent, SDL_Rect rect, std::string text,
              lazy_ref<stylesheet, flex_style> flex,
              lazy_ref<stylesheet, button_style> style)
        {
            if(parent) parent->add_child(this);
            this->flex = flex;
            this->style = style;
            this->rect = rect;
            this->text = std::move(text);
            this->text_texture = nullptr;
            this->state = button_state::normal;
        }

        void render(xsdl::renderer& renderer, stylesheet* stylesheet)
        {
            button_style& s = style(stylesheet);
            

            xsdl::color c = s.background;
            if(state == button_state::hover) c = s.background_hover;
            if(state == button_state::down)  c = s.background_down;

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




        void on_event(const SDL_Event& event)
        {
            using namespace event_util;
            if(is_mouse_event(event))
            {
                auto[x, y] = get_mouse_pos(event);
                int left_button = has_button(event, SDL_BUTTON_LEFT);

                bool has_mouse = xsdl::point_in_rect({x, y}, rect);

                if(has_mouse)
                {
                    state = button_state::hover;

                    if(left_button && event.type != SDL_MOUSEBUTTONUP)
                        state = button_state::down;
                }
                else
                {
                    state = button_state::normal;
                }
            }
        }
    };
}