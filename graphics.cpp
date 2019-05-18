#include "graphics.h"
#include "message.h"
#include<memory>
#pragma comment (lib,"Gdiplus.lib")

extern HWND hWnd;

void  NetPoint::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
{
	pGraphics->DrawEllipse(pen, Gdiplus::RectF(this->x - this->size / 2, this->y - this->size / 2, this->size, this->size));
}

NetPoint::NetPoint(float x, float y, float size, DWORD id) :
	x(x), y(y), size(size), id(id)
{}

NetPoint::NetPoint() :
	x(0), y(0), size(0), id(0)
{}

void  NetLine::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
{
	pGraphics->DrawLine(pen, Gdiplus::PointF(this->start.x, this->start.y), Gdiplus::PointF(this->end.x, this->end.y));
}

NetLine::NetLine()
{
}

void  NetLineStrip::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
{
	static std::vector<Gdiplus::PointF> buffer;
	buffer.resize(this->points.size());

	for (int i = 0; i < this->points.size(); i++) {
		buffer[i].X = this->points[i].x;
		buffer[i].Y = this->points[i].y;
	}

	pGraphics->DrawLines(pen, &buffer[0], buffer.size());
}


Gdiplus::Pen *  INetDrawable::GetDefaultPen()
{
	static Gdiplus::Pen * default_pen = NULL;
	if (!default_pen) {
		default_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0));
	}
	return default_pen;
}

void  INetDrawable::Draw(Gdiplus::Graphics * pGraphics)
{
	auto pen = this->pen;
	if (!pen) {
		pen = GetDefaultPen();
	}
	this->Draw(pGraphics, pen);
}



void View::Init(HDC hdc)
{
	if (!external_graphics) {
		delete pGraphics;
	}

	this->pGraphics = new Gdiplus::Graphics(hdc);
	external_graphics = false;
}

void View::Cleanup()
{
	if (!external_graphics) {
		delete pGraphics;
	}
	pGraphics = NULL;
}

View::View(Gdiplus::Graphics * pGraphics)
{
	this->pGraphics = pGraphics;
	external_graphics = true;
}

View::View(HDC hdc)
{
	this->pGraphics = new Gdiplus::Graphics(hdc);
	external_graphics = false;
}

View::View()
{
	this->external_graphics = false;
	this->pGraphics = NULL;
}

void View::ResetTransform(ELEMENT_TRANSFORM * transform)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	pGraphics->ResetTransform();
	pGraphics->TranslateTransform((rc.right - rc.left) / 2.0, (rc.bottom - rc.top) / 2.0);
	pGraphics->TranslateTransform(transform->x, transform->y);
	pGraphics->ScaleTransform(transform->scale, -transform->scale);
	pGraphics->RotateTransform(transform->rotate);

}

void View::PaintLayer(Layer * layer)
{
	for (auto e = layer->elements.begin(); e != layer->elements.end(); e++) {
		(*e)->Draw(pGraphics);
	}
}
void View::PaintTransformedLayer(Layer * layer, ELEMENT_TRANSFORM * transform)
{

	if (!transform) {
		transform = &layer->transform;
	}

	ResetTransform(transform);
	PaintLayer(layer);

}

void View::PaintModel(Model * model)
{

	ResetTransform(&model->transform);

	for (auto i = model->layers.begin(); i != model->layers.end(); i++) {
		PaintLayer((*i));
	}

}

void View::DrawBackground(Model * model)
{
}

void View::DrawForeground(Model * model)
{
}

View::~View()
{
	Cleanup();
}

void AxisView::DrawBackground(Model * model)
{
	//ResetTransform(&model->transform);
	Gdiplus::Pen pen(Gdiplus::Color(128, 128, 128));
	Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
	RECT rc;
	GetClientRect(hWnd, &rc);
	pGraphics->ResetTransform();
	float cx = model->transform.x + rc.right / 2.0f;
	float cy = model->transform.y + rc.bottom / 2.0f;
	pGraphics->DrawLine(&pen, 0.0f, cy, (float)rc.right, cy);
	pGraphics->DrawLine(&pen, cx, 0.0f, cx, (float)rc.bottom);

	auto step = 100.0*model->transform.scale;
	while (abs(step / 10.0 - 100.0) < abs(step - 100.0))
	{
		step /= 10;
	}

	while (abs(step*10.0 - 100.0) < abs(step - 100.0))
	{
		step *= 10;
	}

	float vx = fmod(cx, step);
	while (vx < rc.right)
	{
		pGraphics->DrawLine(&pen, vx, cy - 5, vx, cy + 5);
		vx += step;
	}

	float vy = fmod(cy, step);
	while (vy < rc.bottom)
	{
		pGraphics->DrawLine(&pen, cx - 5, vy, cx + 5, vy);
		vy += step;
	}

	if (cx <= 0 || cx >= rc.right || cy <= 0 || cy >= rc.bottom) {

		if (cx < 0) {
			cx = 0;
		}
		else if (cx > rc.right) {
			cx = rc.right;
		}
		if (cy < 0) {
			cy = 0;
		}
		else if (cy > rc.bottom) {
			cy = rc.bottom;
		}

		Gdiplus::PointF points[]{
			Gdiplus::PointF(cx, cy + 10),
			Gdiplus::PointF(cx + 10, cy),
			Gdiplus::PointF(cx, cy - 10),
			Gdiplus::PointF(cx - 10, cy),
		};
		pGraphics->FillPolygon(&brush, points, 4);
	}


}




