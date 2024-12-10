#include <windows.h>
#include <wingdi.h>
#include "Utils/EXEPatchUtils.h"
#include "MazeAddrs.h"

#define MAX_MONITORS_EDW590 10

struct MonitorInfo {
	bool primary;
	LONG x;
	LONG y;
	LONG width;
	LONG height;
	HWND hWnd;
	HDC hdc;
	HGLRC hglrc;
};

struct WindowParams {
	uint32_t dwExStyle;
	uint32_t lpClassName;
	uint32_t lpWindowName;
	uint32_t dwStyle;
	uint32_t hWndParent;
	uint32_t hMenu;
	uint32_t hInstance;
	uint32_t lpParam;
};

int num_monitors_GL = 0;
struct MonitorInfo monitors_GL[MAX_MONITORS_EDW590] = {0};
struct WindowParams window_params_GL = {0};

int current_monitor_GL = 0;
int num_prim_monitor_GL = 0;

void mainLoopHook() {
	struct MonitorInfo monitor_info = monitors_GL[current_monitor_GL];
	//HDC hdc = monitor_info.hdc;
	//HGLRC hglrc = monitor_info.hglrc;
	//bool success = 0;
	//__asm {
	//		push    hglrc
	//		push    hdc
	//		call    dword ptr ds:[wglMakeCurrent_EXE]
	//		mov     dword ptr success, eax
	//}

	SetWindowPos(monitors_GL[num_prim_monitor_GL].hWnd, NULL, monitor_info.x, monitor_info.y, monitor_info.width, monitor_info.height, 0x24);

	current_monitor_GL++;
	if (current_monitor_GL >= num_monitors_GL) {
		current_monitor_GL = 0;
	}

	Sleep(1/(60*num_monitors_GL));
}

void handlesPreps() {
	for (int i = 0; i < num_monitors_GL; i++) {
		struct MonitorInfo *monitor_info = &monitors_GL[i];
		if (monitor_info->primary) {
			continue;
		}

		HWND hWnd = monitor_info->hWnd;
		if (hWnd == NULL) {
			continue;
		}

		HDC hdc = GetDC(hWnd);
		if (hdc == NULL) {
			continue;
		}
		monitor_info->hdc = hdc;

		// 0x47 comes from the call to SetPixelFormat() inside the EXE. But that call happens only after this function
		// is called, so I must put here the value that would be used if the order was inverted.
		SetPixelFormat(hdc, 0x47, NULL);

		HGLRC hglrc = {0};
		__asm {
				push    dword ptr hdc
				call    dword ptr ds:[wglCreateContext_EXE]
				mov     hglrc, eax
		}

		if (hglrc == NULL) {
			continue;
		}
		monitor_info->hglrc = hglrc;
	}
}

void __declspec(naked) handlesPrepsWrapper() {
	__asm {
			call    dword ptr SetupInitialWindows
			pusha
			call    handlesPreps
			popa

			push    0x1008E57
			ret
	}
}

void createAddWindows() {
	for (int i = 0; i < num_monitors_GL; i++) {
		struct MonitorInfo *monitor = &monitors_GL[i];
		if (monitor->primary) {
			num_prim_monitor_GL = i;
		}

		monitor->hWnd = CreateWindowExW(
				window_params_GL.dwExStyle,
				(LPCWSTR) window_params_GL.lpClassName,
				(LPCWSTR) window_params_GL.lpWindowName,
				window_params_GL.dwStyle,
				monitor->x,
				monitor->y,
				monitor->width,
				monitor->height,
				(HWND) window_params_GL.hWndParent,
				(HMENU) window_params_GL.hMenu,
				(HINSTANCE) window_params_GL.hInstance,
				(LPVOID) window_params_GL.lpParam
		);

		ShowWindow(monitor->hWnd, SW_SHOW);
	}

	// Primary goes last, in case it's useful to have the window above the others
	struct MonitorInfo *monitor = &monitors_GL[num_prim_monitor_GL];
	monitor->hWnd = CreateWindowExW(
			window_params_GL.dwExStyle,
			(LPCWSTR) window_params_GL.lpClassName,
			(LPCWSTR) window_params_GL.lpWindowName,
			window_params_GL.dwStyle,
			monitor->x,
			monitor->y,
			monitor->width,
			monitor->height,
			(HWND) window_params_GL.hWndParent,
			(HMENU) window_params_GL.hMenu,
			(HINSTANCE) window_params_GL.hInstance,
			(LPVOID) window_params_GL.lpParam
	);

	ShowWindow(monitor->hWnd, SW_SHOW);
}

