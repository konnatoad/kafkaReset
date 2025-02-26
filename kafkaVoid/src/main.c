#include <windows.h>
#include <commctrl.h>
#include <wchar.h>
#include <uxtheme.h>

HFONT fontMain;
HWND mainTab, backupPanel, restorePanel, installPanel;
HBRUSH darkBrush = NULL;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORLISTBOX:
		if (!darkBrush) darkBrush = CreateSolidBrush(RGB(30, 30, 30));
		SetBkColor((HDC)wParam, RGB(30, 30, 30));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (INT_PTR)darkBrush;

	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->idFrom == 1 && ((LPNMHDR)lParam)->code == TCN_SELCHANGE) {
			int idx = TabCtrl_GetCurSel(mainTab);
			ShowWindow(backupPanel, idx == 0 ? SW_SHOW : SW_HIDE);
			ShowWindow(restorePanel, idx == 1 ? SW_SHOW : SW_HIDE);
			ShowWindow(installPanel, idx == 2 ? SW_SHOW : SW_HIDE);
		}
		break;

	case WM_DESTROY:
		DeleteObject(fontMain);
		DeleteObject(darkBrush);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpCmd, int nCmd) {
	WNDCLASSW wc = { 0 };
	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = hInst;
	wc.lpszClassName = L"KafkaResetMain";
	wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 30));

	RegisterClassW(&wc);

	int w = 800, h = 600;
	int scrW = GetSystemMetrics(SM_CXSCREEN), scrH = GetSystemMetrics(SM_CYSCREEN);
	HWND hwnd = CreateWindowExW(0, L"KafkaResetMain", L"KafkaReset", WS_OVERLAPPEDWINDOW,
		(scrW - w) / 2, (scrH - h) / 2, w, h, NULL, NULL, hInst, NULL);

	if (!hwnd) return 0;

	fontMain = CreateFontW(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");

	INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_TAB_CLASSES };
	InitCommonControlsEx(&icex);

	mainTab = CreateWindowExW(0, WC_TABCONTROL, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | TCS_TABS,
		10, 10, 780, 550, hwnd, (HMENU)1, hInst, NULL);

	SendMessageW(mainTab, WM_SETFONT, (WPARAM)fontMain, TRUE);
	SetWindowTheme(mainTab, L"", L"");

	TCITEMW ti = { TCIF_TEXT };
	wchar_t* tabNames[] = { L"Backup", L"Restore", L"Install Apps" };
	for (int i = 0; i < 3; i++) {
		ti.pszText = tabNames[i];
		TabCtrl_InsertItem(mainTab, i, &ti);
	}

	backupPanel = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE, 15, 40, 760, 500, hwnd, (HMENU)10, hInst, NULL);
	restorePanel = CreateWindowExW(0, L"STATIC", L"", WS_CHILD, 15, 40, 760, 500, hwnd, (HMENU)11, hInst, NULL);
	installPanel = CreateWindowExW(0, L"STATIC", L"", WS_CHILD, 15, 40, 760, 500, hwnd, (HMENU)12, hInst, NULL);

	HWND labels[3] = { 0 };
	HWND panels[3] = { backupPanel, restorePanel, installPanel };
	LPCWSTR texts[3] = { L"Backup Options", L"Restore Options", L"App Installation Options" };

	for (int i = 0; i < 3; i++) {
		labels[i] = CreateWindowExW(0, L"STATIC", texts[i], WS_CHILD | WS_VISIBLE | SS_CENTER,
			280, 20, 250, 30, panels[i], NULL, hInst, NULL);
		SendMessageW(labels[i], WM_SETFONT, (WPARAM)fontMain, TRUE);
	}

	ShowWindow(hwnd, nCmd);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return (int)msg.wParam;
}
