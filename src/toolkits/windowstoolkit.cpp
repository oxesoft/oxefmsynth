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

#include "editor.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "windowstoolkit.h"
#include "resources.h"
#include <windowsx.h>
#include <strsafe.h>
#include <stdio.h>

static int g_useCount = 0;
extern void* hInstance;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN64
    CWindowsToolkit* toolkit = (CWindowsToolkit*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
#else
    CWindowsToolkit* toolkit = (CWindowsToolkit*)GetWindowLong   (hWnd, GWL_USERDATA );
#endif
    if (!toolkit)
    {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    switch (message)
    {
    case WM_LBUTTONDBLCLK:
    {
        toolkit->editor->OnLButtonDblClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        toolkit->editor->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        SetFocus(hWnd);
        return 0;
    }
    case WM_LBUTTONUP:
    {
        toolkit->editor->OnLButtonUp();
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
        if (toolkit->editor->OnChar(text[0]) == true)
            return 0;
        else if (toolkit->parentWindow)
            PostMessage(GetParent((HWND)toolkit->parentWindow), message, wParam, lParam);
        // ---------------------------------------
        break;
    }
    case WM_MOUSEMOVE:
    {
        toolkit->editor->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
            toolkit->editor->OnMouseWheel(point.x, point.y, zDelta);
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
        BitBlt(dc, rect->left, rect->top, w, h, toolkit->hdcMem, rect->left, rect->top, SRCCOPY);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_TIMER:
    {
        toolkit->editor->Update();
        return 0;
    }
    case WM_DESTROY:
    {
        if (!toolkit->parentWindow)
        {
            PostQuitMessage(0);
        }
        return 0L;
    }
    default:
        break;
    }
    return DefWindowProc (hWnd, message, wParam, lParam);
}

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
    StringCchCatA(path, MAX_PATH, "\\"BMP_PATH);
}

CWindowsToolkit::CWindowsToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow  = parentWindow;
    this->editor        = editor;

    g_useCount++;
    if (g_useCount == 1)
    {
        WNDCLASSW windowClass;
        windowClass.style         = CS_DBLCLKS;
        windowClass.lpfnWndProc   = WindowProc;
        windowClass.cbClsExtra    = 0;
        windowClass.cbWndExtra    = 0;
        windowClass.hInstance     = (HINSTANCE)hInstance;
        windowClass.hIcon         = LoadIcon((HINSTANCE)hInstance,MAKEINTRESOURCE(IDI_ICON));
        windowClass.hCursor       = LoadCursor( NULL, IDC_ARROW );
        windowClass.hbrBackground = NULL;
        windowClass.lpszMenuName  = 0;
        windowClass.lpszClassName = L"OxeVstEditorClass";
        RegisterClassW(&windowClass);
    }

    if (!parentWindow)
    {
        RECT rect;
        rect.left   = 100;
        rect.top    = 100;
        rect.right  = GUI_WIDTH;
        rect.bottom = GUI_HEIGHT;
        AdjustWindowRect(&rect, WS_SYSMENU | WS_CAPTION, FALSE);
        rect.bottom += GetSystemMetrics(SM_CYCAPTION);
        rect.bottom += GetSystemMetrics(SM_CYFIXEDFRAME);
        rect.right  += GetSystemMetrics(SM_CXFIXEDFRAME);

        this->hWnd = CreateWindowW
        (
            L"OxeVstEditorClass",
            L"",
            WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
            rect.left,
            rect.top,
            rect.right,
            rect.bottom,
            0,
            0,
            (HINSTANCE)hInstance,
            NULL
        );
    }
    else
    {
        this->hWnd = CreateWindowExW
        (
            0,
            L"OxeVstEditorClass",
            L"",
            WS_CHILD | WS_VISIBLE,
            0,
            0,
            GUI_WIDTH,
            GUI_HEIGHT,
            (HWND)parentWindow,
            NULL,
            (HINSTANCE)hInstance,
            NULL
        );
    }

#ifdef _WIN64
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);
#else
    SetWindowLong(hWnd, GWL_USERDATA, (LONG)this);
