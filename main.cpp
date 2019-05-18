#include "net.h"
#include "message.h"
#include "graphics.h"
#include "Database.h"
#include<chrono>

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RenderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ResizeRenderingArea();

HWND hWnd;
HWND hMainWnd;

ATOM RegisterWindowClasses(HINSTANCE hInstance);

Model* model;
View* view;
Controller* controller;


auto main_window_class_name = "N2D_WINDOW_CLASS";
auto render_window_class_name = "N2D_RENDERING_CLASS";

auto wind_tittle = "N2D";

#include<iostream>
HDC backdc = NULL;
HBITMAP backbuffer = NULL;


void Render(HDC hdc);

int main() {
	RegisterNetMessage();


	Database db("data.sqlite");
	db.PutData(0, 10, 10, 0, 0, 0);
	db.PutData(0, 20, 10, 0, 0, 1);
	db.PutData(0, 30, 20, 0, 0, 2);
	db.PutData(0, 40, 40, 0, 0, 3);
	db.PutData(0, 50, 60, 0, 0, 4);


	std::list <POINT_MESSAGE> saved;
	db.GetData(saved);

	for (auto i = saved.begin(); i != saved.end(); i++) {
		printf("SAVED: x=%f y=%f\n", i->x, i->y, i->size = 10);
	}


	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	NetThread udp(RunUDP);


	model = new Model();
	view = new AxisView();
	controller = new Controller(model);


	HINSTANCE hInstance = GetModuleHandle(NULL);


	if (!RegisterWindowClasses(hInstance)) {
		return -1;
	}


	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hMainWnd = CreateWindow(main_window_class_name, wind_tittle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!hMainWnd)
	{
		return -1;
	}	//return E_FAIL;


	ShowWindow(hMainWnd, SW_SHOW);


	hWnd = CreateWindow(render_window_class_name, "DRAW", WS_CHILD, 0, 0, 200, 300, hMainWnd, 0, hInstance, 0);
	ShowWindow(hWnd, SW_SHOW);
	ResizeRenderingArea();
	SetWindowLong(hWnd, GWL_WNDPROC, (LONG)RenderProc);

	LINE_MESSAGE testline;
	testline.p0.x = 0;
	testline.p0.y = 0;
	testline.p1.x = 400;
	testline.p1.y = 300;

	SendNetLineMessage(hWnd, 0, testline);
	SendNetPointListMessage(hWnd, 0, saved);
	/*MSG msg = { 0 };


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		auto enter_time = std::chrono::high_resolution_clock::now();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/*	else
			{*/
			//HDC hdc = GetDCEx(hWnd, NULL, DCX_WINDOW);
		HDC hdc = GetDC(hWnd);
		if (!backdc) {
			backdc = CreateCompatibleDC(hdc);
			if (backbuffer)
				DeleteObject(backbuffer);
			backbuffer = CreateCompatibleBitmap(hdc, 4096, 4096);
			SelectObject(backdc, backbuffer);
		}

		Render(hdc);

		//if (backdc) {

		//	RECT rc;
		//	GetClientRect(hWnd, &rc);
		//	FillRect(backdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

		//	view->Init(backdc);

		//	view->DrawBackground(model);
		//	view->PaintModel(model);
		//	view->DrawForeground(model);
		//	view->Cleanup();

		//	auto width = rc.right - rc.left;
		//	auto height = rc.bottom - rc.top;

		//	/*	Gdiplus::Graphics graphics(backdc);
		//		Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 255));
		//		graphics.DrawLine(&pen, 0, 0, 200, 300);*/
		//	BitBlt(hdc, 0, 0, width, height, backdc, 0, 0, SRCCOPY);
		///*	MoveToEx(hdc, 0, 0, 0);
		//	LineTo(hdc, 200, 300);*/
		//	//UpdateWindow(hSubWnd);
		//}
		ReleaseDC(hWnd, hdc);
		auto exit_time = std::chrono::high_resolution_clock::now();
		auto delta_time = exit_time - enter_time;

		int sleep_time = 5 - delta_time.count() / 1000000;
		if (sleep_time > 0) {
			Sleep(sleep_time);
		}


		/*	exit_time = std::chrono::high_resolution_clock::now();
			delta_time = exit_time - enter_time;*/
			//printf("FPS: %.2lf  sleep: %d\n", 1000000000.0 / delta_time.count(), sleep_time);
	//}

	}


	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}


void Render(HDC hdc) {
	if (backdc) {

		RECT rc;
		GetClientRect(hWnd, &rc);
		FillRect(backdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

		view->Init(backdc);

		view->DrawBackground(model);
		view->PaintModel(model);
		view->DrawForeground(model);
		view->Cleanup();

		auto width = rc.right - rc.left;
		auto height = rc.bottom - rc.top;

		/*	Gdiplus::Graphics graphics(backdc);
			Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 255));
			graphics.DrawLine(&pen, 0, 0, 200, 300);*/
		BitBlt(hdc, 0, 0, width, height, backdc, 0, 0, SRCCOPY);
		/*	MoveToEx(hdc, 0, 0, 0);
			LineTo(hdc, 200, 300);*/
			//UpdateWindow(hSubWnd);
	}
	/*ReleaseDC(hWnd, hdc);
	auto exit_time = std::chrono::high_resolution_clock::now();
	auto delta_time = exit_time - enter_time;

	int sleep_time = 5 - delta_time.count() / 1000000;
	if (sleep_time > 0) {
		Sleep(sleep_time);
	}*/
}

LRESULT CALLBACK RenderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		Render(hdc);
		EndPaint(hWnd, &ps);
		break;

	default:
		if (controller->OnNetMessage(message, wParam, lParam)) {
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}

		if (controller->OnViewTransform(message, wParam, lParam)) {
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;

}

void ResizeRenderingArea() {
	RECT rc;
	GetClientRect(hMainWnd, &rc);
	rc.top = 100;
	SetWindowPos(::hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	RECT rc;
	UINT width;
	UINT height;

	MINMAXINFO* pMMI = NULL;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		ResizeRenderingArea();
		//InvalidateRect(::hWnd, NULL, FALSE);
		break;

	case WM_GETMINMAXINFO:
		pMMI = (MINMAXINFO*)lParam;
		pMMI->ptMinTrackSize.x = 400;
		pMMI->ptMinTrackSize.y = 300;
		break;

	default:

		/*	if (controller->OnNetMessage(message, wParam, lParam)) {
				InvalidateRect(hWnd, NULL, FALSE);
				return 0;
			}

			if (controller->OnViewTransform(message, wParam, lParam)) {
				InvalidateRect(hWnd, NULL, FALSE);
				return 0;
			}*/

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

ATOM RegisterWindowClasses(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = main_window_class_name;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex)) {
		return FALSE;
	}

	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_HAND);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = render_window_class_name;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex)) {
		return FALSE;
	}

	return TRUE;

}
