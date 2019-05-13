#include "message.h"



UINT _NM_CLEAR = 0;

UINT _NM_POINT = 0;
UINT _NM_LINE = 0;
UINT _NM_POINT_LIST = 0;


UINT GET_NM_CLEAR()
{
	return _NM_CLEAR;
}

UINT GET_NM_POINT()
{
	return _NM_POINT;
}

UINT GET_NM_POINT_LIST()
{
	return _NM_POINT_LIST;
}

UINT GET_NM_LINE()
{
	return _NM_LINE;
}

HRESULT  SendNetPointMessage(HWND hWnd, UINT layer, POINT_MESSAGE & msgdata) {
	auto m = new POINT_MESSAGE;
	*m = msgdata;
	return SendMessage(hWnd, _NM_POINT, layer, (LPARAM)m);

}

HRESULT SendNetLineMessage(HWND hWnd, UINT layer, LINE_MESSAGE & msgdata) {
	auto m = new LINE_MESSAGE;
	*m = msgdata;
	return SendMessage(hWnd, _NM_LINE, layer, (LPARAM)m);
}

HRESULT SendNetPointListMessage(HWND hWnd, UINT layer, std::list<POINT_MESSAGE>& point_list)
{
	auto m = new std::list<POINT_MESSAGE>;
	m->assign(point_list.begin(), point_list.end());
	return SendMessage(hWnd, _NM_POINT_LIST, layer, (LPARAM)m);
}

HRESULT SendNetClearMessage(HWND hWnd, UINT layer)
{
	return SendMessage(hWnd, _NM_CLEAR, layer, 0);
}

//void CreatePaintLayer(int count, PAINT_LAYER_STACK ** result)
//{
//	auto l = new PAINT_LAYER_STACK;
//	for (int i = 0; i < count; i++) {
//		l->layers.push_back(new PAINT_LAYER);
//	}
//	*result = l;
//}

//BOOL OnNetMessage(PAINT_LAYER_STACK * layer_stack, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	int layer = wParam;
//	if (layer < 0 || layer >= layer_stack->layers.size()) {
//		return FALSE;
//	}
//
//	//switch (message) {
//	if (message == _NM_POINT)
//	{
//		POINT_MESSAGE pm = *((POINT_MESSAGE*)lParam);
//		layer_stack->layers[layer]->point_buffer.push_back(pm);
//			
//
//		free(((POINT_MESSAGE*)lParam));
//		return TRUE;
//	}
//
//
//	else if (message == _NM_LINE) {
//		LINE_MESSAGE lm = *((LINE_MESSAGE*)lParam);
//		layer_stack->layers[layer]->line_buffer.push_back(lm);
//		free(((LINE_MESSAGE*)lParam));
//		return TRUE;
//	}
//
//	else if (message == _NM_CLEAR) {
//		layer_stack->layers[layer]->point_buffer.clear();
//		layer_stack->layers[layer]->line_buffer.clear();
//		return TRUE;
//	}
//
//	return FALSE;
//}

void RegisterNetMessage()
{
	_NM_POINT = RegisterWindowMessage("NM_POINT");
	_NM_LINE = RegisterWindowMessage("NM_LINE");
	_NM_CLEAR = RegisterWindowMessage("NM_CLEAR");
	_NM_POINT_LIST = RegisterWindowMessage("NM_POINT_LIST");
}
