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
        ptr_type get_noconst() const { return ptr; } \
        operator bool() const { return ptr != nullptr; }


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

        void fill(SDL_Rect rect);
        void frame(SDL_Rect rect, int width);
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
        renderer* the_renderer;
        const texture* the_texture;
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

    struct sdl_hint
    {
        const char* name;
        const char* value;
    };
    
    std::pair<window, renderer> create_window_and_renderer(int w, int h, flags, std::initializer_list<sdl_hint> hints={});


    

    struct texture
    {
        XSDL_DECLARE_MANAGED(texture, SDL_Texture*, SDL_DestroyTexture)

        texture(renderer&, const surface&);

        SDL_Point size() const noexcept;
    };



    inline constexpr bool point_in_rect(SDL_Point p, SDL_Rect r)
    {
        return r.x <= p.x && p.x < r.x+r.w
            && r.y <= p.y && p.y < r.y+r.h;
    }
}










// Implementation

#include <sstream>

namespace xsdl
{
    
    inline sdl_exception::sdl_exception(std::string msg): m_msg(msg) {}

    inline const char* sdl_exception::what() const noexcept
    {
        return m_msg.c_str();
    }

    inline void throw_sdl_exception(std::string_view context)
    {
        std::stringstream s;
        s << context << ": " << SDL_GetError();
        throw sdl_exception(std::move(s).str());
    }





    inline sdl::sdl(flags flags)
    {
        if(SDL_Init(flags) < 0) throw_sdl_exception("SDL_Init");
        active = true;
    }
    inline sdl::~sdl() noexcept { if(active) SDL_Quit(); }


    inline sdl_image::sdl_image(flags flags)
    {
        if(IMG_Init(flags) < 0) throw_sdl_exception("IMG_Init");
        active = true;
    }
    inline sdl_image::~sdl_image() noexcept { if(active) IMG_Quit(); }



    inline SDL_Point window::size() const noexcept
    {
        SDL_Point ret;
        SDL_GetWindowSize(ptr, &ret.x, &ret.y);
        return ret;
    }


    
    inline surface surface::load(const char* filename)
    {
        return surface(err_check(IMG_Load(filename), "IMG_Load"));
    }

    
    inline surface surface::from_data(size_t w, size_t h, const color* data)
    {
        return surface(err_check(SDL_CreateRGBSurfaceFrom(const_cast<color*>(data), w, h, 32, 4*w,
                                                          color::red_mask,
                                                          color::grn_mask,
                                                          color::blu_mask,
                                                          color::alp_mask), "SDL_CreateRGBSurfaceFrom"));
    }

    inline SDL_Point surface::size() const noexcept
    {
        return {ptr->w, ptr->h};
    }


    inline window::window(char* title, int x, int y, int w, int h, flags flags):
        ptr(err_check(SDL_CreateWindow(title, x, y, w, h, flags), "SDL_CreateWindow"))
    {}




    inline renderer::renderer(window& window, int index, flags flags):
        ptr(err_check(SDL_CreateRenderer(window.get(), index, flags), "SDL_CreateRenderer"))
    {}

    inline renderer::renderer(surface& surface):
        ptr(err_check(SDL_CreateSoftwareRenderer(surface.get()), "SDL_CreateSoftwareRenderer"))
    {}


    inline void renderer::draw_color(color color) {
        int_check(SDL_SetRenderDrawColor(ptr, color.r, color.g, color.b, color.a), "SDL_SetRenderDrawColor");
    }
    inline void renderer::clear() {
        int_check(SDL_RenderClear(ptr), "SDL_RenderClear");
    }
    inline void renderer::present() noexcept {
        SDL_RenderPresent(ptr);
    }
    inline painter renderer::paint(const texture& texture, std::optional<SDL_Rect> srcrect) {
        return {this, &texture, srcrect};
    }

    inline void renderer::fill(SDL_Rect rect)
    {
        SDL_RenderFillRect(get(), &rect);
    }

    inline void renderer::frame(SDL_Rect rect, int width)
    {
        SDL_Rect rects[4] {
            {rect.x, rect.y, rect.w, width},
            {rect.x, rect.y+rect.h-width, rect.w, width},
            {rect.x, rect.y+width, width, rect.h-2*width},
            {rect.x+rect.w-width, rect.y+width, width, rect.h-2*width},
        };
        SDL_RenderFillRects(get(), rects, 4);
    }

    inline painter painter::at(std::optional<SDL_Rect> dstrect)
    {
        int_check(SDL_RenderCopy(the_renderer->get(), the_texture->get_noconst(), opt2ptr(srcrect), opt2ptr(dstrect)), "SDL_RenderCopy");
        return *this;
    }

    inline painter painter::at(std::optional<SDL_Rect> dstrect, 
                        double angle,
                        std::optional<SDL_Point> center,
                        SDL_RendererFlip flip)
    {
        int_check(SDL_RenderCopyEx(the_renderer->get(), the_texture->get_noconst(), opt2ptr(srcrect), opt2ptr(dstrect), angle, opt2ptr(center), flip), "SDL_RenderCopyEx");
        return *this;
    }


    inline painter painter::atf(std::optional<SDL_FRect> dstrect)
    {
        int_check(SDL_RenderCopyF(the_renderer->get(), the_texture->get_noconst(), opt2ptr(srcrect), opt2ptr(dstrect)), "SDL_RenderCopy");
        return *this;
    }

    inline painter painter::atf(std::optional<SDL_FRect> dstrect, 
                         double angle,
                         std::optional<SDL_FPoint> center,
                         SDL_RendererFlip flip)
    {
        int_check(SDL_RenderCopyExF(the_renderer->get(), the_texture->get_noconst(), opt2ptr(srcrect), opt2ptr(dstrect), angle, opt2ptr(center), flip), "SDL_RenderCopyEx");
        return *this;
    }





    inline std::pair<window, renderer> create_window_and_renderer(int w, int h, flags flags, std::initializer_list<sdl_hint> hints)
    {
        for(sdl_hint hint : hints)
            SDL_SetHint(hint.name, hint.value);

        SDL_Window* window;
        SDL_Renderer* renderer;

        if(SDL_CreateWindowAndRenderer(w, h, flags, &window, &renderer) < 0)
            throw_sdl_exception("SDL_CreateWindowAndRenderer");
        
        return { window, renderer };
    }





    inline texture::texture(renderer& renderer, const surface& surface):
        texture(err_check(SDL_CreateTextureFromSurface(renderer.get(), surface.get_noconst()), "SDL_CreateTextureFromSurface"))
    {}
    
    inline SDL_Point texture::size() const noexcept
    {
        SDL_Point ret;
        SDL_QueryTexture(ptr, nullptr, nullptr, &ret.x, &ret.y);
        return ret;
    }
}