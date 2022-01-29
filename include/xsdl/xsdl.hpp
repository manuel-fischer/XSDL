#pragma once

#include <cstdint>
#include <exception>
#include <string>
#include <string_view>
#include <utility> // exchange

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "xsdl/xsdl_color.hpp"
#include <optional>

namespace xsdl
{
    using flags = uint32_t;


    class sdl_exception : public std::exception
    {
        std::string m_msg;
    public:
        sdl_exception(std::string msg);
        virtual const char* what() const noexcept;
    };

    void throw_sdl_exception(std::string_view context);

    struct sdl
    {
        bool active;

        ~sdl() noexcept;
        sdl(uint32_t subsystems);
        
        sdl(sdl&& other): active(std::exchange(other.active, false)) {}
        sdl& operator=(sdl other) { std::swap(active, other.active); return *this; }
        sdl(const sdl&) = delete;
    };

    struct sdl_image
    {
        bool active;

        ~sdl_image() noexcept;
        sdl_image(uint32_t subsystems);
        
        sdl_image(sdl_image&& other): active(std::exchange(other.active, false)) {}
        sdl_image& operator=(sdl_image other) { std::swap(active, other.active); return *this; }
        sdl_image(const sdl_image&) = delete;
    };



    template<class T>
    inline auto err_check(T* ptr, std::string_view context) -> T*
    {
        if(ptr == nullptr) throw_sdl_exception(context);
        return ptr;
    }

    inline void int_check(int retval, std::string_view context)
    {
        if(retval < 0) throw_sdl_exception(context);
    }


    #define XSDL_DECLARE_MANAGED(clsname, ptr_type, destroy_function) \
        ptr_type ptr; \
        ~clsname() noexcept { destroy_function(ptr); } \
        clsname(clsname&& other): ptr(std::exchange(other.ptr, nullptr)) {} \
        clsname& operator=(clsname other) { std::swap(ptr, other.ptr); return *this; } \
        clsname(const clsname&) = delete; \
        clsname(ptr_type p): ptr(p) {} \
        ptr_type get() { return ptr; } \
        ptr_type get_noconst() const { return ptr; }


    template<class T>
    auto opt2ptr(std::optional<T>& opt) -> T*
    {
        return opt ? &opt.value() : nullptr;
    }


    // Corresponding to SDL structs
    struct surface;
    struct window;
    struct renderer;
    struct texture;

    // Custom
    struct painter;

    struct window
    {
        XSDL_DECLARE_MANAGED(window, SDL_Window*, SDL_DestroyWindow)

        window(char* title, int x, int y, int w, int h, flags);

        SDL_Point size() const noexcept;
    };

    struct renderer
    {
        XSDL_DECLARE_MANAGED(renderer, SDL_Renderer*, SDL_DestroyRenderer)

        renderer(window&, int index, flags);
        renderer(window& window, flags flags): renderer(window, -1, flags) {}
        renderer(surface&);


        void draw_color(color color);
        void clear();
        void present() noexcept;
        [[nodiscard]]
        painter paint(const texture&, std::optional<SDL_Rect> srcrect=std::nullopt);
    };



    struct surface
    {
        XSDL_DECLARE_MANAGED(surface, SDL_Surface*, SDL_FreeSurface)

        static surface load(const char* filename);
        static surface from_data(size_t w, size_t h, const color* data);

        SDL_Point size() const noexcept;
    };

    struct painter
    {
        renderer* renderer;
        const texture* texture;
        std::optional<SDL_Rect> srcrect;

        painter at(std::optional<SDL_Rect> dstrect=std::nullopt);
        painter at(std::optional<SDL_Rect> dstrect, 
                   double angle,
                   std::optional<SDL_Point> center=std::nullopt,
                   SDL_RendererFlip flip=SDL_RendererFlip::SDL_FLIP_NONE);


        painter atf(std::optional<SDL_FRect> dstrect=std::nullopt);
        painter atf(std::optional<SDL_FRect> dstrect, 
                    double angle,
                    std::optional<SDL_FPoint> center=std::nullopt,
                    SDL_RendererFlip flip=SDL_RendererFlip::SDL_FLIP_NONE);
    };

    
    std::pair<window, renderer> create_window_and_renderer(int w, int h, flags);


    

    struct texture
    {
        XSDL_DECLARE_MANAGED(texture, SDL_Texture*, SDL_DestroyTexture)

        texture(renderer&, const surface&);

        SDL_Point size() const noexcept;
    };
}
