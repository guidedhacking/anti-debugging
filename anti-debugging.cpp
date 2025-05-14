#include <windows.h>
#include <tchar.h>
#include "AntiDebugMethod.h"
#include <io.h>
#include <fcntl.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <fstream>

#include "json.hpp"

#include "Methods/MethodIsDebuggerPresent.h"
#include "Methods/MethodUnhandledException.h"
#include "Methods/MethodCheckRemoteDebuggerPresent.h"
#include "Methods/MethodPEBBeingDebugged.h"
#include "Methods/MethodNtGlobalFlag.h"
#include "Methods/MethodGetParentProcess.h"
#include "Methods/MethodWow64PEB.hpp"
#include "Methods/MethodThreadHideFromDebugger.h"
#include "Methods/MethodTrapFlag.h"
#include "Methods/MethodGetLocalTime.h"
#include "Methods/MethodGetTickCount.h"
#include "Methods/MethodQPC.h"
#include "Methods/MethodHeapFlag.h"
#include "Methods/MethodLFH.h"

#include "imgui/DX11Base.h"
#include "shared.h"

#define USE_IMGUI

LRESULT CALLBACK WindowProcedure( HWND, UINT, WPARAM, LPARAM );

void AddMenus( HWND hWnd );
void AddControls( HWND hWnd );
void LoadImages( HWND hwnd );
void SetIcon(HWND hwnd);

HMENU hMenu;
HBITMAP hLogoImage;
HWND hLogo, hDetectedMessage;

bool dx11_loaded = false;

// any WM_COMMAND above 90 is for the antiDebugMethods instances.

FILE * fp;

const char* good_string = "You're doing good. No debugger has been detected yet!";
const char* bad_string = "Busted! You've been detected!";
const char* cfg_name = "cfg.json";

std::string exe_path = "";

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
#if _DEBUG
	AllocConsole( );
	freopen_s( &fp, "CONOUT$", "w", stdout );
#endif

	LPWSTR cmdLineW = GetCommandLineW(); 
	int argc;
	LPWSTR* argv = CommandLineToArgvW(cmdLineW, &argc);

	std::wstring warg = argv[0];
	exe_path = std::string(warg.begin(), warg.end());

	WNDCLASSA wc = { 0 };
	wc.hbrBackground = ( HBRUSH )COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = "MainWindowClass";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassA( &wc ) )
	{
		return -1;
	}

	bool allow_imgui = true;
	std::ifstream config(cfg_name);
	if (config.is_open())
	{
		nlohmann::json cfg_json = nlohmann::json::parse(config);
		if (cfg_json.contains("useImgui"))
		{
			allow_imgui = cfg_json["useImgui"];
		}
		config.close();
	}
	

	HWND hwnd = CreateWindowA( "MainWindowClass", "Guided Hacking - AntiDebug", (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~(WS_MAXIMIZEBOX), 100, 100, 538, 330, NULL, NULL, NULL, NULL);
	SetIcon(hwnd);

#ifdef USE_IMGUI
	if (allow_imgui)
	{
		dx11_loaded = dx11_base::create(hwnd);
	}
#endif

	// fallback to win32 gui
	if (!dx11_loaded)
	{
		LoadImages(hwnd);
		AddMenus(hwnd);
	}
	
	AddControls(hwnd);

	if (dx11_loaded)
	{
		std::thread([] 
		{
			while (true)
			{
				dx11_base::frame(dx11_base::on_render, true);
			}
		}).detach();
	}

	dx11_base::update_message(good_string);
	std::thread([]
	{
		while (true)
		{
			bool localAnyDetection = AntiDebugMethod::anyDetection;
			AntiDebugMethod::mainLoop();
			if (localAnyDetection != AntiDebugMethod::anyDetection)
			{
				if (AntiDebugMethod::anyDetection)
				{
					dx11_loaded ? dx11_base::update_message(bad_string) : SendMessageA(hDetectedMessage, WM_SETTEXT, 0, (LPARAM)bad_string);
					
				}
				else
				{
					dx11_loaded ? dx11_base::update_message(good_string) : SendMessageA(hDetectedMessage, WM_SETTEXT, 0, (LPARAM)good_string);
				}
			}

			Sleep(1);
		}
	}).detach();

	MSG msg = { 0 };
	while ( true )
	{
		if ( PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}


	return 0;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProcedure( HWND hWnd, UINT msg, WPARAM wp, LPARAM lp ) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wp, lp))
		return true;

	bool disabling_imgui = false;

	switch ( msg )
	{
	case WM_COMMAND:
		std::cout << "lp: " << lp << "\n";
		std::cout << "wp: " << wp << "\n";

		switch ( wp ) {
		case WM_COMMAND_MENU_ID_EXIT:
			DestroyWindow(hWnd);
			break;
		case WM_COMMAND_MENU_ID_ABOUT:
			MessageBoxA(hWnd, "v1.0.0\n\nBy: RyccoSN \n\n\n www.guidedhacking.com", "About", MB_OK );
			break;
		case WM_COMMAND_MENU_HELP:
			ShellExecuteA(NULL, "open", "https://guidedhacking.com/threads/anti-debug-techniques-a-comprehensive-guide.20391/", NULL, NULL, SW_SHOWNORMAL);
			break;

		case WM_COMMAND_MENU_DISABLE_IMGUI:
			disabling_imgui = true;
		case WM_COMMAND_MENU_ENABLE_IMGUI:
			std::ifstream cfg(cfg_name);
			if (cfg.is_open())
			{
				nlohmann::json cfg_json = nlohmann::json::parse(cfg);
				cfg_json["useImgui"] = !disabling_imgui;
				
				cfg.close();
				std::ofstream out_cfg(cfg_name);
				out_cfg << cfg_json.dump(4) << std::endl;
				out_cfg.close();
			}
			else
			{
				nlohmann::json cfg_json;
				cfg_json["useImgui"] = !disabling_imgui;

				std::ofstream out_cfg(cfg_name);
				out_cfg << cfg_json.dump(4) << std::endl;
				out_cfg.close();
			}

			std::string start_str = "start \"\" \"" + exe_path + "\" & exit";

			std::thread([start_str]
			{
				system(start_str.c_str());
			}).detach();

			Sleep(10);
			exit(0);
			break;
		}

		if (wp >= 90)
		{
			if (!dx11_loaded)
			{
				AntiDebugMethod::toggleThisMethod(wp - 90);
			}
		}
		break;
	case WM_SIZE:
		dx11_base::handle_resize(lp);
		break;
	case WM_DESTROY:
#if _DEBUG
		FreeConsole( );
		fclose( fp );
#endif
		ExitProcess( 0 );
		break;
	}

	return DefWindowProcA(hWnd, msg, wp, lp);
}

