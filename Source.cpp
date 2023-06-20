#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>

static HHOOK g_hook;

int lockX, lockY;

bool locked = true;

LRESULT WINAPI callback_mouse(int code, WPARAM wParam, LPARAM lParam)
{
	if (!code) {
		MSLLHOOKSTRUCT* pHookStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

		if (pHookStruct) {
			if (wParam == WM_MOUSEMOVE) {
				if ((pHookStruct->pt.x > lockX || pHookStruct->pt.y > lockY) && locked) {
					return 1;
				}
			}
		}
	}

	return CallNextHookEx(g_hook, code, wParam, lParam);
}

void Hook()
{
	g_hook = SetWindowsHookExA(WH_MOUSE_LL, callback_mouse, NULL, NULL);

	if (!g_hook) {
		printf("Error: %x\n", GetLastError());
	}
}

void UnHook()
{
	UnhookWindowsHookEx(g_hook);
}

std::string getFocusName() {
	static HWND forWindow = GetForegroundWindow();
	forWindow = GetForegroundWindow();

	if (forWindow == NULL)
		return "";

	char title[256];
	GetWindowTextA(forWindow, title, sizeof title);

	return title;
}


int main()
{
	lockX = GetSystemMetrics(SM_CXSCREEN) - 1;
	lockY = GetSystemMetrics(SM_CYSCREEN);
	Hook();

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
			if (getFocusName() == "Roblox") {
				locked = true;
			}
			else {
				locked = false;
			}

			Sleep(1);

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (GetAsyncKeyState(VK_INSERT) && GetAsyncKeyState(VK_SPACE)) {
				break;
			}
	}

	UnHook();

	return static_cast<int>(msg.wParam);
}