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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <stdio.h>
#include "editor.h"
#include "windowstoolkit.h"
#include "resource.h"

#define TIMER_RESOLUTION_MS 20
#define BMP_PATH "skin\\"

void GetResourcesPath(char *path, int size)
{
    ZeroMemory(path, size);
    HMODULE hm = NULL;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) &GetResourcesPath, &hm))
    {
        return;
    }
    DWORD x = GetModuleFileNameA(hm, path, size);
    if (!x)
        return;
    int p = lstrlenA(path);
    if (!p)
        return;
    while (p--)
    {
        if (path[p] == '\\')
        {
            path[p+1] = 0;
            break;
        }
    }
}

CWindowsToolkit::CWindowsToolkit(HINSTANCE hInstance, HWND hWnd)
{
    this->hWnd = hWnd;
    // load resources
    char dll_path[MAX_PATH];
    char tmp_path[MAX_PATH];
    GetResourcesPath(dll_path, MAX_PATH);
    StringCchCopyA(tmp_path, MAX_PATH, dll_path);
    StringCchCatA (tmp_path, MAX_PATH, BMP_PATH"chars.bmp");
    bmps[BMP_CHARS]   = (HBITMAP)LoadImageA(NULL, tmp_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchCopyA(tmp_path, MAX_PATH, dll_path);
    StringCchCatA (tmp_path, MAX_PATH, BMP_PATH"knob.bmp");
    bmps[BMP_KNOB]    = (HBITMAP)LoadImageA(NULL, tmp_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchCopyA(tmp_path, MAX_PATH, dll_path);
    StringCchCatA (tmp_path, MAX_PATH, BMP_PATH"knob2.bmp");
    bmps[BMP_KNOB2]   = (HBITMAP)LoadImageA(NULL, tmp_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchCopyA(tmp_path, MAX_PATH, dll_path);
    StringCchCatA (tmp_path, MAX_PATH, BMP_PATH"knob3.bmp");
    bmps[BMP_KNOB3]   = (HBITMAP)LoadImageA(NULL, tmp_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchCopyA(tmp_path, MAX_PATH, dll_path);
    StringCchCatA (tmp_path, MAX_PATH, BMP_PATH"key.bmp");
    bmps[BMP_KEY]     = (HBITMAP)LoadImageA(NULL, tmp_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchCopyA(tmp_path, MAX_PATH, dll_path);
    StringCchCatA (tmp_path, MAX_PATH, BMP_PATH"bg.bmp");
    bmps[BMP_BG]      = (HBITMAP)LoadImageA(NULL, tmp_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchCopyA(tmp_path, MAX_PATH, dll_path);
    StringCchCatA (tmp_path, MAX_PATH, BMP_PATH"buttons.bmp");
    bmps[BMP_BUTTONS] = (HBITMAP)LoadImageA(NULL, tmp_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchCopyA(tmp_path, MAX_PATH, dll_path);
    StringCchCatA (tmp_path, MAX_PATH, BMP_PATH"ops.bmp");
    bmps[BMP_OPS]     = (HBITMAP)LoadImageA(NULL, tmp_path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!bmps[BMP_CHARS  ]) bmps[BMP_CHARS  ] = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_CARACTERES));
    if (!bmps[BMP_KNOB   ]) bmps[BMP_KNOB   ] = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_KNOB));
    if (!bmps[BMP_KNOB2  ]) bmps[BMP_KNOB2  ] = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_KNOB2));
    if (!bmps[BMP_KNOB3  ]) bmps[BMP_KNOB3  ] = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_KNOB3));
    if (!bmps[BMP_KEY    ]) bmps[BMP_KEY    ] = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_CHAVE));
    if (!bmps[BMP_BG     ]) bmps[BMP_BG     ] = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_FUNDO));
    if (!bmps[BMP_BUTTONS]) bmps[BMP_BUTTONS] = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BUTTONS));
    if (!bmps[BMP_OPS    ]) bmps[BMP_OPS    ] = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_OPS));
    // create offscreen buffer
    hdc = GetDC(hWnd); 
    hdcMem = CreateCompatibleDC(hdc);
    hdcAux = CreateCompatibleDC(hdc);
    bitmap = CreateCompatibleBitmap(hdc, GUI_WIDTH, GUI_HEIGHT);
    SelectObject(hdcMem, bitmap);
    SetTimer(hWnd, 0, TIMER_RESOLUTION_MS, NULL);
}