Controller::Controller(Model * model) :
	model(model)
{
}


template <class T>
auto lparam_ptr(LPARAM lParam) {
	return std::auto_ptr<T>((T*)lParam);
}

BOOL Controller::OnNetMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	int layer_index = wParam;
	auto layers = this->model->layers;
	if (layer_index < 0 || layer_index >= layers.size()) {
		return FALSE;
	}


	if (message == NM_POINT)
	{
		auto pm = lparam_ptr<POINT_MESSAGE>(lParam);
		layers[layer_index]->elements.push_back(new  NetPoint(pm->x, pm->y, pm->size, 0));
		return TRUE;
	}

	else if (message == NM_POINT_LIST) {
		auto pm = lparam_ptr<std::list<POINT_MESSAGE>>(lParam);
		for (auto i = pm->begin(); i != pm->end(); i++) {
			layers[layer_index]->elements.push_back(new  NetPoint(i->x, i->y, i->size, 0));
		}
		return TRUE;
	}

	else if (message == NM_LINE) {
		auto lm = lparam_ptr<LINE_MESSAGE>(lParam);
		auto line = new  NetLine();
		line->start = NetPoint(lm->p0.x, lm->p0.y, lm->p0.size, 0);
		line->end = NetPoint(lm->p1.x, lm->p1.y, lm->p1.size, 0);

		layers[layer_index]->elements.push_back(line);
		return TRUE;
	}

	else if (message == NM_CLEAR) {
		layers[layer_index]->elements.clear();
		return TRUE;
	}
	return FALSE;
}


BOOL Controller::OnViewTransform(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (WM_LBUTTONDOWN == message) {
		isDrag = true;
		old_transform_x = model->transform.x;
		old_transform_y = model->transform.y;
		drag_enter_x = cur_mouse_x;
		drag_enter_y = cur_mouse_y;

		return TRUE;

	}
	else if (WM_LBUTTONUP == message) {
		isDrag = false;
		return TRUE;
	}

	else if (WM_MOUSELEAVE == message) {
		isDrag = false;
		return TRUE;
	}

	else if (WM_MOUSEMOVE == message) {

		cur_mouse_x = LOWORD(lParam);
		cur_mouse_y = HIWORD(lParam);

		if (isDrag) {
			model->transform.x = old_transform_x + (cur_mouse_x - drag_enter_x);
			model->transform.y = old_transform_y + (cur_mouse_y - drag_enter_y);
		}

		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = hWnd;
		tme.dwHoverTime = HOVER_DEFAULT;
		TrackMouseEvent(&tme);
		return TRUE;

	}
	else if (WM_CHAR == message) {
		auto old_scale = model->transform.scale;
		auto new_scale = 1.0f;

		if (wParam == '=') {
			new_scale = old_scale * 1.2;
		}
		else if (wParam == '-') {
			new_scale = old_scale / 1.2;
		}

		if (new_scale < 0.01) {
			new_scale = 0.01;
		}
		if (new_scale > 100) {
			new_scale = 100;
		}



		RECT rc;
		GetClientRect(hWnd, &rc);
		float cx = model->transform.x;
		float cy = model->transform.y;
		float zoom_rate = new_scale / old_scale;
		float shift_x = (zoom_rate - 1) * cx;
		float shift_y = (zoom_rate - 1) * cy;
		model->transform.x += shift_x;
		model->transform.y += shift_y;

		model->transform.scale = new_scale;

		if (1 || wParam == '=' || wParam == '-')
		{
			return TRUE;
		}
		return FALSE;
	}

	return FALSE;
}

Model::Model(int layer_count)
{
	while (layer_count-- > 0)
	{
		this->layers.push_back(new Layer);
	}
}

Model::Model()
{
	this->layers.push_back(new Layer);
}

