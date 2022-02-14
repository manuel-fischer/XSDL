#include "xsdl/xsdl.hpp"

#include <sstream>

#include <iostream> // TODO remove
namespace xsdl
{
    
    sdl_exception::sdl_exception(std::string msg): m_msg(msg) {}

    const char* sdl_exception::what() const noexcept
    {
        return m_msg.c_str();
    }

    void throw_sdl_exception(std::string_view context)
    {
        std::stringstream s;
        s << context << ": " << SDL_GetError();
        throw sdl_exception(std::move(s).str());
    }





    sdl::sdl(flags flags)
    {
        if(SDL_Init(flags) < 0) throw_sdl_exception("SDL_Init");
        active = true;
    }
    sdl::~sdl() noexcept { if(active) SDL_Quit(); }


    sdl_image::sdl_image(flags flags)
    {
        if(IMG_Init(flags) < 0) throw_sdl_exception("IMG_Init");
        active = true;
    }
    sdl_image::~sdl_image() noexcept { if(active) IMG_Quit(); }



    SDL_Point window::size() const noexcept
    {
        SDL_Point ret;
        SDL_GetWindowSize(ptr, &ret.x, &ret.y);
        return ret;
    }


    
    surface surface::load(const char* filename)
    {
        return surface(err_check(IMG_Load(filename), "IMG_Load"));
    }

    
    surface surface::from_data(size_t w, size_t h, const color* data)
    {
        return surface(err_check(SDL_CreateRGBSurfaceFrom(const_cast<color*>(data), w, h, 32, 4*w,
                                                          color::red_mask,
                                                          color::grn_mask,
                                                          color::blu_mask,
                                                          color::alp_mask), "SDL_CreateRGBSurfaceFrom"));
    }

    SDL_Point surface::size() const noexcept
    {
        return {ptr->w, ptr->h};
    }


    window::window(char* title, int x, int y, int w, int h, flags flags):
        ptr(err_check(SDL_CreateWindow(title, x, y, w, h, flags), "SDL_CreateWindow"))
    {}




    renderer::renderer(window& window, int index, flags flags):
        ptr(err_check(SDL_CreateRenderer(window.get(), index, flags), "SDL_CreateRenderer"))
    {}

    renderer::renderer(surface& surface):
        ptr(err_check(SDL_CreateSoftwareRenderer(surface.get()), "SDL_CreateSoftwareRenderer"))
    {}


    void renderer::draw_color(color color) {
        int_check(SDL_SetRenderDrawColor(ptr, color.r, color.g, color.b, color.a), "SDL_SetRenderDrawColor");
    }
    void renderer::clear() {
        int_check(SDL_RenderClear(ptr), "SDL_RenderClear");
    }
    void renderer::present() noexcept {
        SDL_RenderPresent(ptr);
    }
    painter renderer::paint(const texture& texture, std::optional<SDL_Rect> srcrect) {
        return {this, &texture, srcrect};
    }

    void renderer::fill(SDL_Rect rect)
    {
        SDL_RenderFillRect(get(), &rect);
    }

    void renderer::frame(SDL_Rect rect, int width)
    {
        SDL_Rect rects[4] {
            {rect.x, rect.y, rect.w, width},
            {rect.x, rect.y+rect.h-width, rect.w, width},
            {rect.x, rect.y+width, width, rect.h-2*width},
            {rect.x+rect.w-width, rect.y+width, width, rect.h-2*width},
        };
        SDL_RenderFillRects(get(), rects, 4);
    }

    painter painter::at(std::optional<SDL_Rect> dstrect)
    {
        int_check(SDL_RenderCopy(the_renderer->get(), the_texture->get_noconst(), opt2ptr(srcrect), opt2ptr(dstrect)), "SDL_RenderCopy");
        return *this;
    }

    painter painter::at(std::optional<SDL_Rect> dstrect, 
                        double angle,
                        std::optional<SDL_Point> center,
                        SDL_RendererFlip flip)
    {
        int_check(SDL_RenderCopyEx(the_renderer->get(), the_texture->get_noconst(), opt2ptr(srcrect), opt2ptr(dstrect), angle, opt2ptr(center), flip), "SDL_RenderCopyEx");
        return *this;
    }


    painter painter::atf(std::optional<SDL_FRect> dstrect)
    {
        int_check(SDL_RenderCopyF(the_renderer->get(), the_texture->get_noconst(), opt2ptr(srcrect), opt2ptr(dstrect)), "SDL_RenderCopy");
        return *this;
    }

    painter painter::atf(std::optional<SDL_FRect> dstrect, 
                         double angle,
                         std::optional<SDL_FPoint> center,
                         SDL_RendererFlip flip)
    {
        int_check(SDL_RenderCopyExF(the_renderer->get(), the_texture->get_noconst(), opt2ptr(srcrect), opt2ptr(dstrect), angle, opt2ptr(center), flip), "SDL_RenderCopyEx");
        return *this;
    }





    std::pair<window, renderer> create_window_and_renderer(int w, int h, flags flags, std::initializer_list<sdl_hint> hints)
    {
        for(sdl_hint hint : hints)
            SDL_SetHint(hint.name, hint.value);

        SDL_Window* window;
        SDL_Renderer* renderer;

        if(SDL_CreateWindowAndRenderer(w, h, flags, &window, &renderer) < 0)
            throw_sdl_exception("SDL_CreateWindowAndRenderer");
        
        return { window, renderer };
    }





    texture::texture(renderer& renderer, const surface& surface):
        texture(err_check(SDL_CreateTextureFromSurface(renderer.get(), surface.get_noconst()), "SDL_CreateTextureFromSurface"))
    {}
    
    SDL_Point texture::size() const noexcept
    {
        SDL_Point ret;
        SDL_QueryTexture(ptr, nullptr, nullptr, &ret.x, &ret.y);
        return ret;
    }

}