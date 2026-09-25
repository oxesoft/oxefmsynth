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
#include "font_data.h"

class CFontManager
{
public:
    static BLFontFace& GetFace()
    {
        static BLFontFace face;
        static bool loaded = false;
        if (!loaded)
        {
            BLFontData fontData;
            if (fontData.create_from_data(kFontRegularData, kFontRegularSize) == BL_SUCCESS)
            {
                face.create_from_data(fontData, 0);
            }
            loaded = true;
        }
        return face;
    }

    static BLFontFace& GetBoldFace()
    {
        static BLFontFace face;
        static bool loaded = false;
        if (!loaded)
        {
            BLFontData fontData;
            if (fontData.create_from_data(kFontBoldData, kFontBoldSize) == BL_SUCCESS)
            {
                face.create_from_data(fontData, 0);
            }
            if (!face.is_valid())
            {
                face = GetFace();
            }
            loaded = true;
        }
        return face;
    }

    static BLFont GetFont(float size)
    {
        BLFont font;
        BLFontFace& face = GetFace();
        font.create_from_face(face, size);
        return font;
    }

    static BLFont GetFontBold(float size)
    {
        BLFont font;
        BLFontFace& face = GetBoldFace();
        font.create_from_face(face, size);
        return font;
    }
};
