#pragma once


#include "xsdl/xsdl.hpp"
#include "xsdl/xsdl_gl.hpp"
#include "xsdl/xsdl_ttf.hpp"
#include "xsdl/xsdl_shapes.hpp"

#include <vector>

#include <cassert>
#define xgui_assert assert

namespace xgui
{
    struct gui_root
    {
    };


    /*template<class T>
    concept Component = requires(T& a)
    {
        { a.render() } -> std::is_same_as<void>;
    };*/

        template<class B, class T, class C, class M>
    constexpr size_t memberoffset(M C::* mp)
    {
        //return offsetof(M, *mp);
        //static_assert(std::is_base_of_v<B, C>);
        //static_assert(std::is_base_of_v<T, M>);
        //return (uintptr_t)(&(((B*)(C*)nullptr)->*(T B::*)mp));

        const char obj_dummy[sizeof(C)] {};
        const C* obj = reinterpret_cast<const C*>(obj_dummy);

        /*const C* obj = nullptr;*/
        return ptrdiff_t(intptr_t(&(obj->*mp)) - intptr_t(obj));
    }

    #if 1
    template<class B, class T>
    struct lazy_ref
    {
        constexpr static size_t null_offset = -1;

        size_t offset;

        constexpr lazy_ref(lazy_ref const&) = default;

        constexpr lazy_ref(std::nullptr_t):
            offset(null_offset)
        {}

        template<class C, class M>
        constexpr lazy_ref(M C::* mp):
            offset(memberoffset<B, T>(mp))
        {}

        constexpr operator bool() const { return offset != null_offset; }

        T& operator()(B* obj) const
        {
            xgui_assert(*this);
            return *(T*)((char*)obj + offset);
        }

        const T& operator()(const B* obj) const
        {
            xgui_assert(*this);
            return *(const T*)((const char*)obj + offset);
        }
    };
    #else
    template<class D, class S>
    constexpr D c_cast(S s)
    {
        return (D)s;
    }

    template<class B, class T>
    struct lazy_ref
    {
        T B::* offset;

        constexpr lazy_ref(lazy_ref const&) = default;

        constexpr lazy_ref(std::nullptr_t):
            offset(nullptr)
        {}

        template<class C, class M>
        constexpr lazy_ref(M C::* mp):
            offset(c_cast<T B::*>(c_cast<M B::*>(mp)))
        {}

        constexpr operator bool() const { return offset != nullptr; }

        T& operator()(B* obj) const
        {
            xgui_assert(*this);
            return obj->*offset;
        }

        const T& operator()(const B* obj) const
        {
            xgui_assert(*this);
            return obj->*offset;
        }
    };
    #endif


    namespace event_util
    {

        constexpr inline bool is_mouse_event(const SDL_Event& event)
        {
            return event.type == SDL_MOUSEBUTTONDOWN
                || event.type == SDL_MOUSEBUTTONUP
                || event.type == SDL_MOUSEMOTION;
        }

        constexpr SDL_Point get_mouse_pos(const SDL_Event& event)
        {
            return event.type == SDL_MOUSEMOTION
                ? SDL_Point{ event.motion.x, event.motion.y }
                : SDL_Point{ event.button.x, event.button.y };
        }

        constexpr bool has_button(const SDL_Event& event, uint8_t button)
        {
            return event.type == SDL_MOUSEMOTION
                ? event.motion.state & SDL_BUTTON(button)
                : event.button.button == button;
        }
    }


    // Base class: typesafe
    struct stylesheet {};



    



    constexpr inline int FLEX = ~0 + 1;


    /*struct font_style
    {
        std::string_view name;
        int size;
    };*/


    /*struct component_style
    {

    };*/



    struct form_style
    {
        int width, height;

        xsdl::color background;
    };

    struct form
    {

    };
}


#include "xgui/xgui_component.hpp"
#include "xgui/xgui_button.hpp"
#include "xgui/xgui_label.hpp"
#include "xgui/xgui_textbox.hpp"
#include "xgui/xgui_spacer.hpp"