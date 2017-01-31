#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hList;
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("取得"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 256, 32, TRUE);
		MoveWindow(hList, 10, 50, 512, 512, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			TCHAR szDefaultPrinter[256] = { 0 };
			DWORD dwSize = _countof(szDefaultPrinter);
			if (GetDefaultPrinter(szDefaultPrinter, &dwSize))
			{
				const int count = DeviceCapabilities(szDefaultPrinter, NULL, DC_PAPERNAMES, NULL, NULL);
				if (count > 0)
				{
					// 用紙名
					LPTSTR pszNames = new TCHAR[count * 64];
					DeviceCapabilities(szDefaultPrinter, NULL, DC_PAPERNAMES, pszNames, NULL);

					// 用紙定数
					LPWORD pnIds = new WORD[count];
					DeviceCapabilities(szDefaultPrinter, NULL, DC_PAPERS, (LPTSTR)pnIds, NULL);

					// 用紙サイズ
					POINT* pSizes = new POINT[count];
					DeviceCapabilities(szDefaultPrinter, NULL, DC_PAPERSIZE, (LPTSTR)pSizes, NULL);

					for (int i = 0; i < count; ++i)
					{
						TCHAR szText[1024];
						wsprintf(szText, TEXT("%s (%d, %dx%d)"), (pszNames + i * 64), pnIds[i], pSizes[i].x, pSizes[i].y);
						SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szText);
					}

					delete[] pszNames;
					delete[] pnIds;
					delete[] pSizes;
				}
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("デフォルトプリンターの用紙サイズリストを取得"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