void __declspec(naked) createWindows() {
	__asm {
			call    dword ptr CreateSSWindow
			call    createAddWindows
			mov     eax, 1

			push    0x10090B6
			ret
	}
}

void shareLists() {
	HGLRC hglrc_prim_monitor = monitors_GL[num_prim_monitor_GL].hglrc;
	for (int i = 0; i < num_monitors_GL; i++) {
		if (monitors_GL[i].primary) {
			// Don't share the primary with itself

			continue;
		}

		HGLRC hglrc = monitors_GL[i].hglrc;
		__asm {
				push    dword ptr hglrc
				push    dword ptr hglrc_prim_monitor
				call    dword ptr ds:[wglShareLists_EXE]
		}
	}
}

void storePrimMonitorHandles(HDC hdc_prim_monitor, HGLRC hglrc_prim_monitor) {
	monitors_GL[num_prim_monitor_GL].hdc = hdc_prim_monitor;
	monitors_GL[num_prim_monitor_GL].hglrc = hglrc_prim_monitor;
}

void __declspec(naked) storeAndShareHandles() {
	__asm {
			call    storePrimMonitorHandles
			//call    shareLists

			call    dword ptr ds:[wglMakeCurrent_EXE]

			push    0x1009A30
			ret
	}
}

void __declspec(naked) getWindowParams() {
	__asm {
			mov     eax, dword ptr [ebp+0x10]
			mov     dword ptr window_params_GL.dwExStyle, eax
			mov     eax, dword ptr [ebp+0x1C]
			mov     dword ptr window_params_GL.lpClassName, eax
			mov     eax, dword ptr [ebp+0x14]
			mov     dword ptr window_params_GL.lpWindowName, eax
			mov     eax, dword ptr [ebp+0xC]
			mov     dword ptr window_params_GL.dwStyle, eax
			mov     dword ptr window_params_GL.hWndParent, ecx
			mov     dword ptr window_params_GL.hMenu, 0
			mov     eax, dword ptr [ebp+0x8]
			mov     dword ptr window_params_GL.hInstance, eax
			mov     dword ptr window_params_GL.lpParam, esi

			push    0x1009744
			ret
	}
}

void __declspec(naked) replaceGetMessage() {
	__asm {
		start:
			lea     eax, [ebp-0x1C]
			push    1
			push    0
			push    0
			push    0
			push    eax
			call    dword ptr ds:[PeekMessageW_EXE]
			test    eax, eax
			jz      start
			lea     eax, [ebp-0x1C]
			push    eax
			call    dword ptr ds:[TranslateMessage_EXE]
			lea     eax, [ebp-0x1C]
			push    eax
			call    dword ptr ds:[DispatchMessageW_EXE]
			call    mainLoopHook

			cmp     dword ptr [ebp-0x18], 0x12
			jne     start

			push    0x1008E93
			ret
	}
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &info);

	monitors_GL[num_monitors_GL].primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
	monitors_GL[num_monitors_GL].x = info.rcMonitor.left;
	monitors_GL[num_monitors_GL].y = info.rcMonitor.top;
	monitors_GL[num_monitors_GL].width = info.rcMonitor.right - info.rcMonitor.left;
	monitors_GL[num_monitors_GL].height = info.rcMonitor.bottom - info.rcMonitor.top;

	num_monitors_GL++;

	return TRUE;
}



BOOL __stdcall DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason != DLL_PROCESS_ATTACH) {
		return TRUE;
	}

	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

	// Replace GetMessage by PeekMessage on the main message loop
	makeCall(0x1008E5B, replaceGetMessage, true, false);

	makeCall(0x10096D2, getWindowParams, true, false); // 1st
	makeCall(0x1009A2A, storeAndShareHandles, true, false); // 4th

	makeCall(0x10090B1, createWindows, true, false); // 2nd
	makeCall(0x1008E52, handlesPrepsWrapper, true, false); // 3rd

	return TRUE;
}
