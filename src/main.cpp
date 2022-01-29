#include <cstdlib>
#include <iostream>
#include <exception>
#include <cmath>

#include "xsdl/xsdl.hpp"
#include "xsdl/xsdl_gl.hpp"
#include "xsdl/xsdl_ttf.hpp"
#include "xsdl/xsdl_shapes.hpp"

extern"C" int main(int argc, char** argv)
{
    try
    {
        xsdl::sdl sdl(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        xsdl::sdl_image sdl_image(IMG_INIT_JPG | IMG_INIT_PNG);
        xsdl::sdl_ttf sdl_ttf;

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); // enable bilinear interpolation
        //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // enable anti aliasing

        auto[window, renderer] = xsdl::create_window_and_renderer(
            600, 400, 
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE //| SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP 
            //SDL_WINDOW_ALLOW_HIGHDPI
        ); // | SDL_WINDOW_OPENGL);
        SDL_SetWindowTitle(window.get(), "XSDL Program");
        //SDL_SetWindowOpacity(window.get(), 0.5f);

        //auto gl_ctx = xsdl::gl_context::make_common(window);

        auto font = xsdl::font::load("fonts/DejaVuSans.ttf", 30);


        auto tex = xsdl::texture(renderer, xsdl::surface::load("circle.png"));
        auto txt1 = xsdl::texture(renderer, font.blended("Hello World", {  0,  0,  0}));
        auto txt2 = xsdl::texture(renderer, font.blended("Hello World", {255,255,255}));
        
        auto[txtw, txth] = txt1.size();

        //auto pen = xsdl::pen::create_pen(renderer, {255, 255, 255, 128}, 10);
        //auto pen = xsdl::pen::create_pen(renderer, {255, 240, 128, 255}, 100, true);
        //auto pen = xsdl::pen::create_smooth_pen(renderer, {128, 240, 255, 255}, 100, true);
        auto pen = xsdl::pen::create_smooth_pen(renderer, {128, 240, 255, 255}, 100);
        //SDL_SetTextureBlendMode(pen.line_crosssection.get(), SDL_BLENDMODE_MUL); // premultiply = true!
        //SDL_SetTextureBlendMode(pen.line_crosssection.get(), SDL_BLENDMODE_ADD);
        //SDL_SetTextureBlendMode(pen.line_crosssection.get(), SDL_BLENDMODE_MOD);


        int linex=200, liney=300;

        int lastx, lasty;
        bool mousedown = false;

        float rotation = 45;

        while(true)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event) == 1)
            {
                if(event.type == SDL_QUIT) return 0;
                if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return 0;

                if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
                {
                    int x, y;
                    SDL_GetWindowPosition(window.get(), &x, &y);
                    lastx = event.button.x + x;
                    lasty = event.button.y + y;
                    mousedown = true;
                    
                    linex=event.button.x, liney=event.button.y;
                }
                if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
                {
                    mousedown = false;
                }
            }
                rotation+=0.01;


            if(0&&mousedown)
            {
                // SDL_MOUSEMOTION event cannot be used, because it does not occurr, when the mouse is outside the window
                int mx, my;
                SDL_GetGlobalMouseState(&mx, &my);

                int x, y;
                SDL_GetWindowPosition(window.get(), &x, &y);
                x+=mx-lastx;
                y+=my-lasty;
                lastx = mx;
                lasty = my;
                SDL_SetWindowPosition(window.get(), x, y);
            }


            auto[w, h] = window.size();
            /*glViewport(0, 0, w, h);

            glClearColor(0.1, 0.1, 0.1, 0.1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            SDL_GL_SwapWindow(window.get());*/

            
            renderer.draw_color({0x22, 0x22, 0xff, 0xff});
            //renderer.draw_color({255, 255, 255, 255});
            renderer.clear();

            //renderer.paint(tex).at();

            SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);

            renderer.draw_color({0xff, 0x00, 0x00, 0xc0});
            SDL_Rect r {w/3, h/3, w/3, h/3};
            SDL_RenderFillRect(renderer.get(), &r);


            renderer.paint(txt1).atf(SDL_FRect{(w-txtw)/2+sin(rotation/180*M_PI), (h-txth)/2, txtw, txth});
            renderer.paint(txt2).atf(SDL_FRect{(w-txtw)/2-2, (h-txth)/2-2, txtw, txth}, rotation);

            //SDL_RenderDrawLineF(renderer.get(), 100, 100, 500, 300);

            //draw_line(renderer, 100, 100, linex, liney);
            pen.draw_line(100, 100, linex, liney);


            renderer.present();
        }


    }
    catch(const std::exception& e)
    {
        std::cout << "Exception ocurred: " << e.what() << '\n';
    }

    return 0;
}