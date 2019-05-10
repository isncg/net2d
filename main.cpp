#include "net.h"
#include "message.h"
#include "graphics.h"
#include "Database.h"
#include<chrono>

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HWND hWnd;


ATOM RegisterMainWindow(const char * name, HINSTANCE hInstance);

Model* model;
View* view;
Controller* controller;


auto wind_class_name = "N2D_WINDOW_CLASS";
auto wind_tittle = "N2D";

#include<iostream>
HDC backdc = NULL;
HBITMAP backbuffer = NULL;


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
		printf("SAVED: x=%f y=%f\n", i->x, i->y, i->size=10);
	}


	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	NetThread udp(RunUDP);


	model = new Model();
	view = new AxisView();
	controller = new Controller(model);


	HINSTANCE hInstance = GetModuleHandle(NULL);


	if (!RegisterMainWindow(wind_class_name, hInstance)) {
		return -1;
	}


	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(wind_class_name, wind_tittle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!hWnd)
	{
		return -1;
	}	//return E_FAIL;


	ShowWindow(hWnd, SW_SHOW);


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
		else
		{
			HDC hdc = GetDC(hWnd);
			if (!backdc) {
				backdc = CreateCompatibleDC(hdc);
				if (backbuffer)
					DeleteObject(backbuffer);
				backbuffer = CreateCompatibleBitmap(hdc, 4096, 4096);
				SelectObject(backdc, backbuffer);
			}


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

			}
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
		}

	}


	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	RECT rc;
	UINT width;
	UINT height;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
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

ATOM RegisterMainWindow(const char * name, HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = 0;
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = name;
	wcex.hIconSm = 0;
	return RegisterClassEx(&wcex);

}
