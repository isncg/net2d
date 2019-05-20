#pragma once
#include <Windows.h>
#include <gdiplus.h>
namespace UI {
	extern HWND MainWindow;
	extern HWND GraphicsArea;
	extern HWND Header;
	extern HWND StatusBar;
	namespace images {
		extern Gdiplus::Image* header_normal;
		extern Gdiplus::Image* header_selected;
	}
};




BOOL CreateUI(HINSTANCE hInstance);
