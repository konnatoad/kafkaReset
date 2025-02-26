#include <windows.h>
#include <commctrl.h>
#include <wchar.h>
#include <stdio.h>

HFONT hFont;
HWND hTab, hBackupPanel, hRestorePanel, hInstallPanel;
HBRUSH hBrush = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORLISTBOX: {
		if (!hBrush) hBrush = CreateSolidBrush(RGB(30, 30, 30));
		HDC hdc = (HDC)wParam;
		SetBkColor(hdc, RGB(30, 30, 30));
		SetTextColor(hdc, RGB(255, 255, 255));
		return (INT_PTR)hBrush;
	}
	case WM_NOTIFY: {
		LPNMHDR lpnmhdr = (LPNMHDR)lParam;
		if (lpnmhdr->idFrom == 1 && lpnmhdr->code == TCN_SELCHANGE) {
			int tabIndex = TabCtrl_GetCurSel(hTab);

			ShowWindow(hBackupPanel, (tabIndex == 0) ? SW_SHOW : SW_HIDE);
			ShowWindow(hRestorePanel, (tabIndex == 1) ? SW_SHOW : SW_HIDE);
			ShowWindow(hInstallPanel, (tabIndex == 2) ? SW_SHOW : SW_HIDE);
		}
	}
				  break;

	case WM_DESTROY:
		DeleteObject(hFont);
		DeleteObject(hBrush);
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow
) {
	const wchar_t CLASS_NAME[] = L"KafkaResetWindow";

	WNDCLASSW wc = { 0 };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));

	RegisterClassW(&wc);

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = 800;
	int windowHeight = 600;
	int posX = (screenWidth - windowWidth) / 2;
	int posY = (screenHeight - windowHeight) / 2;

	HWND hwnd = CreateWindowExW(
		0, CLASS_NAME, L"KafkaReset", WS_OVERLAPPEDWINDOW,
		posX, posY, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL
	);

	if (!hwnd) return 0;

	hFont = CreateFontW(
		20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI"
	);

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&icex);

	hTab = CreateWindowExW(
		0, WC_TABCONTROL, L"",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_TABS,
		10, 10, 780, 550,
		hwnd, (HMENU)1, hInstance, NULL
	);

	SendMessageW(hTab, WM_SETFONT, (WPARAM)hFont, TRUE);
	SetWindowTheme(hTab, L"", L"");

	if (!hTab) {
		MessageBoxW(NULL, L"Tab Control Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	TCITEMW tie;
	tie.mask = TCIF_TEXT;
	tie.pszText = L"Backup";
	tie.cchTextMax = 256;
	TabCtrl_InsertItem(hTab, 0, &tie);

	tie.pszText = L"Restore";
	tie.cchTextMax = 256;
	TabCtrl_InsertItem(hTab, 1, &tie);

	tie.pszText = L"Install Apps";
	tie.cchTextMax = 256;
	TabCtrl_InsertItem(hTab, 2, &tie);

	hBackupPanel = CreateWindowExW(
		0, L"STATIC", L"",
		WS_CHILD | WS_VISIBLE,
		15, 40, 760, 500,
		hwnd, (HMENU)10, hInstance, NULL
	);

	hRestorePanel = CreateWindowExW(
		0, L"STATIC", L"",
		WS_CHILD,
		15, 40, 760, 500,
		hwnd, (HMENU)11, hInstance, NULL
	);

	hInstallPanel = CreateWindowExW(
		0, L"STATIC", L"",
		WS_CHILD,
		15, 40, 760, 500,
		hwnd, (HMENU)12, hInstance, NULL
	);

	HWND hBackupLabel = CreateWindowExW(
		0, L"STATIC", L"Backup Options",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		280, 20, 200, 30,
		hBackupPanel, NULL, hInstance, NULL
	);
	SendMessageW(hBackupLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

	HWND hRestoreLabel = CreateWindowExW(
		0, L"STATIC", L"Restore Options",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		280, 20, 200, 30,
		hRestorePanel, NULL, hInstance, NULL
	);
	SendMessageW(hRestoreLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

	HWND hAppLabel = CreateWindowExW(
		0, L"STATIC", L"App Installation Options",
		WS_CHILD | WS_VISIBLE | SS_CENTER,
		280, 20, 250, 30,
		hInstallPanel, NULL, hInstance, NULL
	);
	SendMessageW(hAppLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg = { 0 };
	while (GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}
