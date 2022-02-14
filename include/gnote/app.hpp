#pragma once

#include <cstdlib>
#include <iostream>
#include <exception>
#include <cmath>

#include "xgui/xgui.hpp"

namespace gnote
{
    struct app_style : xgui::stylesheet
    {
        xsdl::font button_font = xsdl::font::load("fonts/DejaVuSans.ttf", 14);
        xsdl::font mono_font = xsdl::font::load("fonts/DejaVuSansMono.ttf", 14);
        xsdl::font big_font = xsdl::font::load("fonts/DejaVuSans.ttf", 30);

        xgui::button_style button_style
        {
            .font = &button_font,
            .foreground         = {255, 255, 255, 255},
            .background         = { 50,  50,  50, 255},
            .background_hover   = { 70,  70,  70, 255},
            .background_down    = { 20,  20,  20, 255},
        };
        xgui::flex_style button_flex = { xgui::fixed, xgui::fixed };


        xgui::button_style button_style_highlight
        {
            .font = &button_font,
            .foreground         = {255, 255, 255, 255},
            .background         = { 30,  80, 160, 255},
            .background_hover   = { 40, 120, 200, 255},
            .background_down    = { 10,  20,  30, 255},
        };

        xgui::label_style big_label_style
        {
            .font = &big_font,
            .foreground = {255, 255, 255, 255},
        };
        xgui::flex_style label_flex = { 100, xgui::fixed };

        xgui::textbox_style textbox_style =
        {
            .font = &mono_font,
            .foreground = {255, 255, 255, 255},
            .background = { 20,  20,  20, 255},
            .padding = { .left = 5 },
        };

        xgui::composite_style main_style
        {
            .layout = xgui::composite_layout::vertical,
            .padding =
            {
                .left = 10,
                .top = 10,
                .right = 10,
                .bottom = 10,
                .vertical = 10,
            },
        };
        xgui::flex_style main_flex = { 100, 100 };


        xgui::composite_style bar_style
        {
            .layout = xgui::composite_layout::horizontal,
            .padding =
            {
                .horizontal = 10,
            },
        };
        xgui::flex_style bar_flex = { 100, xgui::fixed };

        xgui::flex_style spacer_flex = { 100, 100 };
    };

    struct app_ui : xgui::composite
    {
        app_ui(composite* parent):
            composite{ parent, {}, &app_style::main_flex, &app_style::main_style }
        {}


        xgui::label   lblTitle   = { this, {.h = 50}, "Sample program",   &app_style::label_flex,    &app_style::big_label_style };
        xgui::textbox txtTextbox = { this, {},        "Sample Text",      &app_style::textbox_style, &app_style::textbox_style };

        xgui::composite button_bar = { this, {.h = 30}, &app_style::bar_flex, &app_style::bar_style };
        xgui::button  cmdHelp   = { &button_bar, {.w = 130, .h = 30}, "Help",   &app_style::button_flex, &app_style::button_style };
        xgui::spacer  spcSpacer = { &button_bar, {}, &app_style::spacer_flex };
        xgui::button  cmdOK     = { &button_bar, {.w = 130, .h = 30}, "OK",     &app_style::button_flex, &app_style::button_style_highlight };
        xgui::button  cmdCancel = { &button_bar, {.w = 130, .h = 30}, "Cancel", &app_style::button_flex, &app_style::button_style };
    };
    
    struct app_base
    {
        xsdl::window window;
        xsdl::renderer renderer;

        app_base(std::tuple<xsdl::window, xsdl::renderer> r):
            window(std::move(std::get<0>(r))),
            renderer(std::move(std::get<1>(r)))
        {}
    };

    struct app : app_base
    {
        app():
            app_base(xsdl::create_window_and_renderer(
                600, 400, 
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
                {
                    {SDL_HINT_RENDER_SCALE_QUALITY, "1"},
                    //{SDL_HINT_EVENT_LOGGING, "1"},
                }
            ))
        {
            SDL_SetWindowTitle(window.get(), "XSDL Program");
        }


        app_style styles {};
        app_ui ui { nullptr };


    };


    void app_loop(app& the_app)
    {

        auto[w, h] = the_app.window.size();
        the_app.ui.update_layout({0, 0, w, h}, &the_app.styles);
        
        while(true)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event) == 1)
            {
                if(event.type == SDL_QUIT) return;
                if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) return;
                if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    auto[w, h] = the_app.window.size();
                    the_app.ui.update_layout({0, 0, w, h}, &the_app.styles);
                }
                
                the_app.ui.on_event(event);
            }


            auto[w, h] = the_app.window.size();
            
            the_app.renderer.draw_color({30, 30, 30, 0xff});
            the_app.renderer.clear();

            SDL_SetRenderDrawBlendMode(the_app.renderer.get(), SDL_BLENDMODE_BLEND);

            the_app.ui.render(the_app.renderer, &the_app.styles);

            the_app.renderer.present();
        }
    }
}