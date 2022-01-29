#pragma once

#include <cstdint>

namespace xsdl
{
    struct color
    {
        uint8_t r, g, b, a;
        

        constexpr color operator*(color o) const noexcept
        {
            return {uint8_t((int)r*o.r / 255),
                    uint8_t((int)g*o.g / 255),
                    uint8_t((int)b*o.b / 255),
                    uint8_t((int)a*o.a / 255)};
        }
        
        constexpr color& operator*=(color o) noexcept
        {
            return *this = *this * o;
        }

        constexpr color operator|(color o) const noexcept
        {
            int src_alp = o.a;
            int dst_alp = a;
            int r_alp = src_alp + (int)(255^src_alp)*dst_alp/255;
            if(r_alp == 0) return *this;

            int r_red = (src_alp*255*o.r + (255^src_alp)*dst_alp*r)/(255*r_alp);
            int r_grn = (src_alp*255*o.g + (255^src_alp)*dst_alp*g)/(255*r_alp);
            int r_blu = (src_alp*255*o.b + (255^src_alp)*dst_alp*b)/(255*r_alp);
            return {uint8_t(r_red), uint8_t(r_grn), uint8_t(r_blu), uint8_t(r_alp)};
        }
        
        constexpr color& operator|=(color o) noexcept
        {
            return *this = *this | o;
        }

        operator SDL_Color() const noexcept
        {
            return {r, g, b, a};
        }


        /*explicit operator uint32_t()
        {
            return std::bit_cast
        }*/

        constexpr static uint32_t red_mask = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 0x000000ff : 0xff000000;
        constexpr static uint32_t grn_mask = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 0x0000ff00 : 0x00ff0000;
        constexpr static uint32_t blu_mask = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 0x00ff0000 : 0x0000ff00;
        constexpr static uint32_t alp_mask = __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ? 0xff000000 : 0x000000ff;


        constexpr color premultiply() const noexcept
        {
            return {uint8_t((int)r*a / 255),
                    uint8_t((int)g*a / 255),
                    uint8_t((int)b*a / 255),
                    a};
        }

        constexpr color premultiply_if(bool cond) const noexcept
        {
            return cond ? premultiply() : *this; 
        }
    };


}