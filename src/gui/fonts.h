/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2026  Daniel Moura <oxe@oxesoft.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <blend2d/blend2d.h>

class CFontManager
{
public:
    static BLFontFace& GetFace()
    {
        static BLFontFace face;
        static bool loaded = false;
        if (!loaded)
        {
            const char* paths[] = {
#if defined(__APPLE__)
                "/System/Library/Fonts/SFNS.ttf",
                "/System/Library/Fonts/Helvetica.ttc",
                "/System/Library/Fonts/Supplemental/Arial.ttf",
                "/Library/Fonts/Arial.ttf"
#elif defined(_WIN32)
                "C:\\Windows\\Fonts\\segoeui.ttf",
                "C:\\Windows\\Fonts\\arial.ttf"
#else
                "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"
#endif
            };
            for (const char* p : paths)
            {
                if (face.create_from_file(p) == BL_SUCCESS)
                {
                    loaded = true;
                    break;
                }
            }
            if (!loaded)
            {
                loaded = true;
            }
        }
        return face;
    }

    static BLFontFace& GetBoldFace()
    {
        static BLFontFace face;
        static bool loaded = false;
        if (!loaded)
        {
            const char* paths[] = {
#if defined(__APPLE__)
                "/System/Library/Fonts/Supplemental/Trebuchet MS Bold.ttf",
                "/System/Library/Fonts/Supplemental/Arial Bold.ttf",
                "/System/Library/Fonts/Supplemental/Tahoma Bold.ttf",
                "/Library/Fonts/Arial Bold.ttf"
#elif defined(_WIN32)
                "C:\\Windows\\Fonts\\trebucbd.ttf",
                "C:\\Windows\\Fonts\\arialbd.ttf",
                "C:\\Windows\\Fonts\\tahomabd.ttf",
                "C:\\Windows\\Fonts\\segoeuib.ttf"
#else
                "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
                "/usr/share/fonts/truetype/freefont/FreeSansBold.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf"
#endif
            };
            for (const char* p : paths)
            {
                if (face.create_from_file(p) == BL_SUCCESS)
                {
                    loaded = true;
                    break;
                }
            }
            if (!loaded)
            {
                face = GetFace();
                loaded = true;
            }
        }
        return face;
    }

    static BLFont GetFont(float size)
    {
        BLFont font;
        BLFontFace& face = GetFace();
        if (face.is_valid())
        {
            font.create_from_face(face, size);
        }
        return font;
    }

    static BLFont GetFontBold(float size)
    {
        BLFont font;
        BLFontFace& face = GetBoldFace();
        if (face.is_valid())
        {
            font.create_from_face(face, size);
        }
        return font;
    }
};
