#pragma once

#include "xgui/xgui.hpp"

namespace xgui
{

    constexpr inline int fixed = -1;
    struct flex_style
    {
        int width, height;
    };

    struct component
    {
        component* prev,* next,* parent;

        SDL_Rect rect {};

        lazy_ref<stylesheet, flex_style> flex = nullptr;
        

        virtual void render(xsdl::renderer& renderer, stylesheet* stylesheet) = 0;
        virtual void on_event(const SDL_Event& event, behavior* behavior) = 0;

        virtual void init_layout(stylesheet* stylesheet)
        {}

        virtual void update_layout(SDL_Rect new_rect, stylesheet* stylesheet)
        {
            rect = new_rect;
        }
    };


    struct component_iterator
    {
        component* c;

        constexpr component_iterator& operator++()
        {
            c = c->next;
            return *this;
        }

        constexpr bool operator==(const component_iterator& o) const { return c == o.c; };
        constexpr bool operator!=(const component_iterator& o) const { return c != o.c; };

        constexpr component& operator*()
        {
            return *c;
        }
    };


    enum class composite_layout
    {
        none,
        horizontal,
        vertical,
    };

    struct composite_style
    {
        composite_layout layout;

        struct
        {
            int left;
            int top;
            int right;
            int bottom;

            int horizontal;
            int vertical;
        } padding;
    };


    struct composite : component
    {
        component* first = nullptr,* last = nullptr;
        size_t m_size = 0;

        lazy_ref<stylesheet, composite_style> style;

        composite(composite* parent,
                  SDL_Rect rect,
                  lazy_ref<stylesheet, flex_style> flex,
                  lazy_ref<stylesheet, composite_style> style):
            style(style)
        {
            this->rect = rect;
            this->flex = flex;
            if(parent) parent->add_child(this);
        }

        void add_child(component* c)
        {
            ++m_size;
            if(last == nullptr)
            {
                first = last = c;

                c->prev = c->next = nullptr;
            }
            else
            {
                last->next = c;
                c->prev = last;

                c->next = nullptr;
                last = c;
            }
        }


        component_iterator begin()
        {
            return { first };
        }

        component_iterator end()
        {
            return { nullptr };
        }

        size_t size() const
        {
            return m_size;
        }

            template<class Func>
        void each_while(Func&& func)
        {
            for(component& c : *this)
                if(!func(c)) return;
        }

            template<class Func>
        void each(Func&& func)
        {
            for(component& c : *this)
                func(c);
        }


        void render(xsdl::renderer& renderer, stylesheet* stylesheet) override
        {
            each_while([&](auto& child)
            {
                child.render(renderer, stylesheet);
                return true;
            });
        }

        void on_event(const SDL_Event& event, behavior* behavior) override
        {
            each_while([&](auto& child)
            {
                child.on_event(event, behavior);
                return true;
            });
        }

        void update_layout(SDL_Rect new_rect, stylesheet* stylesheet) override
        {
            component::update_layout(new_rect, stylesheet);

            const composite_style& s = style(stylesheet);

            auto update = [&](int SDL_Rect::* mp_pos,
                              int SDL_Rect::* mp_size,
                              int flex_style::* mp_wheight,
                              int inner_padding)
            {
                SDL_Rect rflex =
                {
                    .x = rect.x + s.padding.left,
                    .y = rect.y + s.padding.top,
                    .w = rect.w - s.padding.left - s.padding.right,
                    .h = rect.h - s.padding.top - s.padding.bottom
                };


                int fixed_size = 0;
                int flex_wheight = 0;

                each([&](auto& c)
                {
                    c.init_layout(stylesheet);

                    auto& f = c.flex(stylesheet);
                    
                    if(f.*mp_wheight == fixed)
                        fixed_size += c.rect.*mp_size;
                    else
                        flex_wheight += f.*mp_wheight;
                });

                rflex.*mp_size -= fixed_size + (size()-1) * inner_padding;

                each([&](auto& c)
                {
                    auto& f = c.flex(stylesheet);

                    SDL_Rect cr
                    {
                        .x = rflex.x,
                        .y = rflex.y,
                        .w = (f.width  == fixed) ? c.rect.w : rflex.w,
                        .h = (f.height == fixed) ? c.rect.h : rflex.h,
                    };

                    if(f.*mp_wheight != fixed)
                    {   
                        cr.*mp_size = cr.*mp_size * f.*mp_wheight / (flex_wheight?flex_wheight:1);
                        flex_wheight -= f.*mp_wheight;
                        rflex.*mp_size -= cr.*mp_size;
                    }

                    c.update_layout(cr, stylesheet);

                    rflex.*mp_pos += cr.*mp_size + inner_padding;
                });
            };

            if(s.layout == composite_layout::horizontal)
                update(
                    &SDL_Rect::x,
                    &SDL_Rect::w,
                    &flex_style::width,
                    s.padding.horizontal
                );
            else if(s.layout == composite_layout::vertical)
                update(
                    &SDL_Rect::y,
                    &SDL_Rect::h,
                    &flex_style::height,
                    s.padding.vertical
                );
        }
    };
    
}