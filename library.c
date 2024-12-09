#include <stdio.h>
#include <windows.h>
#include "Utils/EXEPatchUtils.h"
#include "MazeAddrs.h"

#define MAX_MONITORS_EDW590 10

struct MonitorInfo {
	LONG x;
	LONG y;
	LONG width;
	LONG height;
};

int num_monitors_GL = 0;
struct MonitorInfo monitors_GL[MAX_MONITORS_EDW590] = { 0 };
HWND windows_handles_GL[MAX_MONITORS_EDW590] = { 0 };

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
struct WindowParams window_params_GL = { 0 };

void createAddWindows() {
	for (int i = 1; i < num_monitors_GL; i++) {
		struct MonitorInfo monitor = monitors_GL[i];
		windows_handles_GL[i] = CreateWindowExW(
				window_params_GL.dwExStyle,
				(LPCWSTR) window_params_GL.lpClassName,
				(LPCWSTR) window_params_GL.lpWindowName,
				window_params_GL.dwStyle,
				monitor.x,
				monitor.y,
				monitor.width,
				monitor.height,
				(HWND) window_params_GL.hWndParent,
				(HMENU) window_params_GL.hMenu,
				(HINSTANCE) window_params_GL.hInstance,
				(LPVOID) window_params_GL.lpParam
		);
	}
}

void __declspec(naked) createWindows() {
	__asm {
			call    CreateSSWindow
			push    eax
			call    createAddWindows
			pop     eax
			mov     dword ptr windows_handles_GL[0], eax

			push    0x10090B6
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

			push    esi
			push    dword ptr [ebp+0x8]
			push    0

			push    0x10096D8
			ret
	}
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &info);

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

	// CreateSSWindow (sub_100967D) mods //
	// Make X, Y, width and height parameters of CreateWindowEx main function arguments.
	//writeMem8(0x10096DF + 2, 0x28);
	//writeMem8(0x10096E2 + 2, 0x24);
	//writeMem16(0x10096D9 + 1, 0x3075);
	//writeMem16(0x10096DC + 1, 0x2C75);
	//writeMem8(0x100974A + 1, 0x2C);
	// Modify return value to be the window handle.
	writeMem16(0x100973F, 0x078B);
	writeMem32(0x100973F + 2, 0x5F909090);

	makeCall(0x10096D2, getWindowParams, true, false);
	makeCall(0x10090B1, createWindows, true, false);

	return TRUE;
}
