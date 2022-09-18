#include "stdafx.h"
#include "JPSSimulator.h"
#include "JumpPointSearch_GDI.h"
#include <windowsx.h>

#define NAME_TITLE		L"JPSSimulator"
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
StraightLine g_Bresenham;
std::list<StraightLine::Point> g_StraightPath;

// Double Buffering Variable
HBITMAP g_hMemDCBitmap;
HBITMAP g_hMemDCBitmapOld;
HDC		g_hMemDC;
RECT	g_MemDCRect;

// Forward declarations of functions included in this code module:
VOID RenderJumpPointSearch(HDC hdc);
VOID RenderStraightPath(HDC hdc);
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
VOID RenderJumpPointSearch(HDC hdc)
{
	g_JPS.Render(hdc, g_ScreenX, g_ScreenY, g_Scale);
}
VOID RenderStraightPath(HDC hdc)
{
	SelectObject(hdc, GetStockObject(LTGRAY_BRUSH));
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	if (g_StraightPath.size() > 2)
	{
		for (auto iter = g_StraightPath.begin(); iter != g_StraightPath.end();)
		{
			int iX = (iter->xPos - g_ScreenX) * g_Scale;
			int iY = (iter->yPos - g_ScreenY) * g_Scale;
			Rectangle(hdc, iX, iY, iX + g_Scale, iY + g_Scale);
			++iter;
		}
		auto source = g_StraightPath.front();
		auto dest = g_StraightPath.back();
		MoveToEx(hdc, ((source.xPos - g_ScreenX) * g_Scale) + (g_Scale / 2), ((source.yPos - g_ScreenY) * g_Scale) + (g_Scale / 2), NULL);
		LineTo(hdc, ((dest.xPos - g_ScreenX) * g_Scale) + (g_Scale / 2), ((dest.yPos - g_ScreenY) * g_Scale) + (g_Scale / 2));
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
		g_Scale = SCALE_MIN;
		g_ScreenX = 0;
		g_ScreenY = 0;
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_ERASEALL:
			memset(g_Map, 0, sizeof(g_Map));
			g_StraightPath.clear();
			g_JPS.Cleanup();
			InvalidateRect(hWnd, NULL, false);
			break; 
		case IDM_ERASEOBSTACLE:
			memset(g_Map, 0, sizeof(g_Map));
			InvalidateRect(hWnd, NULL, false);
			break;
		case IDM_ERASESTRAIGHT:
			g_StraightPath.clear();
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

			if (g_bCtrl)
			{
				// 스크린 좌표 갱신을 위한 좌표 값 저장
				g_xPos = xPos;
				g_yPos = yPos;
			}
			else
			{
				// 첫 선택 타일이 장애물이면 지우기 모드
				// 첫 선택 타일이 장애물이 아니면 장애물 넣기 모드
				g_bErase = g_Map[iTileY][iTileX] == TRUE;
			}
			g_bDrag = TRUE;
		}
		break;
	case WM_LBUTTONUP:
		g_bDrag = FALSE;
		break;
	case WM_RBUTTONDOWN:
		// 브레즌햄 직선 알고리즘 처리를 위한 좌표 값 저장
		g_xPos = GET_X_LPARAM(lParam);
		g_yPos = GET_Y_LPARAM(lParam);
		g_StraightPath.clear();
		break;
	case WM_RBUTTONUP:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			int iTileSrcX = (g_xPos / g_Scale) + g_ScreenX;
			int iTileSrcY = (g_yPos / g_Scale) + g_ScreenY;
			int iTileDstX = (xPos / g_Scale) + g_ScreenX;
			int iTileDstY = (yPos / g_Scale) + g_ScreenY;

			// 브레즌햄 직선 알고리즘 처리
			if (iTileSrcX != iTileDstX || iTileSrcY != iTileDstY)
			{
				StraightLine::Point point;
				g_Bresenham.Line(iTileSrcX, iTileSrcY, iTileDstX, iTileDstY);
				while (g_Bresenham.GetPoint(&point))
				{
					g_StraightPath.emplace_back(point.xPos, point.yPos);
					g_Bresenham.NextPoint();
				}
				InvalidateRect(hWnd, NULL, false);
			}
		}
		break;
	case WM_MOUSEMOVE:
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			int iTileX = (xPos / g_Scale) + g_ScreenX;
			int iTileY = (yPos / g_Scale) + g_ScreenY;

			if (g_bDrag)
			{
				if (g_bCtrl)
				{
					// 스크린 좌표 갱신
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
				else
				{
					// 장애물 넣기 또는 지우기
					g_Map[iTileY][iTileX] = !g_bErase;

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

			// 클릭한 타일에 장애물이 없을 경우
			if (!g_Map[iTileY][iTileX])
			{
				// 길찾기 출발지 등록
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

			// 클릭한 타일에 장애물이 없을 경우
			if (!g_Map[iTileY][iTileX])
			{
				// 길찾기 도착지 등록
				g_JPS.SetDestination(iTileX, iTileY);
				InvalidateRect(hWnd, NULL, false);
			}
		}
		break;
	case WM_MBUTTONUP:
		SetTimer(hWnd, 1, 100, NULL);	// 길찾기 시작
		SendMessage(hWnd, WM_TIMER, 1, NULL);
		break;
	case WM_MOUSEWHEEL:
		if (g_bCtrl)
		{
			// 타일 스케일 및 스크린 좌표 갱신
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
				KillTimer(hWnd, 1);		// 길찾기 종료
			InvalidateRect(hWnd, NULL, false);
			break;
		default:
			break;
		}
		break;
	case WM_PAINT:
		{
			// 랜더링
			PatBlt(g_hMemDC, 0, 0, g_MemDCRect.right, g_MemDCRect.bottom, WHITENESS);
			SetBkMode(g_hMemDC, TRANSPARENT);
			RenderJumpPointSearch(g_hMemDC);
			RenderStraightPath(g_hMemDC);

			// 더블버퍼링
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
