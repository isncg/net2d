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
	testline.p0.x = 100;
	testline.p0.y = -200;
	testline.p1.x = 200;
	testline.p1.y = 300;

	SendNetLineMessage(hWnd, 0, testline);

	MSG msg = { 0 };


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	PAINTSTRUCT ps;
	HDC hdc;

	
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		view->Init(hdc);
		view->DrawBackground(model);
		view->PaintModel(model);
		view->DrawForeground(model);

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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = name;
	wcex.hIconSm = 0;
	return RegisterClassEx(&wcex);

}