void AddControls( HWND hWnd ) {
	int cur_x = 20, cur_y = 100;
	using fnptr = bool( * )( void );

	auto AddMethod = [ & ]( fnptr fn, const char * name)
	{
		auto methamphetamine = new AntiDebugMethod(fn, cur_x, cur_y, name);
		if ( cur_x == 20 )
			cur_x = 270;
		else
		{
			cur_x = 20;
			cur_y += 60;

			if ( cur_y > 220 ) // idk random number
			{
				RECT r = { 0 };
				GetWindowRect(hWnd, &r );
				SetWindowPos(hWnd, 0, 0, 0, r.right - r.left, ( r.bottom - r.top ) + 50, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
				// automatically resize window as we add more methods.
			}
		}

		if (!dx11_loaded)
		{
			methamphetamine->createGUI(hWnd);
		}
	};

	AddMethod(MethodIsDebuggerPresent, "IsDebuggerPresent");
	AddMethod(MethodPEBBeingDebugged, "PEB->BeingDebugged");
	AddMethod(MethodNtGlobalFlag, "NtGlobalFlag");
	AddMethod(MethodCheckRemoteDebuggerPresent, "CheckRemoteDebuggerPresent");
	AddMethod(MethodGetParentProcess, "Check Parent Process");
	AddMethod(MethodUnhandledException, "UnhandledExceptionFilter");
	AddMethod(MethodWow64PEB, "WoW64 PEB->BeingDebugged");
	AddMethod(MethodThreadHideFromDebugger, 
		dx11_loaded ? "ThreadHideFromDebugger\n(will crash if debugged)" : "ThreadHideFromDebugger (will crash if debugged)");
	AddMethod(MethodTrapFlag, "SEH & TrapFlag");
	AddMethod(MethodLFH, "LowFragmentationHeap");
	AddMethod(MethodHeapFlags, "Heap Flags");
	AddMethod(MethodGetLocalTime, "GetLocalTime Detection");
	AddMethod(MethodGetTickCount, "GetTickCount Detection");
	AddMethod(MethodQPC, dx11_loaded ? "QueryPerformanceCounter\nDetection" : "QueryPerformanceCounter Detection");

	if (!dx11_loaded)
	{
		hDetectedMessage = CreateWindowA("static", good_string, WS_VISIBLE | WS_CHILD | SS_CENTER, 100, 60, 300, 40, hWnd, NULL, NULL, NULL);
		hLogo = CreateWindowA( "static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, -10, 0, 100, 100, hWnd, NULL, NULL, NULL );
		SendMessageA( hLogo, STM_SETIMAGE, IMAGE_BITMAP, ( LPARAM )hLogoImage );
	}
}

void LoadImages( HWND hwnd ) {
	hLogoImage = ( HBITMAP )LoadImageA( NULL, "resources/new_logo.bmp", IMAGE_BITMAP, 538, 0, LR_LOADFROMFILE );
	printf("%p\n", hLogoImage);
}

void AddMenus( HWND hWnd ) {
	hMenu = CreateMenu( );
	HMENU hFileMenu = CreateMenu( );

	AppendMenuA(hFileMenu, MF_STRING, WM_COMMAND_MENU_ENABLE_IMGUI, "Enable Imgui");
	AppendMenuA(hFileMenu, MF_STRING, WM_COMMAND_MENU_ID_EXIT, "Exit" );

	AppendMenuA(hMenu, MF_POPUP, ( UINT_PTR )hFileMenu, "File");
	AppendMenuA(hMenu, MF_STRING, WM_COMMAND_MENU_ID_ABOUT, "About");
	AppendMenuA(hMenu, MF_STRING, WM_COMMAND_MENU_HELP, "Help");

	SetMenu( hWnd, hMenu );
}

void SetIcon(HWND hwnd)
{
	HANDLE hIcon = LoadImage(0, _T("resources/gh.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (hIcon)
	{
		//Change both icons to the same icon handle.
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		//This will ensure that the application icon gets changed too.
		SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	}

}
