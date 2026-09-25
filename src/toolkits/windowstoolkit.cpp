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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>
#include <stdio.h>
#include <math.h>
#include "editor.h"
#include "windowstoolkit.h"
#include "resources.h"

static int g_useCount = 0;
extern void* hInstance;

static void GetMouseScaled(HWND hWnd, LPARAM lParam, int &outX, int &outY)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    int cw = rc.right - rc.left;
    int ch = rc.bottom - rc.top;
    if (cw <= 0 || ch <= 0)
    {
        outX = GET_X_LPARAM(lParam);
        outY = GET_Y_LPARAM(lParam);
        return;
    }
    float sx = (float)cw / (float)GUI_WIDTH;
    float sy = (float)ch / (float)GUI_HEIGHT;
    outX = (int)(GET_X_LPARAM(lParam) / sx);
    outY = (int)(GET_Y_LPARAM(lParam) / sy);
}

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
        int mx, my;
        GetMouseScaled(hWnd, lParam, mx, my);
        toolkit->editor->OnLButtonDblClick(mx, my);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        int mx, my;
        GetMouseScaled(hWnd, lParam, mx, my);
        toolkit->editor->OnLButtonDown(mx, my);
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
        const UINT scanCode = MapVirtualKey((UINT)wParam, 0);
        BYTE keyState[256];
        GetKeyboardState(keyState);

        WCHAR text[16] = { 0 };
        if (ToUnicode((UINT)wParam, scanCode, keyState, text, 8, 0) != 1)
            text[0] = 0;
        if (toolkit->editor->OnChar((char)text[0]) == true)
            return 0;
        else if (toolkit->parentWindow)
            PostMessage(GetParent((HWND)toolkit->parentWindow), message, wParam, lParam);
        break;
    }
    case WM_MOUSEMOVE:
    {
        int mx, my;
        GetMouseScaled(hWnd, lParam, mx, my);
        toolkit->editor->OnMouseMove(mx, my);
        return 0;
    }
    case WM_MOUSEWHEEL:
    {
        POINT point;
        point.x = GET_X_LPARAM(lParam);
        point.y = GET_Y_LPARAM(lParam);

        RECT rect;
        GetWindowRect(hWnd, &rect);
        if (PtInRect(&rect, point))
        {
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            ScreenToClient(hWnd, &point);
            RECT rc;
            GetClientRect(hWnd, &rc);
            int cw = rc.right - rc.left;
            int ch = rc.bottom - rc.top;
            float sx = cw > 0 ? (float)cw / (float)GUI_WIDTH : 1.0f;
            float sy = ch > 0 ? (float)ch / (float)GUI_HEIGHT : 1.0f;
            int mx = (int)(point.x / sx);
            int my = (int)(point.y / sy);
            toolkit->editor->OnMouseWheel(mx, my, zDelta);
            SetFocus(hWnd);
            return 0;
        }
        break;
    }
    case WM_SIZING:
    {
        if (!toolkit->parentWindow)
        {
            RECT *r = (RECT*)lParam;
            int w = r->right - r->left;
            int h = (int)(w * ((float)GUI_HEIGHT / (float)GUI_WIDTH));
            r->bottom = r->top + h;
            return TRUE;
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hWnd, &ps);
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int cw = clientRect.right - clientRect.left;
        int ch = clientRect.bottom - clientRect.top;
        if (cw > 0 && ch > 0 && toolkit->editor)
        {
            BLImage blImg(cw, ch, BL_FORMAT_PRGB32);
            BLContext ctx(blImg);
            ctx.clear_all();
            double sx = (double)cw / (double)GUI_WIDTH;
            double sy = (double)ch / (double)GUI_HEIGHT;
            ctx.scale(sx, sy);
            toolkit->editor->Paint(ctx);
            ctx.end();

            BLImageData imgData;
            blImg.get_data(&imgData);

            BITMAPINFO bmi;
            ZeroMemory(&bmi, sizeof(BITMAPINFO));
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = cw;
            bmi.bmiHeader.biHeight = -ch; // top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;

            SetStretchBltMode(dc, COLORONCOLOR);
            StretchDIBits(dc, 0, 0, cw, ch, 0, 0, cw, ch, imgData.pixel_data, &bmi, DIB_RGB_COLORS, SRCCOPY);
        }
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
    return DefWindowProc(hWnd, message, wParam, lParam);
}

CWindowsToolkit::CWindowsToolkit(void *parentWindow, CEditor *editor)
{
    this->parentWindow = parentWindow;
    this->editor       = editor;

    g_useCount++;
    if (g_useCount == 1)
    {
        WNDCLASSW windowClass;
        ZeroMemory(&windowClass, sizeof(WNDCLASSW));
        windowClass.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc   = WindowProc;
        windowClass.cbClsExtra    = 0;
        windowClass.cbWndExtra    = 0;
        windowClass.hInstance     = (HINSTANCE)hInstance;
        windowClass.hIcon         = LoadIcon((HINSTANCE)hInstance, MAKEINTRESOURCE(IDI_ICON));
        windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = NULL;
        windowClass.lpszMenuName  = 0;
        windowClass.lpszClassName = L"OxeEditorClass";
        RegisterClassW(&windowClass);
    }

    if (!parentWindow)
    {
        RECT rect;
        rect.left   = 100;
        rect.top    = 100;
        rect.right  = 100 + GUI_WIDTH;
        rect.bottom = 100 + GUI_HEIGHT;
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

        this->hWnd = CreateWindowW
        (
            L"OxeEditorClass",
            L"Oxe FM Synth",
            WS_OVERLAPPEDWINDOW,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
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
            L"OxeEditorClass",
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

    SetTimer(hWnd, 0, TIMER_RESOLUTION_MS, NULL);
}

CWindowsToolkit::~CWindowsToolkit()
{
    KillTimer(hWnd, 0);
    g_useCount--;
    if (g_useCount == 0)
    {
        UnregisterClassW(L"OxeEditorClass", (HINSTANCE)hInstance);
    }
    blImage.reset();
}

void CWindowsToolkit::StartWindowProcesses()
{
    if (!parentWindow)
    {
        ShowWindow(this->hWnd, SW_SHOWNORMAL);
    }
}

void CWindowsToolkit::Invalidate()
{
    if (hWnd)
    {
        ::InvalidateRect(hWnd, NULL, FALSE);
    }
}

void CWindowsToolkit::InvalidateRect(int x, int y, int width, int height)
{
    Invalidate();
}

void CWindowsToolkit::CopyRect(int destX, int destY, int width, int height, int origBmp, int origX, int origY)
{
    InvalidateRect(destX, destY, width, height);
}

void CWindowsToolkit::StartMouseCapture()
{
    SetCapture(this->hWnd);
}

void CWindowsToolkit::StopMouseCapture()
{
    ReleaseCapture();
}

float CWindowsToolkit::GetScale()
{
    if (!hWnd) return 1.0f;
    RECT rc;
    GetClientRect(hWnd, &rc);
    int cw = rc.right - rc.left;
    return cw > 0 ? (float)cw / (float)GUI_WIDTH : 1.0f;
}

void CWindowsToolkit::Resize(int width, int height)
{
    if (!hWnd) return;
    SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    ::InvalidateRect(hWnd, NULL, FALSE);
}

int CWindowsToolkit::WaitWindowClosed()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
