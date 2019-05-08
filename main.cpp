#include "net.h"
#include "message.h"
#include "graphics.h"

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

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	NetThread udp(RunUDP);

	RegisterNetMessage();

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

	/*MSG msg = { 0 };


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}*/

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (backdc) {

				RECT rc;
				GetClientRect(hWnd, &rc);
				FillRect(backdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

				view->Init(backdc);
				view->DrawBackground(model);
				view->PaintModel(model);
				view->DrawForeground(model);
			}
		}
	}


	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}


void clear_back_buffer(HDC hdc) {

	if (!backdc) {
		backdc = CreateCompatibleDC(hdc);
		if (backbuffer)
			DeleteObject(backbuffer);
		backbuffer = CreateCompatibleBitmap(backdc, 4096, 4096);
		SelectObject(backdc, backbuffer);
		/*std::cout << "NEW_BUFFER_SIZE  " << width << "  " << height << std::endl;
		HGDIOBJ obj = SelectObject(backdc, backbuffer);
		DeleteObject(obj);*/
	}
	//else {
	//	GetObject(backbuffer, sizeof(BITMAP), &bitmap);
	//	std::cout << "BUFFER_SIZE  " << bitmap.bmWidth << "  " << bitmap.bmWidth << std::endl;
	//	if (bitmap.bmWidth < width || bitmap.bmHeight < height) {
	//		//DeleteObject(backbuffer);
	//		backbuffer = CreateCompatibleBitmap(backdc, width, height);
	//		std::cout << "NEW_BUFFER_SIZE  " << width << "  " << height << std::endl;
	//		HGDIOBJ obj = SelectObject(backdc, backbuffer);

	//		std::cout << "HANDLE_CHANGE  " << backbuffer << "  " << obj << std::endl;
	//		DeleteObject(obj);
	//		GetObject(backbuffer, sizeof(BITMAP), &bitmap);
	//		std::cout << "BUFFER_SIZE  " << bitmap.bmWidth << "  " << bitmap.bmWidth << std::endl;
	//	}
	//}

	RECT rc;
	GetClientRect(hWnd, &rc);
	FillRect(backdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
}


//void clear_buffer(HDC hdc) {
//	RECT rc;
//	GetClientRect(hWnd, &rc);
//	UINT width = rc.right - rc.left;
//	UINT height = rc.bottom - rc.top;
//	BITMAP bitmap;
//	ZeroMemory(&bitmap, sizeof(BITMAP));
//	if (!backdc) {
//		backdc = CreateCompatibleDC(hdc);
//		/*if (backbuffer)
//			DeleteObject(backbuffer);*/
//		backbuffer = CreateCompatibleBitmap(backdc, width, height);
//		std::cout << "NEW_BUFFER_SIZE  " << width << "  " << height << std::endl;
//		HGDIOBJ obj = SelectObject(backdc, backbuffer);
//		DeleteObject(obj);
//	}
//	else {
//		GetObject(backbuffer, sizeof(BITMAP), &bitmap);
//		std::cout << "BUFFER_SIZE  " << bitmap.bmWidth << "  " << bitmap.bmWidth << std::endl;
//		if (bitmap.bmWidth < width || bitmap.bmHeight < height) {
//			//DeleteObject(backbuffer);
//			backbuffer = CreateCompatibleBitmap(backdc, width, height);
//			std::cout << "NEW_BUFFER_SIZE  " << width << "  " << height << std::endl;
//			HGDIOBJ obj = SelectObject(backdc, backbuffer);
//
//			std::cout << "HANDLE_CHANGE  " << backbuffer << "  " << obj << std::endl;
//			DeleteObject(obj);
//			GetObject(backbuffer, sizeof(BITMAP), &bitmap);
//			std::cout << "BUFFER_SIZE  " << bitmap.bmWidth << "  " << bitmap.bmWidth << std::endl;
//		}
//	}
//	
//	FillRect(backdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
//}

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
		GetClientRect(hWnd, &rc);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;
		hdc = BeginPaint(hWnd, &ps);

		//clear_back_buffer(hdc);

		if (!backdc) {
			backdc = CreateCompatibleDC(hdc);
			if (backbuffer)
				DeleteObject(backbuffer);
			backbuffer = CreateCompatibleBitmap(backdc, 4096, 4096);
			SelectObject(backdc, backbuffer);
		}


		/*	view->Init(backdc);
			view->DrawBackground(model);
			view->PaintModel(model);
			view->DrawForeground(model);*/


			//std::cout << "BILTSIZE" << width << "  " << height << std::endl;
		BitBlt(hdc, 0, 0, width, height, backdc, 0, 0, SRCCOPY);

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
