// RBTreeBuilder.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RBTreeBuilder.h"
#include "RedblackTree.h"
#include <windowsx.h>

#define NAME_TITLE		L"Title"
#define NAME_CLASS		L"Class"
#define MOVE_SPEED		15

enum BUILD_STEP
{
	READY = 0,
	INSERT,
	REMOVE
};
BUILD_STEP		g_Step;
RedblackTree	g_RedblackTree;

CHAR			g_Usage[256];
CHAR			g_Text[256];
INT				g_OffsetX;
INT				g_OffsetY;

HBITMAP			g_hMemDCBitmap;
HBITMAP			g_hMemDCBitmapOld;
HDC				g_hMemDC;
RECT			g_MemDCRect;
HPEN			g_hGridPen;

// Forward declarations of functions included in this code module:
VOID RenderText(HDC hdc);
VOID RenderTree(HDC hdc);
VOID KeyEvent(HWND hWnd, WPARAM wParam);
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
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RBTREEBUILDER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_RBTREEBUILDER);
	wcex.lpszClassName = NAME_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);

	// Perform application initialization:
	HWND hWnd = CreateWindowW(NAME_CLASS, NAME_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 1200, 900, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDI_RBTREEBUILDER));

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
VOID RenderText(HDC hdc)
{
	SetTextColor(hdc, RGB(0, 0, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdc, g_hGridPen);
	const char* item = "1 랜덤 노드입력, 2 직접 노드입력, 3 직접 노드삭제, 4 전체 노드삭제, 5 종료";
	int x = g_MemDCRect.right * 0.05;
	int y = g_MemDCRect.bottom * 0.85;
	TextOutA(hdc, x, y, item, strlen(item));
	TextOutA(hdc, x, y + 25, g_Usage, strlen(g_Usage));
	TextOutA(hdc, x, y + 50, g_Text, strlen(g_Text));
	SelectObject(hdc, hOldPen);
}
VOID RenderTree(HDC hdc)
{
	SetTextColor(hdc, RGB(255, 255, 255));
	HPEN hOldPen = (HPEN)SelectObject(hdc, g_hGridPen);
	int x = (g_MemDCRect.right * 0.5) + g_OffsetX;
	int y = (g_MemDCRect.bottom * 0.1) + g_OffsetY;
	g_RedblackTree.Draw(hdc, x, y);
	SelectObject(hdc, hOldPen);
}
VOID KeyEvent(HWND hWnd, WPARAM wParam)
{
	int len;
	switch (wParam)
	{
	case 0x57:				// W
		g_OffsetY -= MOVE_SPEED;
		break;
	case 0x41:				// A
		g_OffsetX -= MOVE_SPEED;
		break;
	case 0x53:				// S
		g_OffsetY += MOVE_SPEED;
		break;
	case 0x44:				// D
		g_OffsetX += MOVE_SPEED;
		break;
	case 0x30:				// 0
	case 0x31:				// 1
	case 0x32:				// 2
	case 0x33:				// 3
	case 0x34:				// 4
	case 0x35:				// 5
	case 0x36:				// 6
	case 0x37:				// 7
	case 0x38:				// 8
	case 0x39:				// 9
		len = strlen(g_Text);
		g_Text[len] = (char)wParam;
		g_Text[len + 1] = '\0';
		break;
	case VK_BACK:
		len = strlen(g_Text);
		if (len > 0)
			g_Text[len - 1] = '\0';
		break;
	case VK_RETURN:
		switch (g_Step)
		{
		case BUILD_STEP::READY:
			switch (atoi(g_Text))
			{
			case 1:			// 랜덤 노드입력
				g_RedblackTree.Insert(rand() % 1000);
				break;
			case 2:			// 직접 노드입력
				strcpy_s(g_Usage, sizeof(g_Usage), "2. 추가할 노드 번호를 입력하세요.");
				g_Step = BUILD_STEP::INSERT;
				break;
			case 3:			// 직접 노드삭제
				strcpy_s(g_Usage, sizeof(g_Usage), "3. 삭제할 노드 번호를 입력하세요.");
				g_Step = BUILD_STEP::REMOVE;
				break;
			case 4:			// 전체 노드삭제
				g_RedblackTree.RemoveAll();
				break;
			case 5:			// 종료
				PostMessage(hWnd, WM_QUIT, NULL, NULL);
				break;
			default:
				break;
			}
			break;
		case BUILD_STEP::INSERT:
			g_RedblackTree.Insert(atoi(g_Text));
			g_Usage[0] = '\0';
			g_Step = BUILD_STEP::READY;
			break;
		case BUILD_STEP::REMOVE:
			g_RedblackTree.Remove(atoi(g_Text));
			g_Usage[0] = '\0';
			g_Step = BUILD_STEP::READY;
			break;
		default:
			break;
		}
		g_Text[0] = '\0';
		break;
	default:
		break;
	}
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
		g_hGridPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		break;
	case WM_CHAR:
		KeyEvent(hWnd, wParam);
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	case WM_PAINT:
		SelectObject(g_hMemDC, GetStockObject(DC_BRUSH));
		SetDCBrushColor(g_hMemDC, RGB(0x90, 0xee, 0x90));
		PatBlt(g_hMemDC, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, PATCOPY);
		SetBkMode(g_hMemDC, TRANSPARENT);
		RenderText(g_hMemDC);
		RenderTree(g_hMemDC);
		{
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
