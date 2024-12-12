#include <windows.h>
#include <wingdi.h>
#include <gl/GL.h>
#include "Utils/EXEPatchUtils.h"
#include "MazeAddrs.h"

#define MAX_MONITORS_EDW590 100

struct MonitorInfo {
	bool primary;
	LONG x;
	LONG y;
	LONG width;
	LONG height;
	HWND hWnd;
	HDC hdc;
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

int num_prim_monitor_GL = 0;

void mainLoopHook() {
	for (int i = 0; i < num_monitors_GL; i++) {
		struct MonitorInfo monitor_info = monitors_GL[i];
		if (monitor_info.primary) {
			continue;
		}

		GLint *viewport = malloc(4 * sizeof(GLint));
		__asm {
				push    viewport
				push    GL_VIEWPORT
				call    ds:[glGetIntegerv_EXE]
		}

		bool full_screen_mode = readMem8((void *) gbTurboMode);
		if (full_screen_mode) {
			StretchBlt(monitor_info.hdc, 0, 0, monitor_info.width, monitor_info.height,
					   monitors_GL[num_prim_monitor_GL].hdc, viewport[0], viewport[1], viewport[2], viewport[3],
					   SRCCOPY);
		} else {
			StretchBlt(monitor_info.hdc, viewport[0], viewport[1], viewport[2], viewport[3],
			           monitors_GL[num_prim_monitor_GL].hdc, viewport[0], viewport[1], viewport[2], viewport[3],
					   SRCCOPY);
		}

		free(viewport);
	}
}

void __declspec(naked) updateWindowHook() {
	__asm {
			call    UpdateWindow_EXE

			pusha
			call    mainLoopHook
			popa

			push    0x100BA8C
			ret
	}
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
	}
}

void __declspec(naked) handlesPrepsWrapper() {
	__asm {
			call    SetupInitialWindows_EXE
			pusha
			call    handlesPreps
			popa

			push    0x1008E57
			ret
	}
}

bool createAddWindows() {
	for (int i = 0; i < num_monitors_GL; i++) {
		struct MonitorInfo *monitor = &monitors_GL[i];
		if (monitor->primary) {
			num_prim_monitor_GL = i;

			continue;
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

	// Primary goes last to be the main window (for some reason that needs to be the last created one)
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

	return monitor->hWnd != NULL;
}

void __declspec(naked) createWindows() {
	__asm {
			call    CreateSSWindow_EXE
			call    createAddWindows

			push    0x10090B6
			ret
	}
}

void storePrimMonitorHandles(HDC hdc_prim_monitor) {
	monitors_GL[num_prim_monitor_GL].hdc = hdc_prim_monitor;
}

void __declspec(naked) storePrimMonitorHandlesWrapper() {
	__asm {
			call    storePrimMonitorHandles

			call    ds:[wglMakeCurrent_EXE]

			push    0x1009A30
			ret
	}
}

void __declspec(naked) getWindowParams() {
	__asm {
			mov     eax, [ebp+0x10]
			mov     window_params_GL.dwExStyle, eax
			mov     eax, [ebp+0x1C]
			mov     window_params_GL.lpClassName, eax
			mov     eax, [ebp+0x14]
			mov     window_params_GL.lpWindowName, eax
			mov     eax, [ebp+0xC]
			mov     window_params_GL.dwStyle, eax
			mov     window_params_GL.hWndParent, ecx
			mov     window_params_GL.hMenu, 0
			mov     eax, [ebp+0x8]
			mov     window_params_GL.hInstance, eax
			mov     window_params_GL.lpParam, esi

			push    0x1009744
			ret
	}
}



BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	MONITORINFO info;
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &info);

	struct MonitorInfo *monitor_info = &monitors_GL[num_monitors_GL];
	monitor_info->primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
	monitor_info->x = info.rcMonitor.left;
	monitor_info->y = info.rcMonitor.top;
	monitor_info->width = info.rcMonitor.right - info.rcMonitor.left;
	monitor_info->height = info.rcMonitor.bottom - info.rcMonitor.top;

	num_monitors_GL++;

	if (num_monitors_GL >= MAX_MONITORS_EDW590) {
		return FALSE;
	}

	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID reserved) {
	if (reason != DLL_PROCESS_ATTACH) {
		return TRUE;
	}

	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

	// If there's only one monitor, there's no need to do anything.
	if (num_monitors_GL <= 1) {
		return TRUE;
	}

	makeCall(0x100BA87, updateWindowHook, true, false);

	makeCall(0x10096D2, getWindowParams, true, false); // 1st
	makeCall(0x1009A2A, storePrimMonitorHandlesWrapper, true, false); // 4th

	makeCall(0x10090B1, createWindows, true, false); // 2nd
	makeCall(0x1008E52, handlesPrepsWrapper, true, false); // 3rd

	return TRUE;
}
