#include "gnote/app.hpp"

extern"C" int main(int argc, char** argv)
{
    try
    {
        xsdl::sdl sdl{SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS};
        xsdl::sdl_image sdl_image{IMG_INIT_JPG | IMG_INIT_PNG};
        xsdl::sdl_ttf sdl_ttf{};

        gnote::app app{};
        app_loop(app);
    }
    catch(const std::exception& e)
    {
        std::cout << "Exception ocurred: " << e.what() << '\n';
    }

    return 0;
}