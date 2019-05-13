#pragma once
#include<Windows.h>
#include<gdiplus.h>
#include<vector>



class INetDrawable {
protected:
	Gdiplus::Pen* GetDefaultPen();
public:
	Gdiplus::Pen* pen = NULL;
	virtual void Draw(Gdiplus::Graphics* pGraphics, Gdiplus::Pen* pen) = 0;
	void Draw(Gdiplus::Graphics* pGraphics);
};

class NetPoint : public INetDrawable {
public:
	virtual void Draw(Gdiplus::Graphics* pGraphics, Gdiplus::Pen* pen) override;
	float x;
	float y;
	float size;
	DWORD id;
	NetPoint(float x, float y, float size, DWORD id);
	NetPoint();
};


class NetLine : public INetDrawable {
	virtual void Draw(Gdiplus::Graphics* pGraphics, Gdiplus::Pen* pen) override;
public:
	NetPoint start;
	NetPoint end;
	NetLine();
};

class NetLineStrip : public INetDrawable {
	virtual void Draw(Gdiplus::Graphics* pGraphics, Gdiplus::Pen* pen) override;
public:
	std::vector<NetPoint> points;
};


struct ELEMENT_TRANSFORM {
	float x = 0;
	float y = 0;
	float scale = 1;
	float rotate = 0;
};

class Layer {
public:
	ELEMENT_TRANSFORM transform;
	std::vector<INetDrawable*> elements;

};


class Model {
public:
	ELEMENT_TRANSFORM transform;
	std::vector<Layer*> layers;

	Model(int layer_count);
	Model();

};

class Controller {

	Model* model;


	bool isDrag = false;
	int drag_enter_x = 0;
	int drag_enter_y = 0;
	int old_transform_x = 0;
	int old_transform_y = 0;
	int cur_mouse_x = 0;
	int cur_mouse_y = 0;

public:
	Controller(Model* model);

	BOOL OnNetMessage(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnViewTransform(UINT message, WPARAM wParam, LPARAM lParam);
};





class View {
	bool external_graphics = false;

protected:
	Gdiplus::Graphics* pGraphics;
public:

	void Init(HDC hdc);
	void Cleanup();

	View(Gdiplus::Graphics* pGraphics);
	View(HDC hdc);
	View();

	void ResetTransform(ELEMENT_TRANSFORM* transform);
	void PaintLayer(Layer* layer);
	void PaintTransformedLayer(Layer* layer, ELEMENT_TRANSFORM* transform);
	void PaintModel(Model* model);

	virtual void DrawBackground(Model * model);
	virtual void DrawForeground(Model * model);
	~View();
};


class AxisView : public View {
	virtual void DrawBackground(Model * model) override;
};

