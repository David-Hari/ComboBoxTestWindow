// TestWindow.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TestWindow.h"

#pragma comment(linker, \
  "\"/manifestdependency:type='Win32' "\
  "name='Microsoft.Windows.Common-Controls' "\
  "version='6.0.0.0' "\
  "processorArchitecture='*' "\
  "publicKeyToken='6595b64144ccf1df' "\
  "language='*'\"")

#pragma comment(lib, "ComCtl32.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;
CHAR szTitle[MAX_LOADSTRING];
CHAR szWindowClass[MAX_LOADSTRING];
HWND mainWindow;
HWND comboBox;

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    EditBoxProc(HWND, UINT, WPARAM, LPARAM);

WNDPROC originalEditBoxProc;

// Some boring placeholder values to show that regular ASCII text works ok
char a[] = "aaa";
char b[] = "bbb";
char c[] = "ccc";

// Some "Unicode" strings.
// First one is using Unicode escape codes, the second is using hex escape codes for the UTF-8 bytes. Both say the same thing.
char testString1[] = "\u4EFB\u4F55\u4EBA\u90FD\u53EF\u4EE5\u8BFB\u8FD9\u4E2A\u5417";
char testString2[] = "\xE4\xBB\xBB\xE4\xBD\x95\xE4\xBA\xBA\xE9\x83\xBD\xE5\x8F\xAF\xE4\xBB\xA5\xE8\xAF\xBB\xE8\xBF\x99\xE4\xB8\xAA\xE5\x90\x97";


int APIENTRY WinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR    lpCmdLine,
                     _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringA(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringA(hInstance, IDC_TESTWINDOW, szWindowClass, MAX_LOADSTRING);

    INITCOMMONCONTROLSEX init;
    init.dwSize = sizeof(INITCOMMONCONTROLSEX);
    init.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES | ICC_USEREX_CLASSES |
        ICC_COOL_CLASSES | ICC_INTERNET_CLASSES | ICC_PAGESCROLLER_CLASS |
        ICC_NATIVEFNTCTL_CLASS | ICC_STANDARD_CLASSES | ICC_LINK_CLASS;
    InitCommonControlsEx(&init);

    WNDCLASSEXA wcex;
    wcex.cbSize = sizeof(WNDCLASSEXA);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTWINDOW));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEA(IDC_TESTWINDOW);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExA(&wcex);

    hInst = hInstance; // Store instance handle in our global variable

    mainWindow = CreateWindowA(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        500, 500, 400, 400, NULL, NULL, hInstance, NULL);
    comboBox = CreateWindowExA(0x200, "COMBOBOX", "", 0x50220042,
        20, 50, 300, 40, mainWindow, NULL, hInstance, NULL);

    // Setting the font is not strictly necessary to reproduce
    HFONT hFont = CreateFontA(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 254,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
        DEFAULT_PITCH, "Microsoft Sans Serif");
    SendMessage(comboBox, WM_SETFONT, (WPARAM)hFont, FALSE);

    HWND editWnd = GetWindow(comboBox, GW_CHILD);
    originalEditBoxProc = (WNDPROC)GetWindowLongA(editWnd, GWL_WNDPROC);
    SetWindowLongA(editWnd, GWL_WNDPROC, (LONG)EditBoxProc);

    ShowWindow(mainWindow, nCmdShow);
    UpdateWindow(mainWindow);
    ShowWindow(mainWindow, nCmdShow);
    UpdateWindow(comboBox);

    SendMessageA(comboBox, CB_RESETCONTENT, 0, 0);
    SendMessageA(comboBox, CB_INSERTSTRING, -1, (LPARAM)&a);
    SendMessageA(comboBox, CB_INSERTSTRING, -1, (LPARAM)&testString1);
    SendMessageA(comboBox, CB_INSERTSTRING, -1, (LPARAM)&b);
    SendMessageA(comboBox, CB_INSERTSTRING, -1, (LPARAM)&testString2);
    SendMessageA(comboBox, CB_INSERTSTRING, -1, (LPARAM)&c);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int) msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (hWnd == mainWindow) {
        switch (message) {
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
            case IDM_SETTEXT: {
                SetWindowTextA(comboBox, "\x61\x62\x63\xE2\x82\xAC\x64\x65\x66\xF0\x9D\x9F\x9A\x67\x68\x69\0");
                break;
            }
            case IDM_GETTEXT: {
                // Put debugger breakpoint here to see the returned sizes.
                int size = GetWindowTextLengthA(comboBox);
                char* tempString = new char[size + 1];
                int size2 = GetWindowTextA(comboBox, tempString, size + 1);
                break;
            }
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }
    else if (hWnd == comboBox) {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK EditBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    return CallWindowProcA(originalEditBoxProc, hWnd, message, wParam, lParam);
}
