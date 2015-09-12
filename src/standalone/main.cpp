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
#include "editor.h"
#include "controller.h"
#include "resource.h"

#define MAX_DEVICES    32
#define MAX_DEVICENAME 128
#define AUDIOID        100
#define MIDIID         200

LRESULT CALLBACK ProcJanela      (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ProcDialogoMIDI (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ProcDialogoAUDIO(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL    CALLBACK DSEnumProc      (LPGUID lpGUID, LPCWSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext);

CController *controller;
CEditor     *editor;

typedef struct
{
    LPGUID GUID;
    WCHAR name[MAX_DEVICENAME];
} SDevice;

SDevice audiodevices[MAX_DEVICES];
SDevice  mididevices[MAX_DEVICES];
unsigned int portamidi  = 0;
unsigned int portaaudio = 0;
unsigned int devsMIDI   = 0;
unsigned int devsAUDIO  = 0;
HMENU submenuMIDI  = NULL;
HMENU submenuAUDIO = NULL;

// This is a helper function for adding a menu item (either a popup 
// or command item) to the specified menu template.
//
//    MenuTemplate - pointer to a menu template
//    MenuString   - string for the menu item to be added
//    MenuID       - id for the command item. Its value is ignored if IsPopup is TRUE.
//    IsPopup      - TRUE for popup menu (or submenu); FALSE for command item
//    LastItem     - TRUE if MenuString is the last item for the popup; 
//                   FALSE otherwise.
UINT AddMenuItem(LPVOID MenuTemplate, WCHAR* MenuString, WORD MenuID, BOOL IsPopup, BOOL IsChecked, BOOL IsGrayed, BOOL LastItem)
{
    MENUITEMTEMPLATE* mitem = (MENUITEMTEMPLATE*) MenuTemplate;

    UINT  bytes_used = 0;
    if (IsPopup)         // for popup menu
    {
        mitem->mtOption = MF_POPUP;
        if (LastItem)
            mitem->mtOption |= MF_END;

        bytes_used += sizeof (mitem->mtOption);  

        mitem = (MENUITEMTEMPLATE*) ((BYTE*) MenuTemplate + bytes_used);
        // a popup doesn't have mtID!!!

        StringCchCopyW((WCHAR*) mitem, MAX_DEVICENAME, MenuString);
        bytes_used += sizeof (WCHAR) * (wcslen(MenuString) + 1); // include '\0'   
    }
    else      // for command item
    {
        mitem->mtOption = 0;
        if (IsChecked)
            mitem->mtOption |= MF_CHECKED;
        if (IsGrayed)
            mitem->mtOption |= MF_GRAYED;
        if (LastItem)
            mitem->mtOption |= MF_END;
        mitem->mtID = MenuID;   
        StringCchCopyW(mitem->mtString, MAX_DEVICENAME, MenuString);  
        bytes_used += sizeof (mitem->mtOption ) + sizeof (mitem->mtID) + 
        sizeof (WCHAR) * (wcslen(MenuString) + 1);   // include '\0'
    }

    return bytes_used;
}

HMENU Submenu(unsigned int count, SDevice *devices, int ID)
{
    // For simplicity, allocate 500 bytes from stack. May use 
    // GlobalAlloc() to allocate memory bytes from heap.
    BYTE milist[500];
    memset(milist, 0, 500);

    // Fill up the MENUITEMTEMPLATEHEADER structure.
    MENUITEMTEMPLATEHEADER* mheader = (MENUITEMTEMPLATEHEADER*) milist;
    mheader->versionNumber = 0;
    mheader->offset = 0;

    int bytes_used = sizeof(MENUITEMTEMPLATEHEADER);

    if (count)
    {
        for (unsigned int i = 0; i < count; i++)
        {
            bytes_used += AddMenuItem(milist + bytes_used, devices[i].name, (WORD)(ID + i), FALSE, i == 0, FALSE, (i == count - 1));
        }
    }
    else
    {
        bytes_used += AddMenuItem(milist + bytes_used, (WCHAR*)L"(none)", 0, FALSE, FALSE, TRUE, TRUE);
    }

    // Load resource from a menu template in memory.
    return LoadMenuIndirect(milist);
}

void AddMenus(HWND hWnd)
{
    HMENU        sysmenu;
    MENUITEMINFO menuItem;
    
    submenuMIDI  = Submenu(devsMIDI, mididevices, MIDIID);
    submenuAUDIO = Submenu(devsAUDIO, audiodevices, AUDIOID);
    sysmenu      = GetSystemMenu(hWnd, FALSE);

    // Initializes the structure
    memset(&menuItem, 0, sizeof(menuItem));
    menuItem.cbSize = sizeof(menuItem);

    // Put a separator
    menuItem.fMask      = MIIM_TYPE;
    menuItem.fType      = MFT_SEPARATOR;
    InsertMenuItem(sysmenu, 0, true, &menuItem);

    // MIDI in menu
    menuItem.fMask      = MIIM_TYPE | MIIM_SUBMENU;
    menuItem.fType      = MFT_STRING;
    menuItem.dwTypeData = (LPSTR)"MIDI In";
    menuItem.hSubMenu   = submenuMIDI;
    InsertMenuItem(sysmenu, 0, true, &menuItem);

    // Audio out menu
    menuItem.fMask      = MIIM_TYPE | MIIM_SUBMENU;
    menuItem.fType      = MFT_STRING;
    menuItem.dwTypeData = (LPSTR)"Audio Out";
    menuItem.hSubMenu   = submenuAUDIO;
    InsertMenuItem(sysmenu, 0, true, &menuItem);
}

int APIENTRY WinMain(HINSTANCE hInstancia, HINSTANCE InstanciaAnt, LPSTR LinhaDeComando, int EstadoDaJanela)
{
    WNDCLASS wc;
    HWND     hWnd;
    RECT     rect;

    // Initializes pointers
    controller = NULL;
    editor     = NULL;

    // Enumarates MIDI ports
    devsMIDI = midiInGetNumDevs();
    for (unsigned int i = 0; i < devsMIDI; i++)
    {
        MIDIINCAPSW mic;
        if (0 == midiInGetDevCapsW(i,&mic,sizeof(mic)))
        {
            StringCchCopyW(mididevices[i].name, MAX_DEVICENAME, mic.szPname);
        }
    }

    // Enumerates audio ports
    if FAILED(DirectSoundEnumerateW((LPDSENUMCALLBACKW)DSEnumProc,NULL))
    {
        MessageBox(NULL,"Error when enumerate audio output ports",TITLE_SMALL,MB_OK);
        return 0;
    }

    if( !InstanciaAnt )
    {
        wc.lpszClassName = "OxeFMSynthStandAlone";
        wc.lpfnWndProc   = ProcJanela;
        wc.style         = CS_DBLCLKS;
        wc.hInstance     = hInstancia;
        wc.hIcon         = LoadIcon(hInstancia,MAKEINTRESOURCE(IDI_ICONE));
        wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = NULL;
        wc.lpszMenuName  = NULL;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;

        RegisterClass( &wc );
    }

    rect.left   = 100;
    rect.top    = 100;
    rect.right  = GUI_WIDTH;
    rect.bottom = GUI_HEIGHT;
    AdjustWindowRect(&rect, WS_SYSMENU | WS_CAPTION, FALSE);
    rect.bottom += GetSystemMetrics(SM_CYCAPTION);
    rect.bottom += GetSystemMetrics(SM_CYFIXEDFRAME);
    rect.right  += GetSystemMetrics(SM_CXFIXEDFRAME);
    
    hWnd = CreateWindow( "OxeFMSynthStandAlone",
        TITLE_FULL,
        WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom, 
        0,
        0,
        hInstancia,
        NULL
    );

    AddMenus(hWnd);

    controller = new CController();
    if (controller->Start(hWnd,portamidi,audiodevices[portaaudio].GUID))
    {
        editor = new CEditor(hInstancia,controller->GetSynth());
        editor->SetHandle(hWnd);
        ShowWindow(hWnd, EstadoDaJanela);
        MSG msg;
        while( GetMessage( &msg, NULL, 0, 0 ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        return (int)msg.wParam;
    }
    return 0;
}

LRESULT CALLBACK ProcJanela( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC         dc;
    switch(msg)
    {
        case WM_LBUTTONDBLCLK:
            if (editor)
            {
                POINT point;
                point.x = GET_X_LPARAM(lParam); 
                point.y = GET_Y_LPARAM(lParam);
                editor->OnLButtonDblClick(point);
                return 0L;
            }
        case WM_LBUTTONDOWN:
            if (editor)
            {
                POINT point;
                point.x = GET_X_LPARAM(lParam); 
                point.y = GET_Y_LPARAM(lParam);
                editor->OnLButtonDown(point);
            }
            return 0L;
        case WM_RBUTTONDOWN:
            if (editor)
            {
                editor->OnRButtonDown();
            }
            return 0L;
        case WM_LBUTTONUP:
            if (editor)
                editor->OnLButtonUp();
            return 0L;
        case WM_KEYDOWN:
            if (editor)
            {
                // from juce_win32_Windowing.cpp:doKeyDown
                const UINT keyChar  = MapVirtualKey ((UINT) wParam, 2);
                const UINT scanCode = MapVirtualKey ((UINT) wParam, 0);
                BYTE keyState[256];
                GetKeyboardState (keyState);
                
                WCHAR text[16] = { 0 };
                if (ToUnicode ((UINT) wParam, scanCode, keyState, text, 8, 0) != 1)
                    text[0] = 0;
                editor->OnChar(text[0]);
                // ---------------------------------------
            }
            return 0L;
        case WM_MOUSEMOVE:
            if (editor)
            {
                POINT point;
                point.x = GET_X_LPARAM(lParam); 
                point.y = GET_Y_LPARAM(lParam);
                editor->OnMouseMove(point);
            }
            return 0L;
        case WM_MOUSEWHEEL:
            if (editor)
            {
                POINT point;
                point.x = GET_X_LPARAM(lParam); 
                point.y = GET_Y_LPARAM(lParam);
                int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                zDelta /= WHEEL_DELTA;
                ScreenToClient(hWnd, &point);
                editor->OnMouseWheel(point, zDelta);
            }
            return 0L;
        case WM_PAINT:
            dc = BeginPaint(hWnd, &ps);
            if (editor)
                editor->OnPaint(dc, ps.rcPaint);
            EndPaint(hWnd, &ps); 
            return 0L;
        case WM_TIMER:
            if (editor)
                editor->OnTimer(hWnd);
            return 0L;
        case WM_SET_PROGRAM:
            if (editor)
            {
                char channel = (char)wParam;
                unsigned char numprog = (unsigned char)lParam;
                controller->GetSynth()->SendEvent(0xC0 + channel, numprog, 0, 0);
            }
            return 0L;
        case WM_SYSCOMMAND:
            MENUITEMINFO info;
            WORD id;
            unsigned int i;
            id = LOWORD(wParam);
            for (i=0; i<devsMIDI; i++)
            {
                if (MIDIID+i == id)
                {
                    if (portamidi != i)
                    {
                        info.cbSize = sizeof(MENUITEMINFO);
                        info.fMask  = MIIM_STATE;
                        info.fState = MFS_UNCHECKED;
                        SetMenuItemInfo(submenuMIDI, MIDIID+portamidi, FALSE, &info);
                        portamidi = i;
                        info.fMask  = MIIM_STATE;
                        info.fState = MFS_CHECKED;
                        SetMenuItemInfo(submenuMIDI, MIDIID+portamidi, FALSE, &info);
                        controller->Stop();
                        controller->Start(hWnd,portamidi,audiodevices[portaaudio].GUID);
                        return 0;
                    }
                }
            }
            for (i=0; i<devsAUDIO; i++)
            {
                if (AUDIOID+i == id)
                {
                    if (portaaudio != i)
                    {
                        info.cbSize = sizeof(MENUITEMINFO);
                        info.fMask  = MIIM_STATE;
                        info.fState = MFS_UNCHECKED;
                        SetMenuItemInfo(submenuAUDIO, AUDIOID+portaaudio, FALSE, &info);
                        portaaudio = i;
                        info.fMask  = MIIM_STATE;
                        info.fState = MFS_CHECKED;
                        SetMenuItemInfo(submenuAUDIO, AUDIOID+portaaudio, FALSE, &info);
                        controller->Stop();
                        controller->Start(hWnd,portamidi,audiodevices[portaaudio].GUID);
                        return 0;
                    }
                }
            }
            break;
        case WM_DESTROY:
            delete editor;
            controller->Stop();
            delete controller;
            editor      = NULL;
            controller = NULL;
            PostQuitMessage( 0 );
            return 0L;
    }
    return DefWindowProc( hWnd, msg, wParam, lParam );
}

BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCWSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext)
{
    if (lpGUID)
    {
        audiodevices[devsAUDIO].GUID = lpGUID;
        StringCchCopyW(audiodevices[devsAUDIO].name, MAX_DEVICENAME, lpszDesc);
        devsAUDIO++;
    }
    return true;
}
