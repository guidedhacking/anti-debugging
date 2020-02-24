#include <windows.h>
#include <tchar.h>
#include "AntiDebugMethod.h"
#include <io.h>
#include <fcntl.h>
#include <tlhelp32.h>
#include <psapi.h>

#include "Methods/MethodIsDebuggerPresent.h"
#include "Methods/MethodUnhandledException.h"
#include "Methods/MethodCheckRemoteDebuggerPresent.h"
#include "Methods/MethodPEBBeingDebugged.h"
#include "Methods/MethodNtGlobalFlag.h"
#include "Methods/MethodGetParentProcess.h"

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

void AddMenus(HWND hWnd);
void AddControls(HWND hWnd);
void LoadImages(HWND hwnd);

HMENU hMenu;
HBITMAP hLogoImage;
HWND hLogo, hDetectedMessage;

enum {WM_COMMAND_MENU_ID_EXIT = 1, WM_COMMAND_MENU_ID_ABOUT };
// any WM_COMMAND above 90 is for the antiDebugMethods instances.


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	#if _DEBUG
		AllocConsole();
		HANDLE stdHandle;
		int hConsole;
		FILE* fp;
		stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		hConsole = _open_osfhandle((long)stdHandle, _O_TEXT);
		fp = _fdopen(hConsole, "w");

		freopen_s(&fp, "CONOUT$", "w", stdout);
	#endif


	WNDCLASSA wc = {0};
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = "MainWindowClass";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassA(&wc))
	{
		return -1;
	}
	CreateWindowA("MainWindowClass", "Guided Hacking - Debugme", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 538, 380, NULL, NULL, NULL,NULL);

	MSG msg = {0};
	while(true)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE ))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		bool localAnyDetection = AntiDebugMethod::anyDetection;
		AntiDebugMethod::mainLoop();
		if (localAnyDetection != AntiDebugMethod::anyDetection)
		{
			if (AntiDebugMethod::anyDetection)
			{
				SendMessageA(hDetectedMessage, WM_SETTEXT, 0, (LPARAM)"Busted! You've been detected!");
			} else {
				SendMessageA(hDetectedMessage, WM_SETTEXT, 0, (LPARAM)"You're doing good. No debugger has been detected yet!");
			}
		}
	}


	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	switch(msg) 
	{
		case WM_COMMAND:
			std::cout << "lp: " << lp << "\n";
			std::cout << "wp: " << wp << "\n";

			switch(wp) {
				case WM_COMMAND_MENU_ID_EXIT:
					MessageBoxA(NULL, "Who clicks exit on the File menu? Just click the X icon u dumb fuck.", "Exit", MB_ICONINFORMATION);

					// DestroyWindow(hWnd);
					break;
				case WM_COMMAND_MENU_ID_ABOUT:
					MessageBoxA(NULL, "v1.0.0\n\nBy: RyccoSN \n\n\n www.guidedhacking.com", "About", MB_OK);
					break;
			}

			if (wp >= 90)
			{
				AntiDebugMethod::toggleThisMethod(wp - 90);
			}
			break;
		case WM_CREATE:
			LoadImages(hWnd);
			AddMenus(hWnd);
			AddControls(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProcA(hWnd, msg, wp, lp);
	}

	return 0;
}

void AddControls(HWND hWnd) {
	hDetectedMessage = CreateWindowA("static", "You're doing good. No debugger has been detected yet!", WS_VISIBLE | WS_CHILD | SS_CENTER, 100, 60, 300, 40, hWnd, NULL, NULL,NULL);
	
	AntiDebugMethod* firstOne = new AntiDebugMethod(MethodIsDebuggerPresent, 20, 100, "IsDebuggerPresent()");
	firstOne->createGUI(hWnd);

	AntiDebugMethod* secondOne = new AntiDebugMethod(MethodPEBBeingDebugged, 270, 100, "PEB->BeingDebugged");
	secondOne->createGUI(hWnd);

	AntiDebugMethod* thirdtOne = new AntiDebugMethod(MethodNtGlobalFlag, 20, 160, "NtGlobalFlag");
	thirdtOne->createGUI(hWnd);

	AntiDebugMethod* fourthOne = new AntiDebugMethod(MethodCheckRemoteDebuggerPresent, 270, 160, "CheckRemoteDebuggerPresent()");
	fourthOne->createGUI(hWnd);

	AntiDebugMethod* fifthOne = new AntiDebugMethod(MethodGetParentProcess, 20, 220, "Check Parent Process (CreateToolhelp32Snapshot)");
	fifthOne->createGUI(hWnd);

	AntiDebugMethod* sixthOne = new AntiDebugMethod(MethodUnhandledException, 270, 220, "UnhandledExceptionFilter");
	sixthOne->createGUI(hWnd);


	hLogo = CreateWindowA("static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, -10, 0, 100, 100, hWnd, NULL, NULL,NULL);
	SendMessageA(hLogo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogoImage);
}

void LoadImages(HWND hwnd) {
	hLogoImage = (HBITMAP)LoadImageA(NULL, "new_logo.bmp", IMAGE_BITMAP, 538,0, LR_LOADFROMFILE);
	HANDLE hIcon = LoadImage(0, _T("gh.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (hIcon) {
	    //Change both icons to the same icon handle.
	    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	    //This will ensure that the application icon gets changed too.
	    SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	    SendMessage(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	}

}

void AddMenus(HWND hWnd) {
	hMenu = CreateMenu();
	HMENU hFileMenu = CreateMenu();

	AppendMenuA(hFileMenu, MF_STRING, WM_COMMAND_MENU_ID_EXIT, "Exit");
	
	AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");
	AppendMenuA(hMenu, MF_STRING, WM_COMMAND_MENU_ID_ABOUT, "About");

	SetMenu(hWnd, hMenu);
}
