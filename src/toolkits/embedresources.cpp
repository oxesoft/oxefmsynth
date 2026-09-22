/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2015  Daniel Moura <oxe@oxesoft.com>

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

#include <stdio.h>
#include <dirent.h>

#define BMP_PATH "skin/"
#define PATH     "skins/default/"

void append_file(FILE *fout, const char *path, const char *name)
{
    FILE *f = fopen(path, "rb");
    unsigned char c;
    unsigned int i = 0;
    size_t n;
    fprintf(fout, "const unsigned char _%s[] = {\n", name);
    while (!feof(f))
    {
        n=fread(&c, 1, 1, f);
        fprintf(fout, "0x%02X%s%s", c, !feof(f) ? "," : "", ++i % 16 ? "" : "\n");
    }
    fprintf(fout, "};\n");
    fprintf(fout, "const char *%s = (const char *)_%s;\n", name, name);
    fclose(f);
}

int main(int argc, char *argv[])
{
    if (argc != 2 || argv[1][0] == 0)
    {
        return 1;
    }
    FILE *f = fopen(argv[1], "wb");
    if (f == NULL)
    {
        return 2;
    }
    DIR *dir = opendir(BMP_PATH);
    if(dir != NULL)
    {
        closedir(dir);
        fprintf(stderr, "Generating %s using %s\n",argv[1],BMP_PATH);
        append_file(f, BMP_PATH"bg.bmp"     , "bg_bmp"     );
        append_file(f, BMP_PATH"buttons.bmp", "buttons_bmp");
        append_file(f, BMP_PATH"chars.bmp"  , "chars_bmp"  );
        append_file(f, BMP_PATH"key.bmp"    , "key_bmp"    );
        append_file(f, BMP_PATH"knob2.bmp"  , "knob2_bmp"  );
        append_file(f, BMP_PATH"knob3.bmp"  , "knob3_bmp"  );
        append_file(f, BMP_PATH"knob.bmp"   , "knob_bmp"   );
        append_file(f, BMP_PATH"ops.bmp"    , "ops_bmp"    );
    }
    else
    {
        fprintf(stderr, "Generating %s using %s\n",argv[1],PATH);
        append_file(f, PATH"bg.bmp"     , "bg_bmp"     );
        append_file(f, PATH"buttons.bmp", "buttons_bmp");
        append_file(f, PATH"chars.bmp"  , "chars_bmp"  );
        append_file(f, PATH"key.bmp"    , "key_bmp"    );
        append_file(f, PATH"knob2.bmp"  , "knob2_bmp"  );
        append_file(f, PATH"knob3.bmp"  , "knob3_bmp"  );
        append_file(f, PATH"knob.bmp"   , "knob_bmp"   );
        append_file(f, PATH"ops.bmp"    , "ops_bmp"    );
    }
    fclose(f);
    return 0;
}
