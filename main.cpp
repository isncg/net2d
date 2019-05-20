#include "net.h"
#include "message.h"
#include "graphics.h"
#include "Database.h"
#include "ui.h"
#include<chrono>
#include<iostream>

LRESULT CALLBACK    MainWindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GraphicsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void ResizeRenderingArea();



Model* model;
View* view;
Controller* controller;


extern HDC backdc;
extern HBITMAP backbuffer;


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
	CreateUI(hInstance);

	LINE_MESSAGE testline;
	testline.p0.x = 0;
	testline.p0.y = 0;
	testline.p1.x = 400;
	testline.p1.y = 300;

	SendNetLineMessage(UI::GraphicsArea, 0, testline);
	SendNetPointListMessage(UI::GraphicsArea, 0, saved);


	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		auto enter_time = std::chrono::high_resolution_clock::now();
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else {
			HDC hdc = GetDC(UI::GraphicsArea);
			if (!backdc) {
				backdc = CreateCompatibleDC(hdc);
				if (backbuffer)
					DeleteObject(backbuffer);
				backbuffer = CreateCompatibleBitmap(hdc, 4096, 4096);
				SelectObject(backdc, backbuffer);
			}

			Render(hdc);

			ReleaseDC(UI::GraphicsArea, hdc);

			auto exit_time = std::chrono::high_resolution_clock::now();
			auto delta_time = exit_time - enter_time;

			int sleep_time = 5 - delta_time.count() / 1000000;
			if (sleep_time > 0) {
				Sleep(sleep_time);
			}
		}
	}


	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}
