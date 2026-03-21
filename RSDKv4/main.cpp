#include "RetroEngine.hpp"

#if !RETRO_USE_ORIGINAL_CODE

#if RETRO_PLATFORM == RETRO_WIN
#include "Windows.h"
#endif

void parseArguments(int argc, char *argv[])
{
    for (int a = 0; a < argc; ++a) {
        const char *find = "";

        find = strstr(argv[a], "stage=");
        if (find) {
            int b = 0;
            int c = 6;
            while (find[c] && find[c] != ';') Engine.startSceneFolder[b++] = find[c++];
            Engine.startSceneFolder[b] = 0;
        }

        find = strstr(argv[a], "scene=");
        if (find) {
            int b = 0;
            int c = 6;
            while (find[c] && find[c] != ';') Engine.startSceneID[b++] = find[c++];
            Engine.startSceneID[b] = 0;
        }

        find = strstr(argv[a], "console=true");
        if (find) {
            engineDebugMode       = true;
            Engine.devMenu        = true;
            Engine.consoleEnabled = true;
#if RETRO_PLATFORM == RETRO_WIN
            AllocConsole();
            freopen_s((FILE **)stdin, "CONIN$", "w", stdin);
            freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
            freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
#endif
        }

        find = strstr(argv[a], "usingCWD=true");
        if (find) {
            usingCWD = true;
        }
    }
}
#endif

#ifdef NXLINK
#include <switch.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <unistd.h>

static int s_nxlinkSock = -1;

static void initNxLink()
{
    if (R_FAILED(socketInitializeDefault()))
        return;

    s_nxlinkSock = nxlinkStdio();
    if (s_nxlinkSock >= 0)
        printf("printf output now goes to nxlink server\n");
    else
        socketExit();
}
#endif

int main(int argc, char *argv[])
{
#ifdef NXLINK
    initNxLink();
#endif

#if !RETRO_USE_ORIGINAL_CODE
    parseArguments(argc, argv);
#endif

    SDL_SetHint(SDL_HINT_WINRT_HANDLE_BACK_BUTTON, "1");
    Engine.Init();
    Engine.Run();

#if !RETRO_USE_ORIGINAL_CODE
    if (Engine.consoleEnabled) {
#if RETRO_PLATFORM == RETRO_WIN
        FreeConsole();
#endif //! RETRO_PLATFORM == RETRO_WIN
    }
#endif //! !RETRO_USE_ORIGINAL_CODE

#ifdef NXLINK
    socketExit();
#endif //! NXLINK

    return 0;
}

#if RETRO_PLATFORM == RETRO_UWP
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) { return SDL_WinRTRunApp(main, NULL); }
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

extern "C" {
    // Called by the JS wrapper to set settings (Mobile vs Standard)
    // type: 0 = Plus Mode (Unused in v4), 1 = Device Profile
    // value: 0 = Standard, 1 = Mobile
    void EMSCRIPTEN_KEEPALIVE RSDK_Configure(int value, int type)
    {
        if (type == 1) {
            if (value == 1) {
                Engine.gameDeviceType = RETRO_MOBILE;
                Engine.gamePlatform   = "MOBILE";
            }
            else {
                Engine.gameDeviceType = RETRO_STANDARD;
                Engine.gamePlatform   = "STANDARD";
            }
        }
    }

    // Called by the JS wrapper to actually start the game
    void EMSCRIPTEN_KEEPALIVE RSDK_Initialize()
    {
        // We manually call main. 
        // Since main() usually enters an infinite loop or emscripten_set_main_loop,
        // this is how we start the engine.
        main(0, NULL);
    }
}
#endif

