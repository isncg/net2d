#include "ui.h"

static ATOM RegisterWindowClasses(HINSTANCE hInstance)
{
	/*WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = 0;
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
	wcex.hCursor = 0;
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = render_window_class_name;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex)) {
		return FALSE;
	}
*/
	return TRUE;

}





BOOL CreateUI()
{
	return 0;
}















namespace UI {
	HWND MainWindow;
	HWND GraphicsArea;
	HWND Header;
	HWND StatusBar;
};