CWindowsToolkit::~CWindowsToolkit()
{
    KillTimer(hWnd, 0);
    DeleteDC(hdcAux);
    DeleteDC(hdcMem);
    DeleteObject(bitmap);
    ReleaseDC(hWnd, hdc);
    // unload resources
    DeleteObject(bmps[BMP_CHARS  ]);
    DeleteObject(bmps[BMP_KNOB   ]);
    DeleteObject(bmps[BMP_KNOB2  ]);
    DeleteObject(bmps[BMP_KNOB3  ]);
    DeleteObject(bmps[BMP_KEY    ]);
    DeleteObject(bmps[BMP_BG     ]);
    DeleteObject(bmps[BMP_BUTTONS]);
    DeleteObject(bmps[BMP_OPS    ]);
}

void CWindowsToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    RECT rect;
    int prevDC = SaveDC(hdcAux);
    SelectObject(hdcAux, bmps[origBmp]);
    BitBlt(hdcMem, destX, destY, width, height, hdcAux, origX, origY, SRCCOPY);
    RestoreDC(hdcAux, prevDC);
    rect.left   = destX;
    rect.top    = destY;
    rect.right  = destX + width;
    rect.bottom = destY + height;
    InvalidateRect(this->hWnd, &rect, FALSE);
}

void CWindowsToolkit::SendMessageToHost(unsigned int messageID, unsigned int par1, unsigned int par2)
{
    PostMessage(this->hWnd, WM_USER + messageID, par1, par2);
}

void CWindowsToolkit::GetMousePosition(int *x, int *y)
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(this->hWnd, &point);
    *x = point.x;
    *y = point.y;
}

void CWindowsToolkit::StartMouseCapture()
{
    SetCapture(this->hWnd);
}

void CWindowsToolkit::StopMouseCapture()
{
    ReleaseCapture();
}

void CWindowsToolkit::OutputDebugString(char *text)
{
    OutputDebugStringA(text);
    printf("%s\n", text);
}

void *CWindowsToolkit::GetImageBuffer()
{
    return hdcMem;
}

int CWindowsToolkit::CommonWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, CEditor *editor, HWND systemWindow)
{
    if (!editor)
    {
        return -1;
    }
    switch (message)
    {
    case WM_LBUTTONDBLCLK:
    {
        editor->OnLButtonDblClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        editor->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        SetFocus(hWnd);
        return 0;
    }
    case WM_LBUTTONUP:
    {
        editor->OnLButtonUp();
        SetFocus(hWnd);
        return 0;
    }
    case WM_KEYDOWN:
    {
        // from juce_win32_Windowing.cpp:doKeyDown
        const UINT keyChar  = MapVirtualKey ((UINT) wParam, 2);
        const UINT scanCode = MapVirtualKey ((UINT) wParam, 0);
        BYTE keyState[256];
        GetKeyboardState (keyState);
        
        WCHAR text[16] = { 0 };
        if (ToUnicode ((UINT) wParam, scanCode, keyState, text, 8, 0) != 1)
            text[0] = 0;
        if (editor->OnChar(text[0]) == true)
            return 0;
        else if (systemWindow)
            PostMessage(GetParent(systemWindow), message, wParam, lParam);
        // ---------------------------------------
        break;
    }
    case WM_MOUSEMOVE:
    {
        editor->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        RECT rect;
        POINT point;
        point.x = GET_X_LPARAM(lParam); 
        point.y = GET_Y_LPARAM(lParam);
        
        GetWindowRect(hWnd, &rect);
        GetCursorPos(&point);
        if (PtInRect(&rect, point))
        {
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            zDelta /= WHEEL_DELTA;
            ScreenToClient(hWnd, &point);
            editor->OnMouseWheel(point.x, point.y, zDelta);
            SetFocus(hWnd);
            return 0;
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hWnd, &ps);
        RECT *rect = &ps.rcPaint;
        int w = rect->right  - rect->left;
        int h = rect->bottom - rect->top;
        BitBlt(dc, rect->left, rect->top, w, h, (HDC)editor->GetToolkit()->GetImageBuffer(), rect->left, rect->top, SRCCOPY);
        EndPaint(hWnd, &ps); 
        return 0;
    }
    case WM_TIMER:
    {
        editor->Update();
        return 0;
    }
    default:
        break;
    }
    return -1;
}
