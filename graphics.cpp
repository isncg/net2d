#include "graphics.h"
#include "message.h"
#pragma comment (lib,"Gdiplus.lib")

extern HWND hWnd;

void element::Point::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
{
	pGraphics->DrawEllipse(pen, Gdiplus::RectF(this->x, this->y, this->size, this->size));
}

element::Point::Point(float x, float y, float size, DWORD id) :
	x(x), y(y), size(size), id(id)
{}

element::Point::Point() :
	x(0), y(0), size(0), id(0)
{}

void element::Line::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
{
	pGraphics->DrawLine(pen, Gdiplus::PointF(this->start.x, this->start.y), Gdiplus::PointF(this->end.x, this->end.y));
}

element::Line::Line()
{
}

void element::LineStrip::Draw(Gdiplus::Graphics * pGraphics, Gdiplus::Pen * pen)
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

Gdiplus::Pen * element::IDrawable::GetDefaultPen()
{
	static Gdiplus::Pen * default_pen = NULL;
	if (!default_pen) {
		default_pen = new Gdiplus::Pen(Gdiplus::Color(255, 0, 0, 0));
	}
	return default_pen;
}

void element::IDrawable::Draw(Gdiplus::Graphics * pGraphics)
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
	pGraphics->ResetTransform();
	pGraphics->ScaleTransform(transform->scale, transform->scale);
	pGraphics->RotateTransform(transform->rotate);
	pGraphics->TranslateTransform(transform->x, transform->y);
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
		POINT_MESSAGE pm = *((POINT_MESSAGE*)lParam);
		layers[layer_index]->elements.push_back(new element::Point(pm.x, pm.y, pm.size, 0));

		free(((POINT_MESSAGE*)lParam));
		return TRUE;
	}


	else if (message == NM_LINE) {
		LINE_MESSAGE lm = *((LINE_MESSAGE*)lParam);
		auto line = new element::Line();
		line->start = element::Point(lm.p0.x, lm.p0.y, lm.p0.size, 0);
		line->end = element::Point(lm.p1.x, lm.p1.y, lm.p1.size, 0);

		layers[layer_index]->elements.push_back(line);
		//layer_stack->layers[layer_index]->line_buffer.push_back(lm);
		free(((LINE_MESSAGE*)lParam));
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
	switch (message)
	{


	case WM_LBUTTONDOWN:
		isDrag = true;
		old_transform_x = model->transform.x;
		old_transform_y = model->transform.y;
		drag_enter_x = cur_mouse_x;
		drag_enter_y = cur_mouse_y;

		return TRUE;

	case WM_LBUTTONUP:
		isDrag = false;

		return TRUE;




	case WM_MOUSEMOVE:

		cur_mouse_x = LOWORD(lParam);
		cur_mouse_y = HIWORD(lParam);

		if (isDrag) {
			model->transform.x = old_transform_x + (cur_mouse_x - drag_enter_x);
			model->transform.y = old_transform_y + (cur_mouse_y - drag_enter_y);
			//std::cout << model->transform.x << ", " << model->transform.y << std::endl;
			InvalidateRect(hWnd, NULL, TRUE);
		}

		return TRUE;

	default:
		break;

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

