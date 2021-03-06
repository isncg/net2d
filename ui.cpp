#include "ui.h"

LRESULT CALLBACK    MainWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GraphicsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HeaderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

auto main_window_class_name = "N2D_WINDOW_CLASS";
auto render_window_class_name = "N2D_RENDERING_CLASS";
auto header_window_class_name = "N2D_HEADER_CLASS";

static ATOM RegisterWindowClasses(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);;
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
	wcex.lpfnWndProc = GraphicsProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_HAND);;
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = render_window_class_name;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex)) {
		return FALSE;
	}

	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = HeaderProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);;
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = header_window_class_name;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex)) {
		return FALSE;
	}

	return TRUE;

}


auto wind_tittle = "N2D";
auto header_size = 64;


BOOL CreateUI(HINSTANCE hInstance)
{
	UI::images::header_normal = Gdiplus::Image::FromFile(L"./img/header_normal.bmp");
	UI::images::header_selected = Gdiplus::Image::FromFile(L"./img/header_selected.bmp");
	
	
	auto style = WS_OVERLAPPEDWINDOW;
	
	
	RegisterWindowClasses(hInstance);
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, style, FALSE);
	UI::MainWindow = CreateWindow(main_window_class_name, wind_tittle, style,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);


	UI::GraphicsArea = CreateWindow(render_window_class_name, "DRAW", WS_CHILD, 0, 0, 200, 300, UI::MainWindow, 0, hInstance, 0);

	GetClientRect(UI::MainWindow, &rc);
	rc.top = header_size;
	SetWindowPos(UI::GraphicsArea, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 0);


	UI::Header = CreateWindow(header_window_class_name, "HEADER", WS_CHILD, 0, 0, 800, 64, UI::MainWindow, 0, hInstance, 0);

	ShowWindow(UI::MainWindow, SW_SHOW);
	ShowWindow(UI::GraphicsArea, SW_SHOW);
	ShowWindow(UI::Header, SW_SHOW);

	return TRUE;
}

HDC backdc = NULL;
HBITMAP backbuffer = NULL;

#include "graphics.h"
extern Model* model;
extern View* view;
extern Controller* controller;

void Render(HDC hdc) {
	if (backdc) {

		RECT rc;
		GetClientRect(UI::GraphicsArea, &rc);
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

LRESULT CALLBACK GraphicsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
	GetClientRect(UI::MainWindow, &rc);
	SetWindowPos(UI::GraphicsArea, NULL, rc.left, header_size, rc.right - rc.left, rc.bottom - rc.top, 0);

	SetWindowPos(UI::Header, NULL, rc.left, rc.top, rc.right, header_size, 0);
}


LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		break;

	case WM_GETMINMAXINFO:
		pMMI = (MINMAXINFO*)lParam;
		pMMI->ptMinTrackSize.x = 400;
		pMMI->ptMinTrackSize.y = 300;
		break;

	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}


LRESULT CALLBACK HeaderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	HDC hdc;

	Gdiplus::Graphics* graphics;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		graphics = new Gdiplus::Graphics(hdc);
		graphics->DrawImage(UI::images::header_normal, 0, 0, 800, 64);
		EndPaint(hWnd, &ps);

		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}










namespace UI {
	HWND MainWindow;
	HWND GraphicsArea;
	HWND Header;
	HWND StatusBar;
	namespace images {

		Gdiplus::Image* header_normal;
		Gdiplus::Image* header_selected;
	}
};


