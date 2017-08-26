#include "CImg.h"
#include <iostream>
#include <algorithm> 
#include <cstdio>
#include <string>
#include <vector>

using namespace std;
using namespace cimg_library;

struct Coordinate
{
	float x;
	float y;
	Coordinate(){}; 
	Coordinate(float x_, float y_) : x(x_), y(y_) {}; 
};

class Hinh
{
public:
	virtual void Draw() = 0;
	virtual void writeSVG(FILE* f) = 0;
	virtual void output() = 0;
};

class Line : public Hinh
{
private:
	Coordinate Start;
	Coordinate End;
public:
	virtual void Draw()
	{
		CImg<int> canvas(400, 300, 1, 255);
		canvas.fill(255);
		const unsigned char	color[] = { 0, 0, 255 };
		canvas.draw_line(Start.x, Start.y, End.x, End.y, color);
		CImgDisplay	 display(canvas, "Line", 0);
		while (!display.is_closed())
			display.wait();
	}

	void output()
	{
		cout << "<line x1 = \"" << Start.x << "\" " << "y1 = \"" << Start.y << "\" " << "x2 = \"" << Start.x << "\" " << "y2 = \"" << Start.y << "\" " << "stroke = \"rgb(0, 0, 255)\" " << "stroke-width = \"5\" />" << endl;
	}

	void writeSVG(FILE* f)
	{
		fprintf(f, "<line x1 = \"%0.4f\" ", Start.x);
		fprintf(f, "y1 = \"%f\" ", Start.y);
		fprintf(f, "x2 = \"%f\" ", End.x);
		fprintf(f, "y2 = \"%f\" ", End.y);
		fprintf(f, "stroke = \"rgb(0, 0, 255)\" ");
		fprintf(f, "stroke-width = \"5\" />");
		fprintf(f, "\n");
	}

	Line()
	{
		Start.x = 0;
		Start.y = 0;
		End.x = 0;
		End.y = 0;
	}

	Line(Coordinate A, Coordinate B)
	{
		Start = A;
		End = B;
	}
};

class Circle :public Hinh
{
private:
	Coordinate Center;
	float Radius;
public:
	void Draw()
	{
		CImg<int> canvas(400, 300, 1, 255);
		canvas.fill(255);
		const unsigned char	color[] = { 0, 0, 255 };
		canvas.draw_circle(Center.x, Center.y, Radius, color, 50);
		CImgDisplay	 display(canvas, "Circle", 0);
		while (!display.is_closed())
			display.wait();
	}

	void output()
	{
		cout << "<circle cx = \"" << Center.x << "\" " << "cy = \"" << Center.y << "\" " << "r = \"" << Radius << "\" " << "fill = \"rgb(0,255,0)\" " << "stroke-width = \"5\" />" << endl;
	}

	void writeSVG(FILE* f)
	{
		fprintf(f, "<circle cx = \"%0.4f\" ", Center.x);
		fprintf(f, "cy = \"%f\" ", Center.y);
		fprintf(f, "r = \"%f\" ", Radius);
		fprintf(f, "fill = \"rgb(0,255,0)\" ");
		fprintf(f, "stroke = \"rgb(0, 0, 255)\" ");
		fprintf(f, "stroke-width = \"5\" />");
		fprintf(f, "\n");
	}

	Circle()
	{
		Center.x = 0;
		Center.y = 0;
		Radius = 0;
	}

	Circle(Coordinate A, float Value)
	{
		Center = A;
		Radius = Value;
	}
};

void main()
{
	Coordinate I(100, 100);
	float R = 58.2; 
	Hinh* Tron = new Circle(I, R); 

	Tron->Draw();

	Coordinate A(2, 5); 
	Coordinate B(100.5, 204.8); 
	Hinh* DuongThang = new Line(A, B);

	DuongThang->output();
	DuongThang->Draw(); 

	return 0; 
	
}