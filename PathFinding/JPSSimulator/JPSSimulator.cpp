#include "stdafx.h"
#include "JPSSimulator.h"
#include "JumpPointSearch_GDI.h"
#include <windowsx.h>

#define NAME_TITLE		L"Title"
#define NAME_CLASS		L"Class"
#define GRID_WIDTH		62
#define GRID_HEIGHT		32
#define SCALE_UNIT		10
#define SCALE_MAX		120
#define SCALE_MIN		20

// Grid Variable
BOOL	g_bErase;
BOOL	g_bDrag;
BOOL	g_bCtrl;
INT		g_Scale;
INT		g_xPos;
INT		g_yPos;
INT		g_ScreenX;
INT		g_ScreenY;
CHAR	g_Map[MAX_HEIGHT][MAX_WIDTH];

// Path Finding Variable
JumpPointSearch_GDI g_JPS(g_Map, GRID_WIDTH, GRID_HEIGHT);

// Double Buffering Variable
HBITMAP g_hMemDCBitmap;
HBITMAP g_hMemDCBitmapOld;
HDC		g_hMemDC;
RECT	g_MemDCRect;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JPSSIMULATOR));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_JPSSIMULATOR);
	wcex.lpszClassName = NAME_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);

	// Perform application initialization:
	HWND hWnd = CreateWindowW(NAME_CLASS, NAME_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1280, 720, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDI_JPSSIMULATOR));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			HDC hdc = GetDC(hWnd);
			GetClientRect(hWnd, &g_MemDCRect);
			g_hMemDCBitmap = CreateCompatibleBitmap(hdc, g_MemDCRect.right, g_MemDCRect.bottom);
			g_hMemDC = CreateCompatibleDC(hdc);
			ReleaseDC(hWnd, hdc);
			g_hMemDCBitmapOld = (HBITMAP)SelectObject(g_hMemDC, g_hMemDCBitmap);
		}
		g_Scale = SCALE_MIN;
		g_ScreenX = 0;
		g_ScreenY = 0;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_ERASE:
			memset(g_Map, 0, sizeof(g_Map));
			g_JPS.Cleanup();
			InvalidateRect(hWnd, NULL, false);
			break; 
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_CONTROL:
			g_bCtrl = TRUE;
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_CONTROL:
			g_bCtrl = FALSE;
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			int iTileX = (xPos / g_Scale) + g_ScreenX;
			int iTileY = (yPos / g_Scale) + g_ScreenY;

			// 첫 선택 타일이 장애물이면 지우기 모드
			// 첫 선택 타일이 장애물이 아니면 장애물 넣기 모드
			if (!g_bCtrl)
				g_bErase = g_Map[iTileY][iTileX] == TRUE;
			else
			{
				g_xPos = xPos;
				g_yPos = yPos;
			}
			g_bDrag = TRUE;
		}
		break;
	case WM_LBUTTONUP:
		g_bDrag = FALSE;
		break;
	case WM_MOUSEMOVE:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			int iTileX = (xPos / g_Scale) + g_ScreenX;
			int iTileY = (yPos / g_Scale) + g_ScreenY;

			if (g_bDrag)
			{
				if (!g_bCtrl)
					g_Map[iTileY][iTileX] = !g_bErase;
				else
				{
					int iDeltaX = ((xPos - g_xPos) / g_Scale) + g_ScreenX;
					int iDeltaY = ((yPos - g_yPos) / g_Scale) + g_ScreenY;
					if (iDeltaX < g_ScreenX && g_ScreenX < GRID_WIDTH)
					{
						g_ScreenX++;
						g_xPos = xPos;
					}
					else if (iDeltaX > g_ScreenX && g_ScreenX > 0)
					{
						g_ScreenX--;
						g_xPos = xPos;
					}
					if (iDeltaY < g_ScreenY && g_ScreenY < GRID_WIDTH)
					{
						g_ScreenY++;
						g_yPos = yPos;
					}
					else if (iDeltaY > g_ScreenY && g_ScreenY > 0)
					{
						g_ScreenY--;
						g_yPos = yPos;
					}
				}
				InvalidateRect(hWnd, NULL, false);
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			int iTileX = (xPos / g_Scale) + g_ScreenX;
			int iTileY = (yPos / g_Scale) + g_ScreenY;
			if (!g_Map[iTileY][iTileX])
			{
				g_JPS.SetSource(iTileX, iTileY);
				InvalidateRect(hWnd, NULL, false);
			}
		}
		break;
	case WM_RBUTTONDBLCLK:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			int iTileX = (xPos / g_Scale) + g_ScreenX;
			int iTileY = (yPos / g_Scale) + g_ScreenY;
			if (!g_Map[iTileY][iTileX])
			{
				g_JPS.SetDestination(iTileX, iTileY);
				InvalidateRect(hWnd, NULL, false);
			}
		}
		break;
	case WM_MBUTTONUP:
		SetTimer(hWnd, 1, 100, NULL);
		SendMessage(hWnd, WM_TIMER, 1, NULL);
		break;
	case WM_MOUSEWHEEL:
		if (g_bCtrl)
		{
			if ((SHORT)HIWORD(wParam) > 0)
			{
				if (g_Scale <= SCALE_MAX - SCALE_UNIT)
				{
					if (g_ScreenX < GRID_WIDTH)
						g_ScreenX++;
					if (g_ScreenY < GRID_HEIGHT)
						g_ScreenY++;
					g_Scale += SCALE_UNIT;
				}
				InvalidateRect(hWnd, NULL, false);
			}
			else
			{
				if (g_Scale >= SCALE_MIN + SCALE_UNIT)
				{
					if (g_ScreenX > 0)
						g_ScreenX--;
					if (g_ScreenY > 0)
						g_ScreenY--;
					g_Scale -= SCALE_UNIT;
				}
				InvalidateRect(hWnd, NULL, false);
			}
		}
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (g_JPS.FindPathOnce())
				KillTimer(hWnd, 1);
			InvalidateRect(hWnd, NULL, false);
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		{
			PatBlt(g_hMemDC, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, WHITENESS);
			SetBkMode(g_hMemDC, TRANSPARENT);
			g_JPS.Render(g_hMemDC, g_ScreenX, g_ScreenY, g_Scale);

			// Double Buffering
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			BitBlt(hdc, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, g_hMemDC, 0, 0, SRCCOPY);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_SIZE:
		SelectObject(g_hMemDC, g_hMemDCBitmapOld);
		DeleteObject(g_hMemDC);
		DeleteObject(g_hMemDCBitmap);
		{
			HDC hdc = GetDC(hWnd);
			GetClientRect(hWnd, &g_MemDCRect);
			g_hMemDCBitmap = CreateCompatibleBitmap(hdc, g_MemDCRect.right, g_MemDCRect.bottom);
			g_hMemDC = CreateCompatibleDC(hdc);
			ReleaseDC(hWnd, hdc);
			g_hMemDCBitmapOld = (HBITMAP)SelectObject(g_hMemDC, g_hMemDCBitmap);
		}
		break;
	case WM_DESTROY:
		SelectObject(g_hMemDC, g_hMemDCBitmapOld);
		DeleteObject(g_hMemDC);
		DeleteObject(g_hMemDCBitmap);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
