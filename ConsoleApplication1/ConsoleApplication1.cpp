// ConsoleApplication1.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <windows.h>
#include <memory>
//#include <libloaderapi.h>
#include <cassert>

struct winampVisModule;

typedef int(__cdecl* funcConfig)(const winampVisModule*);
typedef int(__cdecl* funcInit)(const winampVisModule*);
typedef int(__cdecl* funcRender)(const winampVisModule*);
typedef void(__cdecl* procQuit)(const winampVisModule*);
typedef void(__cdecl* procUserData)(void);

struct winampVisModule
{
    /*
    TwinampVisModule = record*/
        char * description;        // description of module
        HWND hwndParent;         // parent window (filled in by calling app)
        HINSTANCE hDllInstance;        // instance handle to this DLL (filled in by calling app)
        unsigned _int32 sRate;     // sample rate (filled in by calling app)
        unsigned _int32 nCh;     // number of channels (filled in...)
        unsigned _int32 latencyMs;     // latency from call to Render to actual drawing
        unsigned _int32 delayMs;     // delay between calls to Render (in ms)

    // the data is filled in according to the respective Nch entry
        unsigned _int32 spectrumNCh;     // Number of channels
        unsigned _int32 waveformNCh;     // Number of channels
        byte spectrumData[2][576];     // waveform data   (values from 0-255)
        byte waveformData[2][576];     // spectrum data   (values from 0-255)

    // functions that winamp calls to configure the plugin, initialise ...
        //userData: procedure; cdecl;  // user data, optional*/

        funcConfig Config;
        funcInit Init;
        funcRender Render;
        procQuit Quit;
        procUserData userData;
};

typedef winampVisModule*(__cdecl* GetModule)(int);

struct winampVisHeader
{
    int version;
    char* description;
    GetModule getModule;
};

// PWinampVisModule = ^ TwinampVisModule;
static winampVisModule* lpWinampVisModule;

typedef winampVisHeader*(__cdecl* WinampVisGetHeader)();

/// <summary>
/// Call "winampVisGetHeader" from a winamp vis plugin dll.
/// 
/// </summary>
/// <returns></returns>
/// <remarks>https://slideplayer.com/slide/5224459/</remarks>
/// <remarks>http://www.sulaco.co.za/winamp_tut.htm</remarks>
/// <remarks>https://learn.microsoft.com/en-us/windows/win32/dlls/using-run-time-dynamic-linking</remarks>
int main()
{
    auto handleLib = LoadLibraryEx(L"D:\\Temp\\monkey\\vis_monkey.dll", NULL, NULL);
    if (handleLib == NULL)
    {
        auto dwError = GetLastError();

        LPTSTR lpMsgBuf;

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dwError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);

        wprintf(L"%s", (LPTSTR)lpMsgBuf);
    }
    else
    {
        WinampVisGetHeader lpFunc = (WinampVisGetHeader) GetProcAddress(handleLib, "winampVisGetHeader");
        auto lpModule = lpFunc();
        auto header = lpModule->getModule(0);
        /*auto hWnd = CreateWindow(
            L"Main",        // name of window class 
            L"Titre",            // title-bar string 
            WS_OVERLAPPEDWINDOW, // top-level window 
            CW_USEDEFAULT,       // default horizontal position 
            CW_USEDEFAULT,       // default vertical position 
            CW_USEDEFAULT,       // default width 
            CW_USEDEFAULT,       // default height 
            (HWND)NULL,         // no owner window 
            (HMENU)NULL,        // use class menu 
            NULL,           // handle to application instance 
            (LPVOID)NULL);      // no window-creation data 

        if (!hWnd)
            return FALSE;

        header->hwndParent = hWnd;*/
        header->Config(header);
        header->Init(header);
        header->Quit(header);

        FreeLibrary(handleLib);
    }

    std::cout << "Hello World!\n";
}
