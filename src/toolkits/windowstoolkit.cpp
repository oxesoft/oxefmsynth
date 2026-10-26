/*
Oxe FM Synth: a software synthesizer
Copyright (C) 2004-2026  Daniel Moura <oxesoft@gmail.com>

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

static void GetViewport(int cw, int ch, int &vpX, int &vpY, int &vpW, int &vpH, double &scale)
{
    if (cw <= 0 || ch <= 0)
    {
        vpX = 0;
        vpY = 0;
        vpW = GUI_WIDTH;
        vpH = GUI_HEIGHT;
        scale = 1.0;
        return;
    }
    scale = fmin((double)cw / (double)GUI_WIDTH, (double)ch / (double)GUI_HEIGHT);
    vpW = (int)round(GUI_WIDTH * scale);
    vpH = (int)round(GUI_HEIGHT * scale);
    if (vpW < 1) vpW = 1;
    if (vpH < 1) vpH = 1;
    vpX = (cw - vpW) / 2;
    vpY = (ch - vpH) / 2;
}

static void ScaleClientPoint(HWND hWnd, int inX, int inY, int &outX, int &outY)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    int vpX, vpY, vpW, vpH;
    double scale;
    GetViewport(rc.right - rc.left, rc.bottom - rc.top, vpX, vpY, vpW, vpH, scale);
    outX = (int)floor((inX - vpX) / scale);
    outY = (int)floor((inY - vpY) / scale);
}

static void GetMouseScaled(HWND hWnd, LPARAM lParam, int &outX, int &outY)
{
    ScaleClientPoint(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), outX, outY);
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
    case WM_SETCURSOR:
    {
        if (toolkit && (toolkit->isResizing || toolkit->isOverGrip))
        {
            SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
            return TRUE;
        }
        break;
    }
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
        if (toolkit->editor && toolkit->editor->IsOverResizeGrip(mx, my))
        {
            toolkit->isResizing = true;
            GetCursorPos(&toolkit->dragStartScreenPos);
            RECT rc;
            GetClientRect(hWnd, &rc);
            toolkit->dragStartW = rc.right - rc.left;
            toolkit->dragStartH = rc.bottom - rc.top;
            SetCapture(hWnd);
            toolkit->editor->SetResizingGUI(true);
            SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
            return 0;
        }
        toolkit->editor->OnLButtonDown(mx, my);
        SetFocus(hWnd);
        return 0;
    }
    case WM_LBUTTONUP:
    {
        if (toolkit->isResizing)
        {
            toolkit->isResizing = false;
            ReleaseCapture();
            toolkit->editor->SetResizingGUI(false);
            int mx, my;
            GetMouseScaled(hWnd, lParam, mx, my);
            toolkit->isOverGrip = toolkit->editor->IsOverResizeGrip(mx, my);
            toolkit->editor->SetHoveringResizeGrip(toolkit->isOverGrip);
            return 0;
        }
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
        if (toolkit->isResizing)
        {
            POINT pt;
            GetCursorPos(&pt);
            int deltaX = pt.x - toolkit->dragStartScreenPos.x;
            int deltaY = pt.y - toolkit->dragStartScreenPos.y;
            double aspect = (double)GUI_WIDTH / (double)GUI_HEIGHT;
            double delta = (deltaX + deltaY * aspect) * 0.5;
            int targetW = (int)round(toolkit->dragStartW + delta);
            if (targetW < 475) targetW = 475;
            if (targetW > 2850) targetW = 2850;
            int targetH = (int)round((targetW * (double)GUI_HEIGHT) / (double)GUI_WIDTH);
            toolkit->editor->RequestResize(targetW, targetH);
            SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
            return 0;
        }
        int mx, my;
        GetMouseScaled(hWnd, lParam, mx, my);
        if (toolkit->editor)
        {
            bool overGrip = toolkit->editor->IsOverResizeGrip(mx, my);
            if (overGrip != toolkit->isOverGrip)
            {
                toolkit->isOverGrip = overGrip;
                toolkit->editor->SetHoveringResizeGrip(overGrip);
            }
            if (overGrip)
            {
                SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
            }
        }
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
            int mx, my;
            ScaleClientPoint(hWnd, point.x, point.y, mx, my);
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
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hWnd, &ps);
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int cw = clientRect.right - clientRect.left;
        int ch = clientRect.bottom - clientRect.top;
        if (cw > 0 && ch > 0 && toolkit && toolkit->editor)
        {
            int vpX, vpY, vpW, vpH;
            double scale;
            GetViewport(cw, ch, vpX, vpY, vpW, vpH, scale);

            if (toolkit->dibWidth != vpW || toolkit->dibHeight != vpH || !toolkit->hBitmap)
            {
                toolkit->blImage.reset();
                if (toolkit->hBitmap)
                {
                    if (toolkit->hOldBitmap)
                    {
                        SelectObject(toolkit->memDC, toolkit->hOldBitmap);
                        toolkit->hOldBitmap = NULL;
                    }
                    DeleteObject(toolkit->hBitmap);
                    toolkit->hBitmap = NULL;
                    toolkit->pixelData = NULL;
                }
                if (!toolkit->memDC)
                {
                    toolkit->memDC = CreateCompatibleDC(dc);
                }

                BITMAPINFO bmi;
                ZeroMemory(&bmi, sizeof(BITMAPINFO));
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = vpW;
                bmi.bmiHeader.biHeight = -vpH; // top-down
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 32;
                bmi.bmiHeader.biCompression = BI_RGB;

                toolkit->hBitmap = CreateDIBSection(dc, &bmi, DIB_RGB_COLORS, &toolkit->pixelData, NULL, 0);
                if (toolkit->hBitmap)
                {
                    toolkit->hOldBitmap = (HBITMAP)SelectObject(toolkit->memDC, toolkit->hBitmap);
                    toolkit->dibWidth = vpW;
                    toolkit->dibHeight = vpH;
                    toolkit->blImage.create_from_data(vpW, vpH, BL_FORMAT_PRGB32, toolkit->pixelData, (intptr_t)vpW * 4);
                    ps.rcPaint = clientRect;
                }
            }

            if (vpX > 0 || vpY > 0)
            {
                HBRUSH bgBrush = CreateSolidBrush(RGB(0x0b, 0x0e, 0x14));
                RECT bar;
                if (vpY > 0)
                {
                    SetRect(&bar, 0, 0, cw, vpY);
                    FillRect(dc, &bar, bgBrush);
                    SetRect(&bar, 0, vpY + vpH, cw, ch);
                    FillRect(dc, &bar, bgBrush);
                }
                if (vpX > 0)
                {
                    SetRect(&bar, 0, vpY, vpX, vpY + vpH);
                    FillRect(dc, &bar, bgBrush);
                    SetRect(&bar, vpX + vpW, vpY, cw, vpY + vpH);
                    FillRect(dc, &bar, bgBrush);
                }
                DeleteObject(bgBrush);
            }

            if (toolkit->hBitmap && toolkit->pixelData)
            {
                int bltX = ps.rcPaint.left - vpX;
                int bltY = ps.rcPaint.top - vpY;
                int bltW = ps.rcPaint.right - ps.rcPaint.left;
                int bltH = ps.rcPaint.bottom - ps.rcPaint.top;

                if (bltX < 0) { bltW += bltX; bltX = 0; }
                if (bltY < 0) { bltH += bltY; bltY = 0; }
                if (bltX + bltW > vpW) bltW = vpW - bltX;
                if (bltY + bltH > vpH) bltH = vpH - bltY;

                if (bltW > 0 && bltH > 0)
                {
                    int dirtyX = (int)floor(bltX / scale);
                    int dirtyY = (int)floor(bltY / scale);
                    int dirtyW = (int)ceil(bltW / scale) + 1;
                    int dirtyH = (int)ceil(bltH / scale) + 1;

                    if (dirtyX < 0) dirtyX = 0;
                    if (dirtyY < 0) dirtyY = 0;
                    if (dirtyX + dirtyW > GUI_WIDTH) dirtyW = GUI_WIDTH - dirtyX;
                    if (dirtyY + dirtyH > GUI_HEIGHT) dirtyH = GUI_HEIGHT - dirtyY;

                    BLContext ctx(toolkit->blImage);
                    ctx.scale(scale, scale);
                    toolkit->editor->Paint(ctx, dirtyX, dirtyY, dirtyW, dirtyH);
                    ctx.end();

                    GdiFlush();

                    BitBlt(dc, vpX + bltX, vpY + bltY, bltW, bltH, toolkit->memDC, bltX, bltY, SRCCOPY);
                }
            }
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
    this->memDC        = NULL;
    this->hBitmap      = NULL;
    this->hOldBitmap   = NULL;
    this->pixelData    = NULL;
    this->dibWidth     = 0;
    this->dibHeight    = 0;
    this->isResizing   = false;
    this->isOverGrip   = false;
    this->dragStartW   = GUI_WIDTH;
    this->dragStartH   = GUI_HEIGHT;
    this->dragStartScreenPos.x = 0;
    this->dragStartScreenPos.y = 0;

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
    if (memDC)
    {
        if (hOldBitmap)
        {
            SelectObject(memDC, hOldBitmap);
            hOldBitmap = NULL;
        }
        DeleteDC(memDC);
        memDC = NULL;
    }
    if (hBitmap)
    {
        DeleteObject(hBitmap);
        hBitmap = NULL;
        pixelData = NULL;
    }
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
    if (!hWnd) return;
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int vpX, vpY, vpW, vpH;
    double scale;
    GetViewport(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, vpX, vpY, vpW, vpH, scale);
    RECT rc;
    rc.left   = vpX + (int)floor((x - 1) * scale);
    rc.top    = vpY + (int)floor((y - 1) * scale);
    rc.right  = vpX + (int)ceil((x + width + 1) * scale);
    rc.bottom = vpY + (int)ceil((y + height + 1) * scale);
    ::InvalidateRect(hWnd, &rc, FALSE);
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
    int vpX, vpY, vpW, vpH;
    double scale;
    GetViewport(rc.right - rc.left, rc.bottom - rc.top, vpX, vpY, vpW, vpH, scale);
    return (float)scale;
}

void CWindowsToolkit::Resize(int width, int height)
{
    if (!hWnd) return;
    if (!parentWindow)
    {
        RECT rc = { 0, 0, width, height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        SetWindowPos(hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    else
    {
        SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
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
