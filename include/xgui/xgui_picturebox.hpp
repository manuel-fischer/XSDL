#pragma once

#include "xgui/xgui_component.hpp"

namespace xgui
{
    enum picturebox_mode
    {
        resize,
        center,
        stretch,
        stretch_aspect
    };

    struct picturebox_style
    {
        picturebox_mode mode;
        bool autosize; // true: update w/h if size of image changes
        bool stretch; // true: stretch image
    };

    struct label : component
    {
        lazy_ref<stylesheet, picturebox_style> style = nullptr;
        xsdl::surface surface = nullptr;
        xsdl::texture texture = nullptr;
        double zoom;

        label(composite* parent, SDL_Rect rect, xsdl::surface surface,
              lazy_ref<stylesheet, flex_style> flex,
              lazy_ref<stylesheet, picturebox_style> style)
        {
            if(parent) parent->add_child(this);
            this->flex = flex;
            this->style = style;
            this->rect = rect;
            this->surface = std::move(surface);
            this->zoom = 1;
        }

        void init_layout(stylesheet* stylesheet) override
        {
            auto[w, h] = surface.size();
            if(style(stylesheet).autosize)
            {
                rect.w = int(zoom*w);
                rect.h = int(zoom*h);
            }
        }

        void update_layout(SDL_Rect new_rect, stylesheet* stylesheet) override
        {
            component::update_layout(new_rect, stylesheet);

            auto[w, h] = surface.size();
            if(style(stylesheet).stretch)
            {
                zoom = std::min(
                    (double)new_rect.w / (double)w,
                    (double)new_rect.h / (double)h
                );
            }
        }

        void render(xsdl::renderer& renderer, stylesheet* stylesheet)
        {
            picturebox_style& s = style(stylesheet);
            
            if(!texture)
            {
                texture = xsdl::texture(renderer, surface);
            }

            int mx2 = rect.x*2+rect.w;
            int my2 = rect.y*2+rect.h;
            auto[tw, th] = texture.size();
            tw = int(tw * zoom);
            th = int(th * zoom);
            SDL_Rect r {
                .x = (mx2-tw)/2,
                .y = (my2-th)/2,
                .w = tw,
                .h = th,
            };

            renderer.paint(texture).at(r);
        }

        void reset_render()
        {
            texture = nullptr;
        }

        void on_event(const SDL_Event& event, behavior* behavior) override
        {}
    };
}