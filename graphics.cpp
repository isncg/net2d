#include "graphics.h"
#include "message.h"
#pragma comment (lib,"Gdiplus.lib")

extern HWND hWnd;

void  Point::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
{
	pGraphics->DrawEllipse(pen, Gdiplus::RectF(this->x - this->size / 2, this->y - this->size / 2, this->size, this->size));
}

Point::Point(float x, float y, float size, DWORD id) :
	x(x), y(y), size(size), id(id)
{}

Point::Point() :
	x(0), y(0), size(0), id(0)
{}

void  Line::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
{
	pGraphics->DrawLine(pen, Gdiplus::PointF(this->start.x, this->start.y), Gdiplus::PointF(this->end.x, this->end.y));
}

Line::Line()
{
}

void  LineStrip::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
{
	static std::vector<Gdiplus::PointF> buffer;
	buffer.resize(this->points.size());

	for (int i = 0; i < this->points.size(); i++) {
		buffer[i].X = this->points[i].x;
		buffer[i].Y = this->points[i].y;
	}

	pGraphics->DrawLines(pen, &buffer[0], buffer.size());
}

//void Layer::GetMatrix(Gdiplus::Matrix & matrix)
//{
//	matrix.IsIdentity()
//}

Gdiplus::Pen *  IDrawable::GetDefaultPen()
{
	static Gdiplus::Pen * default_pen = NULL;
	if (!default_pen) {
		default_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0));
	}
	return default_pen;
}

void  IDrawable::Draw(Gdiplus::Graphics * pGraphics)
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
	if (!external_graphics) {
		delete pGraphics;
	}
	pGraphics = NULL;
}

void AxisView::DrawBackground(Model * model)
{
	ResetTransform(&model->transform);
	Gdiplus::Pen pen(Gdiplus::Color(128, 128, 128));
	pGraphics->DrawLine(&pen, -100, 0, 100, 0);
	pGraphics->DrawLine(&pen, 0, -100, 0, 100);
}




Controller::Controller(Model * model) :
	model(model)
{
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
		POINT_MESSAGE* pm = (POINT_MESSAGE*)lParam;
		layers[layer_index]->elements.push_back(new  Point(pm->x, pm->y, pm->size, 0));

		delete pm;
		return TRUE;
	}

	else if (message == NM_POINT_LIST) {
		puts("POINT_LIST");
		std::list<POINT_MESSAGE>* pm = (std::list<POINT_MESSAGE>*)lParam;
		for (auto i = pm->begin(); i != pm->end(); i++) {
			layers[layer_index]->elements.push_back(new  Point(i->x, i->y, i->size, 0));
		}
		delete pm;
		return TRUE;
	}

	else if (message == NM_LINE) {
		LINE_MESSAGE* lm = (LINE_MESSAGE*)lParam;
		auto line = new  Line();
		line->start = Point(lm->p0.x, lm->p0.y, lm->p0.size, 0);
		line->end = Point(lm->p1.x, lm->p1.y, lm->p1.size, 0);

		layers[layer_index]->elements.push_back(line);
		//layer_stack->layers[layer_index]->line_buffer.push_back(lm);
		delete lm;
		return TRUE;
	}

	else if (message == NM_CLEAR) {
		layers[layer_index]->elements.clear();
		return TRUE;
	}
	return FALSE;
}


#include <iostream>
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
	else if (WM_MOUSEMOVE == message) {

		cur_mouse_x = LOWORD(lParam);
		cur_mouse_y = HIWORD(lParam);

		if (isDrag) {
			model->transform.x = old_transform_x + (cur_mouse_x - drag_enter_x);
			model->transform.y = old_transform_y + (cur_mouse_y - drag_enter_y);
			//std::cout << model->transform.x << ", " << model->transform.y << std::endl;
			//InvalidateRect(hWnd, NULL, TRUE);
		}

		return TRUE;

	}
	else if (WM_CHAR == message) {
		puts("WM_CHAR: ");
		putc(wParam, stdout);
		putc('\n', stdout);
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
		/*std::cout << "C    _XY " << cx << " " << cy << std::endl;
		std::cout << "SHIFT_XY " << shift_x << " " << shift_y << std::endl;*/
		model->transform.x += shift_x;
		model->transform.y += shift_y;

		model->transform.scale = new_scale;

		if (1 || wParam == '=' || wParam == '-')
		{
			//InvalidateRect(hWnd, &rc, TRUE);
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