#endif

    // load resources
    char path[MAX_PATH];
    GetResourcesPath(path, MAX_PATH);

    char fullPath[MAX_PATH];
    StringCchPrintf(fullPath, MAX_PATH, "%s\\%s", path, "chars.bmp");
    bmps[BMP_CHARS]   = (HBITMAP)LoadImageA(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchPrintf(fullPath, MAX_PATH, "%s\\%s", path, "knob.bmp");
    bmps[BMP_KNOB]    = (HBITMAP)LoadImageA(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchPrintf(fullPath, MAX_PATH, "%s\\%s", path, "knob2.bmp");
    bmps[BMP_KNOB2]   = (HBITMAP)LoadImageA(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchPrintf(fullPath, MAX_PATH, "%s\\%s", path, "knob3.bmp");
    bmps[BMP_KNOB3]   = (HBITMAP)LoadImageA(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchPrintf(fullPath, MAX_PATH, "%s\\%s", path, "key.bmp");
    bmps[BMP_KEY]     = (HBITMAP)LoadImageA(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchPrintf(fullPath, MAX_PATH, "%s\\%s", path, "bg.bmp");
    bmps[BMP_BG]      = (HBITMAP)LoadImageA(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchPrintf(fullPath, MAX_PATH, "%s\\%s", path, "buttons.bmp");
    bmps[BMP_BUTTONS] = (HBITMAP)LoadImageA(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    StringCchPrintf(fullPath, MAX_PATH, "%s\\%s", path, "ops.bmp");
    bmps[BMP_OPS]     = (HBITMAP)LoadImageA(NULL, fullPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    if (!bmps[BMP_CHARS  ]) bmps[BMP_CHARS  ] = LoadBitmap((HINSTANCE)hInstance,MAKEINTRESOURCE(IDB_CHARS));
    if (!bmps[BMP_KNOB   ]) bmps[BMP_KNOB   ] = LoadBitmap((HINSTANCE)hInstance,MAKEINTRESOURCE(IDB_KNOB));
    if (!bmps[BMP_KNOB2  ]) bmps[BMP_KNOB2  ] = LoadBitmap((HINSTANCE)hInstance,MAKEINTRESOURCE(IDB_KNOB2));
    if (!bmps[BMP_KNOB3  ]) bmps[BMP_KNOB3  ] = LoadBitmap((HINSTANCE)hInstance,MAKEINTRESOURCE(IDB_KNOB3));
    if (!bmps[BMP_KEY    ]) bmps[BMP_KEY    ] = LoadBitmap((HINSTANCE)hInstance,MAKEINTRESOURCE(IDB_CHAVE));
    if (!bmps[BMP_BG     ]) bmps[BMP_BG     ] = LoadBitmap((HINSTANCE)hInstance,MAKEINTRESOURCE(IDB_FUNDO));
    if (!bmps[BMP_BUTTONS]) bmps[BMP_BUTTONS] = LoadBitmap((HINSTANCE)hInstance,MAKEINTRESOURCE(IDB_BUTTONS));
    if (!bmps[BMP_OPS    ]) bmps[BMP_OPS    ] = LoadBitmap((HINSTANCE)hInstance,MAKEINTRESOURCE(IDB_OPS));
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
    g_useCount--;
    if (g_useCount == 0)
    {
        UnregisterClassW(L"OxeVstEditorClass", (HINSTANCE)hInstance);
    }
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

void CWindowsToolkit::StartWindowProcesses()
{
    if (!parentWindow)
    {
        ShowWindow(this->hWnd, SW_SHOWNORMAL);
    }
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

void CWindowsToolkit::StartMouseCapture()
{
    SetCapture(this->hWnd);
}

void CWindowsToolkit::StopMouseCapture()
{
    ReleaseCapture();
}

void CWindowsToolkit::Debug(char *text)
{
    OutputDebugStringA(text);
    printf("%s\n", text);
}

int CWindowsToolkit::WaitWindowClosed()
{
    MSG msg;
    while( GetMessage( &msg, NULL, 0, 0 ) )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    return (int)msg.wParam;
}
