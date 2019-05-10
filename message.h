#pragma once


#include<Windows.h>
#include<vector>
#include<list>

#define NM_CLEAR GET_NM_CLEAR()
#define NM_POINT GET_NM_POINT()
#define NM_POINT_LIST GET_NM_POINT_LIST()
#define NM_LINE GET_NM_LINE()

UINT GET_NM_CLEAR();
UINT GET_NM_POINT();
UINT GET_NM_POINT_LIST();
UINT GET_NM_LINE();


struct POINT_MESSAGE {
	float x;
	float y;
	float size = 1;
	UINT frame_id;
	UINT frame_index;
	UINT type = 0;
	UINT time_stamp;
};


struct LINE_MESSAGE {
	POINT_MESSAGE p0;
	POINT_MESSAGE p1;
};


//struct PAINT_LAYER {
//	std::vector<POINT_MESSAGE> point_buffer;
//	std::vector<LINE_MESSAGE> line_buffer;
//};
//
//struct PAINT_LAYER_STACK {
//	std::vector<PAINT_LAYER*> layers;
//};

HRESULT SendNetPointMessage(HWND hWnd, UINT layer, POINT_MESSAGE& msgdata);

HRESULT SendNetLineMessage(HWND hWnd, UINT layer, LINE_MESSAGE& msgdata);

HRESULT SendNetPointListMessage(HWND hWnd, UINT layer, std::list<POINT_MESSAGE> & point_list);

HRESULT SendNetClearMessage(HWND hWnd, UINT layer);

//void CreatePaintLayer(int count, PAINT_LAYER_STACK** result);
//
//BOOL OnNetMessage(PAINT_LAYER_STACK * layer_stack, UINT message, WPARAM wParam, LPARAM lParam);

void RegisterNetMessage();