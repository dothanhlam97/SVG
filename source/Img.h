#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream> 
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")
#include <stdint.h>
#include <string.h>
#include <Windows.h>
#include <set>
#include <cstdlib>
#include "rapidxml-1.13\rapidxml_utils.hpp"
#include <iosfwd>

using namespace std;
using namespace	Gdiplus;


class ImportSVG; 
class Properties;
class Attribute; 

class Shape 
{
public:
	Shape() {}
	friend class Img; 
	friend class Properties; 
	virtual void draw() = 0;  
	virtual void readSVG(rapidxml::xml_node<> *node) = 0; 
	virtual void writeSVG(ostream& out) = 0;
	virtual set <Attribute*> getProperties() = 0;
	virtual void addProperties(set <Attribute*> p) = 0;
};

class Img
{
private : 

	friend class Properties;
	friend class ImportSVG; 

	Img(const HDC& hdc){
		canvas = new Graphics(hdc);
	}

	Img(const Img& other);

	~Img(){
		if (instance != NULL){
			delete canvas;
			canvas = NULL;
		}
	}

public:
	static Graphics* canvas; 
	static Img* instance; 
	static void draw(Shape *src){
		src->draw();
	}
	static void draw(ImportSVG * svg);

	static Img *getInstance(const HDC& hdc){
		if (instance == NULL){
			instance = new Img(hdc);
			return instance;
		}
		return NULL;
	}

	static void destroy(){
		delete instance;
		instance = NULL;
	}

protected: 
};

Img *Img::instance = NULL;
Graphics *Img::canvas = NULL;

class Coor
{
protected:
	float x; 
	float y; 
public: 
	Coor() { 
		x = 0.0;  
		y = 0.0; 
	}
	Coor(float x, float y) { 
		this->x = x; 
		this->y = y; 
	}
	float getX() { 
		return x; 
	}
	float getY() { 
		return y; 
	}
};

class ColorRGB
{
private: 
	uint8_t red; 
	uint8_t blue; 
	uint8_t green; 
public: 
	ColorRGB() {
		red = green = blue = 0;
	}
	ColorRGB(uint8_t red, uint8_t green, uint8_t blue) {
		this->red = red; 
		this->blue = blue; 
		this->green = green;
	}
	ColorRGB(uint8_t* arr) {
		this->red = *arr; 
		this->green = *(arr + 1); 
		this->red = *(arr + 2); 
	}
	uint8_t* getColor() {
		uint8_t* arr = new uint8_t[3]; 
		*arr = this->red; 
		*(arr + 1) = this->green; 
		*(arr + 2) = this->blue;
		return arr; 
	}
	char* getStringcolor() {
		char* x = new char[14];
		x[13] = '\0';
		x[0] = '('; x[12] = ')'; x[4] = x[8] = ',';
		x[1] = red / 100 + '0'; 
		x[2] = (red / 10) % 10 + '0'; 
		x[3] = red % 10 + '0'; 

		x[5] = green / 100 + '0';
		x[6] = (green / 10) % 10 + '0';
		x[7] = green % 10 + '0';

		x[9] = blue / 100 + '0';
		x[10] = (blue / 10) % 10 + '0';
		x[11] = blue % 10 + '0';

		return x; 
	}
	void setRed(uint8_t x) {
		red = x; 
	}
	void setGreen(uint8_t x) {
		green = x;
	}
	void setBlue(uint8_t x) {
		blue = x;
	}
};

ColorRGB convertStringToColorRGB(string s) {
	ColorRGB Value; 
	int len = s.length(); 
	int i = 1; 
	int x = 0;  
	while (s[i] != ',')
	{
		if (s[i] <= '9' && s[i] >= '0')
			x = x * 10 + s[i] - '0';
		i++;
	}
	Value.setRed(x);
	x = 0; 
	i++; 
	while (s[i] != ',')
	{
		if (s[i] <= '9' && s[i] >= '0') 
			x = x * 10 + s[i] - '0'; 
		i++;
	}
	Value.setGreen(x);
	x = 0; 
	while (s[i] != ')')
	{
		if (s[i] <= '9' && s[i] >= '0')
			x = x * 10 + s[i] - '0';
		i++; 
	}
	Value.setBlue(x); 
	return Value; 

}

char* floatToString(float value) {
	std::ostringstream os;
	os << value;
	const char* buffer = os.str().c_str();
	char x[20];
	strcpy_s(x, buffer);
	return x;
}

class Attribute
{
protected:
	char* Name;
	float Value;
	ColorRGB ColorValue;
	string StringValue;

public:
	friend class Properties;
	friend class Transform; 

	virtual void trans() = 0; 

	Attribute() {
		Value = 1;
		ColorValue = ColorRGB(0, 0, 0);
	}
	char* getName() {
		return Name;  
	}
	float getValue() {
		return Value;
	}
	ColorRGB getColorValue() {
		return ColorValue;
	}
	string getStringValue() {
		string x = this->StringValue; 
		return StringValue;
	}
	virtual vector <Transform*> getVectorValue() = 0; 
	virtual void setAttribute(rapidxml::xml_attribute<> *att) = 0;
	void setAttribute(char* NameAttribute, float Value, ColorRGB ColorValue) {
		this->Name = NameAttribute;
		this->Value = Value;
		this->ColorValue = ColorValue;
		if (NameAttribute == "stroke" || NameAttribute == "fill") {
			this->StringValue = ColorValue.getStringcolor();
		}
		else {
			this->StringValue = floatToString(this->Value);
		}
	}
};

class Fill : public Attribute
{
	void trans() {}
	void setAttribute(rapidxml::xml_attribute<> *att) {
		Name = "fill";
		ColorValue = convertStringToColorRGB(att->value() + 3);
		StringValue = att->value() + 3;
	}
	vector <Transform* > getVectorValue() {
		vector <Transform* > x; 
		return x;
	}
};

class Stroke : public Attribute
{
	void trans() {}
	void setAttribute(rapidxml::xml_attribute<> *att) {
		Name = "stroke";
		ColorValue = convertStringToColorRGB(att->value() + 3);
		StringValue = att->value() + 3;
	}
	vector <Transform* > getVectorValue() {
		vector <Transform* > x;
		return x;
	}
};

class Fill_opacity : public Attribute
{
	void trans() {}
	void setAttribute(rapidxml::xml_attribute<> *att) {
		Name = "fill-opacity";
		Value = atof(att->value());
		StringValue = att->value();
	}
	vector <Transform* > getVectorValue() {
		vector <Transform* > x;
		return x;
	}
};

class Stroke_width : public Attribute
{
	void trans() {}
	void setAttribute(rapidxml::xml_attribute<> *att){
		Name = "stroke-width";
		Value = atof(att->value());
		StringValue = att->value();
	}
	vector <Transform* > getVectorValue() {
		vector <Transform* > x;
		return x;
	}
};

class Stroke_opacity : public Attribute
{
	void trans() {}
	void setAttribute(rapidxml::xml_attribute<> *att){
		Name = "stroke-opacity";
		Value = atof(att->value());
		StringValue = att->value();
	}
	vector <Transform* > getVectorValue() {
		vector <Transform* > x;
		return x;
	}
};

class Transform 
{

public  :
	virtual void actionTrans(Graphics *canvas) = 0;
	friend class Properties;
};

class Rotate : public Transform{
private:
	float rotate;

	void actionTrans(Graphics *canvas){
		canvas->RotateTransform(rotate);
	}
public:
	Rotate() {}
	Rotate(float rotate) : rotate(rotate) {}
	static Rotate *readTrans(string& input){
		int pos = 0;
		Rotate *res = new Rotate;
		input.erase(pos, 6);
		char* tmp = new char[10];
		int len = 0;
		pos = 0;
		while (1){
			if (input[pos] == ')' || input[pos] == ',')
				break;
			if (input[pos] == '-' || (input[pos] >= '0' && input[pos] <= '9')) {
				tmp[len] = input[pos];
				len++;
			}
			++pos;
		}
		tmp[len] = '\0';
		res->rotate = atof(tmp);
		input.erase(0, pos + 1); 
		return res;
	}
	void writeTransform(ostream& out) {

	}
};

class Translate : public Coor, public Transform{
private:
	void actionTrans(Graphics *canvas){
		canvas->TranslateTransform(getX(), getY());
	}
public:
	Translate() {}
	Translate(float x, float y) : Coor(x, y) {}
	static Translate *readTrans(string& input){
		int pos = 0;
		Translate *res = new Translate;
		input.erase(pos, 5);
		char* tmp = new char[10];
		int len = 0;
		pos = 0;
		while (1){
			if (input[pos] == ')' || input[pos] == ',')
				break;
			if (input[pos] ==  '-'  || (input[pos] >= '0' && input[pos] <= '9')) {
				tmp[len] = input[pos];
				len++;
			}
			++pos;
		}
		tmp[len] = '\0';
		res->x = atof(tmp);
		pos++;
		len = 0;
		while (1){
			if (input[pos] == ')' || input[pos] == ',')
				break;
			if (input[pos] != ' ') {
				tmp[len] = input[pos];
				len++;
			}
			++pos;
		}
		tmp[len] = '\0';
		while (input[pos] == ')') break; 
		input.erase(0, pos + 1); 
		res->y = atof(tmp);
		return res;
	}
	void writeTransform(ostream& out) {

	}

};

class Scale : public Transform{
private:
	double scale; 
	void actionTrans(Graphics *canvas){
		canvas->ScaleTransform(scale, scale);
	}
public:
	Scale() {}
	Scale(float x) : scale(x) {}
	static Scale *readTrans(string& input){
		int pos = 0;
		Scale *res = new Scale;
		input.erase(pos, 9);
		char* tmp = new char[10];
		int len = 0;
		pos = 0;
		while (1){
			if (input[pos] == ')' || input[pos] == ',')
				break;
			if (input[pos] == '-' || (input[pos] >= '0' && input[pos] <= '9')) {
				tmp[len] = input[pos];
				len++;
			}
			++pos;
		}
		tmp[len] = '\0';
		res->scale = atof(tmp);
		input.erase(0, pos + 1);
		return res;
	}
	void writeTransform(ostream& out) {

	}

};


class TranSet : public Attribute
{
private:
	vector <Transform* > Trans;
public:
	void trans() {
		for (vector <Transform*> ::iterator it = Trans.begin(); it != Trans.end(); it++)
			(*it)->actionTrans(Img::canvas); 
	}
	void setAttribute(rapidxml::xml_attribute<> *att){
		Name = "transform"; 
		string input = att->value();
		this->StringValue = input; 
		Transform *addTransform;
		int posRo, posTra, posSca;
		while (1){
			posRo = input.find("rotate");
			posTra = input.find("translate");
			posSca = input.find("scale"); 
			if (posRo == -1 && posTra == -1 && posSca == -1)
				break;
			if (posRo == -1) posRo = 1000;
			if (posSca == -1) posSca = 1000;
			if (posTra == -1) posTra = 1000;
			if (posRo < posTra && posRo < posSca) {
				addTransform = new Rotate;
				addTransform = Rotate::readTrans(input);
				Trans.push_back(addTransform);
			}
			else if (posTra < posRo && posTra < posSca) {
				addTransform = new Translate;
				addTransform = Translate::readTrans(input);
				Trans.push_back(addTransform);
			}
			else {
				addTransform = new Scale; 
				addTransform = Translate::readTrans(input); 
				Trans.push_back(addTransform);
			}
		}

		//delete addTransform;
	}

	vector <Transform* > getVectorValue() {
		return this->Trans; 
	}


};

Attribute* createAttribute(rapidxml::xml_attribute<>* att) {
	Attribute* x = NULL;
	if (strcmp(att->name(), "stroke-width") == 0)
		x = new Stroke_width();
	if (strcmp(att->name(), "stroke-opacity") == 0)
		x = new Stroke_opacity();
	if (strcmp(att->name(), "stroke") == 0)
		x = new Stroke();
	if (strcmp(att->name(), "fill") == 0)
		x = new Fill();
	if (strcmp(att->name(), "fill-opacity") == 0)
		x = new Fill_opacity();
	if (strcmp(att->name(), "transform") == 0)
		x = new TranSet(); 
	return x;
}


char NameSet[6][20] = { "stroke", "stroke-width", "fill", "stroke-opacity", "fill_opacity" , "transform"};



class Properties
{
protected: 
	friend Img; 

	set <Attribute*> Attributes;

public : 
	Properties() {
	}
	Properties(ColorRGB stroke, float stroke_width, float stroke_opacity, vector <Transform *> transforms)  {
		Attribute* att; 
		att = new Stroke; 
		att->setAttribute("stroke", 0, stroke); 
		Attributes.insert(att); 

		att = new Stroke_width;
		att->setAttribute("stroke-width", stroke_width, ColorRGB(0, 0, 0));
		Attributes.insert(att);
		
		att = new Stroke_opacity;
		att->setAttribute("stroke-opacity", stroke_opacity, ColorRGB(0, 0, 0));
		Attributes.insert(att);

	}
	Properties(ColorRGB fill, ColorRGB stroke, float stroke_width, float fill_opacity, float stroke_opacity, vector <Transform *> transforms) {
		Attribute* att;

		att = new Fill;
		att->setAttribute("fill", 0, fill);
		Attributes.insert(att);

		att = new Fill_opacity;
		att->setAttribute("fill-opacity", fill_opacity, ColorRGB(0, 0, 0));
		Attributes.insert(att);

		att = new Stroke;
		att->setAttribute("stroke", 0, stroke);
		Attributes.insert(att);

		att = new Stroke_width;
		att->setAttribute("stroke-width", stroke_width, ColorRGB(0, 0, 0));
		Attributes.insert(att);

		att = new Stroke_opacity;
		att->setAttribute("stroke-opacity", stroke_opacity, ColorRGB(0, 0, 0));
		Attributes.insert(att);
	}

	set <Attribute*> getAttribute(){
		return this->Attributes; 
	}

	string getValue(char* type){
		for (set <Attribute*>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		{
			char* name = (*it)->Name;
			if (strcmp(type, (*it)->Name) == 0)
			{
				return (*it)->getStringValue();
			}
		}
		if (strcmp(type, "transform") == 0) return "";
		else 
		if (strcmp(type, "stroke") == 0 || strcmp(type, "fill") == 0)
			return "(0,0,0)";
		else
			return "1";
	}

	void trans(){

		for (set <Attribute*>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		if (strcmp("transform", (*it)->Name) == 0)
			(*it)->trans(); 
	}
	
	static Graphics *getCanvas(){
		return Img::canvas;
	}

	void readAttribute(rapidxml::xml_node<> *node) {
		for (rapidxml::xml_attribute<> *att = node->first_attribute(); att; att = att->next_attribute()) {
			Attribute* newAtt = createAttribute(att); 
			if (newAtt != NULL){
				newAtt->setAttribute(att);
				Attributes.insert(newAtt);
			}
		}
	}


	ColorRGB getStroke() {
		for (set <Attribute*>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		if (strcmp("stroke", (*it)->Name) == 0)
			return (*it)->getColorValue();
		return ColorRGB(0, 0, 0);
	}
	ColorRGB getFill() {
		for (set <Attribute*>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		if (strcmp("fill", (*it)->Name) == 0)
			return (*it)->getColorValue();
		return ColorRGB(0, 0, 0); 
	}
	float getStroke_width() {
		for (set <Attribute*>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		if (strcmp("stroke-width", (*it)->Name) == 0)
			return (*it)->getValue();
		return 1; 
	}
	float getStroke_opacity() {
		for (set <Attribute*>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		if (strcmp("stroke-opacity", (*it)->Name) == 0)
			return (*it)->getValue();
		return 1;
	}
	float getFill_opacity() {
		for (set <Attribute*>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		if (strcmp("fill-opacity", (*it)->Name) == 0)
			return (*it)->getValue();
		return 1; 
	}
	vector <Transform* > getTransform() {
		for (set <Attribute*>::iterator it = Attributes.begin(); it != Attributes.end(); ++it)
		if (strcmp("transform", (*it)->Name) == 0)
			return (*it)->getVectorValue();
		vector <Transform*> none; 
		return none;  
	}
};

class _Line : public Properties, public Shape{
private:
	Coor A, B;
	friend class ColorRGB; 


public:
	friend Properties; 
	_Line() {}
	_Line(Coor A, Coor B, ColorRGB stroke, float stroke_width, float stroke_opacity, vector <Transform *> transforms = {}) :
	A(A), B(B),
	Properties(stroke, stroke_width, stroke_opacity, transforms) {}
	void readSVG(rapidxml::xml_node<> *node)  {
		float x1, y1, x2, y2;
		x1 = y1 = x2 = y2 = 0.0; 
		for (rapidxml::xml_attribute<> *att = node->first_attribute(); att; att = att->next_attribute())
		if (string(att->name()) == "x1")
			x1 = atof(att->value());
		else
		if (string(att->name()) == "y1")
			y1 = atof(att->value());
		else
		if (string(att->name()) == "x2")
			x2 = atof(att->value());
		else
		if (string(att->name()) == "y2")
			y2 = atof(att->value());

		A = Coor(x1, y1);
		B = Coor(x2, y2);
		this->readAttribute(node); 
	}

	void draw(){
		uint8_t * colorStroke = (this->getStroke()).getColor();
		Graphics *canvas = getCanvas();
		float x; 
		x = this->getStroke_opacity(); 
		x = this->getStroke_width(); 
		trans();
		Pen pen(Color(this->getStroke_opacity() * 255, colorStroke[0], colorStroke[1], colorStroke[2]), this->getStroke_width());
		canvas->DrawLine(&pen, A.getX(), A.getY(), B.getX(), B.getY());
		delete[] colorStroke;

		canvas->ResetTransform();
	}

	void writeSVG(ostream& out){
		out << "<line x1=\"" << A.getX() << "\" y1=\"" << A.getY()
			<< "\" x2=\"" << B.getX() << "\" y2=\"" << B.getY()
			<< "\" stroke=\"rgb" << this->getValue("stroke") << "\" stroke-width=\"" <<this->getValue("stroke-width")
			<< "\" stroke-opacity=\"" << this->getValue("stroke-opacity")
			<< "\" transform=\"" << this->getValue("transform") << "\""
			<< "/>\n";
	}

	set <Attribute*> getProperties() {
		//return Properties(this->getFill(), this->getStroke(), this->getStroke_width(), this->getFill_opacity(), this->getStroke_width(), this->getTransform());
		return this->Attributes;
	}
	void addProperties(set <Attribute*> q) {
		for (set <Attribute* > ::iterator it = q.begin(); it != q.end(); it++) {
			(this->Attributes).insert(*it); 
		}
	}
};

class _Rectangle : public Properties, public Shape{
private : 
	Coor A; 
	float width, height; 

	friend class ColorRGB;  

public : 
	_Rectangle() {}
	_Rectangle(Coor A, float width, float height, ColorRGB fill, ColorRGB stroke, float stroke_width,
		float fill_opacity, float stroke_opacity, vector <Transform *> transforms = {}) :
		A(A), width(width), height(height),
		Properties(fill, stroke, stroke_width, fill_opacity, stroke_opacity, transforms) {}

	void readSVG(rapidxml::xml_node<> *node)  {
		float x, y;
		x = y = 0.0;
		for (rapidxml::xml_attribute<> *att = node->first_attribute(); att; att = att->next_attribute())
		if (string(att->name()) == "x")
			x = atof(att->value());
		else
		if (string(att->name()) == "y")
			y = atof(att->value());
		else
		if (string(att->name()) == "width")
			width = atof(att->value());
		else
		if (string(att->name()) == "height")
			height = atof(att->value());

		A = Coor(x, y);
		this->readAttribute(node);
	}


	void writeSVG(ostream& out){
		out << "<rect x=\"" << A.getX() << "\" y=\"" << A.getY()
			<< "\" width =\"" << this->width << "\" height=\"" << this->height
			<< "\" stroke=\"rgb" << this->getValue("stroke") << "\" stroke-width=\"" << this->getValue("stroke-width")
			<< "\" fill=\"rgb" << this->getValue("fill") << "\" fill-opacity=\"" << this->getValue("fill-opacity")
			<< "\" stroke-opacity=\"" << this->getValue("stroke-opacity") 
			<< "\" transform=\"" << this->getValue("transform") <<"\""
			<< "/>\n";
	}


	void draw(){
		uint8_t *colorStroke = (this->getStroke()).getColor();
		uint8_t *colorFill = (this->getFill()).getColor();
		Graphics *canvas = getCanvas();

		trans();
		SolidBrush brust(Color(this->getFill_opacity() * 255, colorFill[0], colorFill[1], colorFill[2]));
		canvas->FillRectangle(&brust, A.getX(), A.getY(), width, height);

		Pen pen(Color(this->getStroke_opacity() * 255, colorStroke[0], colorStroke[1], colorStroke[2]), this->getStroke_width());
		canvas->DrawRectangle(&pen, A.getX(), A.getY(), this->width, this->height);

		delete[] colorStroke;
		delete[] colorFill;
		canvas->ResetTransform();
	}

	set <Attribute*> getProperties() {
		//return Properties(this->getFill(), this->getStroke(), this->getStroke_width(), this->getFill_opacity(), this->getStroke_width(), this->getTransform());
		return this->Attributes;
	}
	void addProperties(set <Attribute*> q) {
		for (set <Attribute* > ::iterator it = q.begin(); it != q.end(); it++) {
			(this->Attributes).insert(*it);
		}
	}
};

class _Polyline : public Properties, public Shape{
protected:
	vector <Coor> setCoor;

public:
	_Polyline() {}
	void draw(){
		uint8_t *colorStroke = (this->getStroke()).getColor();
		uint8_t *colorFill = (this->getFill()).getColor();
		Graphics *canvas = getCanvas();

		Point *setPoint = new Point[setCoor.size()];
		for (int i = 0; i < setCoor.size(); ++i)
			setPoint[i] = Point(setCoor[i].getX(), setCoor[i].getY());

		GraphicsPath path;
		path.AddLines(setPoint, setCoor.size());

		trans();
		SolidBrush brust(Color(this->getFill_opacity() * 255, colorFill[0], colorFill[1], colorFill[2]));
		canvas->FillPath(&brust, &path);

		Pen pen(Color(this->getStroke_opacity() * 255, colorStroke[0], colorStroke[1], colorStroke[2]), this->getStroke_width());
		canvas->DrawLines(&pen, setPoint, setCoor.size());

		delete[] setPoint;
		delete[] colorStroke;
		delete[] colorFill;
		canvas->ResetTransform();
	}
	_Polyline(vector <Coor> set,
		ColorRGB fill, ColorRGB stroke, float stroke_width, float fill_opacity,
		float stroke_opacity, vector <Transform *> transforms = {}) :
		setCoor(set),
		Properties(fill, stroke, stroke_width, fill_opacity, stroke_opacity, transforms) {}


	void writeSVGPolyline(ostream& out) {

		for (vector <Coor>::iterator it = setCoor.begin(); it != setCoor.end(); ++it)
			out << (*it).getX() << ", " << (*it).getY() << ' ';

		out << "\" stroke=\"rgb" << this->getValue("stroke") << "\" stroke-width=\"" << this->getValue("stroke-width")
			<< "\" fill=\"rgb" << this->getValue("fill") << "\" stroke-opacity=\"" << this->getValue("stroke_opacity")
			<< "\" fill-opacity=\"" << this->getValue("fill_opacity")
			<< "\" transform=\"" << this->getValue("transform") << "\""
			<< "/>\n";
	}
	void writeSVG(ostream& out){
		out << "<polyline points=\"";
		writeSVGPolyline(out); 
	}


	void readSVG(rapidxml::xml_node<> *node){
		float x, y;

		for (rapidxml::xml_attribute<> *att = node->first_attribute(); att; att = att->next_attribute())
		if (strcmp(att->name(), "points") == 0) {
			char* input = att->value();
			float x, y;
			int len = strlen(input);
			int pos = 0;
			while (pos < len) {
				while (input[pos] == ' ' || input[pos] == '\n') pos++;
				int m = 0;
				char *p;
				p = new char[20];
				while (input[pos] != ',' && input[pos] != ' ' && input[pos] != '\n'){
					p[m] = input[pos];
					m++;  pos++;
				}
				p[m] = '\0';
				x = atof(p);
				m = 0;
				pos++;
				while (input[pos] == ' ' || input[pos] == '\n') pos++;
				while (input[pos] != ' ' && pos < len  && input[pos] != '\n') {
					p[m] = input[pos];
					m++;  pos++;
				}
				p[m] = '\0';
				y = atof(p);
				while (input[pos] == ' ' || input[pos] == '\n') pos++;
				setCoor.push_back(Coor(x, y));
			}
		}
		this->readAttribute(node);
	}

	set <Attribute*> getProperties() {
		//return Properties(this->getFill(), this->getStroke(), this->getStroke_width(), this->getFill_opacity(), this->getStroke_width(), this->getTransform());
		return this->Attributes;
	}
	void addProperties(set <Attribute*> q) {
		for (set <Attribute* > ::iterator it = q.begin(); it != q.end(); it++) {
			(this->Attributes).insert(*it);
		}
	}
};

class _Polygon : public _Polyline{
private:
public:
	_Polygon() {}
	_Polygon(vector <Coor> set,
		ColorRGB fill, ColorRGB stroke, float stroke_width, float fill_opacity,
		float stroke_opacity, vector <Transform *> transforms = {}) :
		_Polyline(set, fill, stroke, stroke_width, fill_opacity, stroke_opacity, transforms) {}


	void draw(){
		uint8_t *colorStroke = (this->getStroke()).getColor();
		uint8_t *colorFill = (this->getFill()).getColor();
		Graphics *canvas = getCanvas();

		Point *setPoint = new Point[setCoor.size()];
		for (int i = 0; i < setCoor.size(); ++i)
			setPoint[i] = Point(setCoor[i].getX(), setCoor[i].getY());

		trans();
		SolidBrush brust(Color(this->getStroke_opacity() * 255, colorFill[0], colorFill[1], colorFill[2]));
		canvas->FillPolygon(&brust, setPoint, setCoor.size());

		Pen pen(Color(this->getStroke_opacity() * 255, colorStroke[0], colorStroke[1], colorStroke[2]), this->getStroke_width());
		canvas->DrawPolygon(&pen, setPoint, setCoor.size());

		delete[] setPoint;
		delete[] colorStroke;
		delete[] colorFill;
		canvas->ResetTransform();
	}

	void writeSVG(ostream& out){
		out << "<polygon points=\"";
		writeSVGPolyline(out); 
	}

	set <Attribute*> getProperties() {
		//return Properties(this->getFill(), this->getStroke(), this->getStroke_width(), this->getFill_opacity(), this->getStroke_width(), this->getTransform());
		return this->Attributes;
	}
	void addProperties(set <Attribute*> q) {
		for (set <Attribute* > ::iterator it = q.begin(); it != q.end(); it++) {
			(this->Attributes).insert(*it);
		}
	}
};

class _Ellipse : public Properties, public Shape{
protected:
	Coor Center;
	float rx, ry;

public:
	_Ellipse() {}
	_Ellipse(Coor center, float rx, float ry, ColorRGB fill, ColorRGB stroke, float stroke_width,
		float fill_opacity, float stroke_opacity, vector <Transform *> transforms = {}) :
		Center(center), rx(rx), ry(ry),
		Properties(fill, stroke, stroke_width, fill_opacity, stroke_opacity, transforms) {}

	void draw(){
		uint8_t *colorStroke = (this->getStroke()).getColor();
		uint8_t *colorFill = (this->getFill()).getColor();
		Graphics *canvas = getCanvas();

		trans();
		SolidBrush brust(Color(this->getFill_opacity() * 255, colorFill[0], colorFill[1], colorFill[2]));
		canvas->FillEllipse(&brust, Center.getX() - rx, Center.getY() - ry, rx * 2, ry * 2);

		Pen pen(Color(this->getStroke_opacity() * 255, colorStroke[0], colorStroke[1], colorStroke[2]), this->getStroke_width());
		canvas->DrawEllipse(&pen, Center.getX() - rx, Center.getY() - ry, rx * 2, ry * 2); 

		delete[] colorStroke;
		delete[] colorFill;
		canvas->ResetTransform();
	}
	void readSVG(rapidxml::xml_node<> *node){
		float x, y;
		x = y = 0;

		for (rapidxml::xml_attribute<> *att = node->first_attribute(); att; att = att->next_attribute())
		if (string(att->name()) == "cx")
			x = atof(att->value());
		else
		if (string(att->name()) == "cy")
			y = atof(att->value());
		else
		if (string(att->name()) == "rx")
			rx = atof(att->value());
		else
		if (string(att->name()) == "ry")
			ry = atof(att->value());

		Center = Coor(x, y);
		this->readAttribute(node);
	}

	void writeSVG(ostream& out){
		out << "<ellipse cx=\"" << Center.getX() << "\" cy=\"" << Center.getY()
			<< "\" rx=\"" << rx << "\" ry=\"" << ry;

		out << "\" stroke=\"rgb" << this->getValue("stroke") << "\" stroke-width=\"" << this->getValue("stroke-width")
			<< "\" fill=\"rgb" << this->getValue("fill") << "\" stroke-opacity=\"" << this->getValue("stroke-opacity")
			<< "\" fill-opacity=\"" << this->getValue("fill-opacity") 
			<< "\" transform=\"" << this->getValue("transform") << "\""
			<< "/>\n";
	}

	set <Attribute*> getProperties() {
		//return Properties(this->getFill(), this->getStroke(), this->getStroke_width(), this->getFill_opacity(), this->getStroke_width(), this->getTransform());
		return this->Attributes;
	}
	void addProperties(set <Attribute*> q) {
		for (set <Attribute* > ::iterator it = q.begin(); it != q.end(); it++) {
			(this->Attributes).insert(*it);
		}
	}


};

class _Circle : public _Ellipse{
private:
public:
	_Circle() {}
	_Circle(Coor tam, float r,
		ColorRGB fill, ColorRGB stroke, float stroke_width, float fill_opacity, float stroke_opacity) :
		
		_Ellipse(tam, r, r, fill, stroke, stroke_width, fill_opacity, stroke_opacity) {}


	void writeSVG(ostream& out){
		out << "<circle cx=\"" << Center.getX() << "\" cy=\"" << Center.getY()
			<< "\" r=\"" << rx
			<< "\" stroke=\"rgb" << this->getValue("stroke") << "\" stroke-width=\"" << this->getValue("stroke-width")
			<< "\" fill=\"rgb" << this->getValue("fill") << "\" stroke-opacity=\"" << this->getValue("stroke-opacity")
			<< "\" fill-opacity=\"" << this->getValue("fill-opacity")
			<< "\" transform=\"" << this->getValue("transform") << "\""
			<< "/>\n";
	}

	void readSVG(rapidxml::xml_node<> *node){
		float x, y;
		x = y = 0;

		for (rapidxml::xml_attribute<> *att = node->first_attribute(); att; att = att->next_attribute())
		if (string(att->name()) == "cx")
			x = atof(att->value());
		else
		if (string(att->name()) == "cy")
			y = atof(att->value());
		else
		if (string(att->name()) == "r")
			rx = ry = atof(att->value());

		Center = Coor(x, y);
		this->readAttribute(node);
	}


	set <Attribute*> getProperties() {
		//return Properties(this->getFill(), this->getStroke(), this->getStroke_width(), this->getFill_opacity(), this->getStroke_width(), this->getTransform());
		return this->Attributes;
	}
	void addProperties(set <Attribute*> q) {
		for (set <Attribute* > ::iterator it = q.begin(); it != q.end(); it++) {
			(this->Attributes).insert(*it);
		}
	}


};

void read(rapidxml::xml_node<> *root, vector <Shape*> &shape, set <Attribute*> &Att) {
	for (rapidxml::xml_node<> *node_temp = root->first_node(); node_temp; node_temp = node_temp->next_sibling())
	{
		Shape* newShape = NULL;
		if (strcmp(node_temp->name(), "line") == 0) {
			newShape = new _Line;
			newShape->readSVG(node_temp);
		}
		if (strcmp(node_temp->name(), "rect") == 0) {
			newShape = new _Rectangle;
			newShape->readSVG(node_temp);
		}
		if (strcmp(node_temp->name(), "polyline") == 0) {
			newShape = new _Polyline;
			newShape->readSVG(node_temp);
		}
		if (strcmp(node_temp->name(), "polygon") == 0) {
			newShape = new _Polygon;
			newShape->readSVG(node_temp);
		}
		if (strcmp(node_temp->name(), "ellipse") == 0) {
			newShape = new _Ellipse;
			newShape->readSVG(node_temp);
		}
		if (strcmp(node_temp->name(), "circle") == 0) {
			newShape = new _Circle;
			newShape->readSVG(node_temp);
		}

		if (newShape != NULL)
		{
			newShape->addProperties(Att); 
			shape.push_back(newShape);
		}
		if (strcmp(node_temp->name(), "g") == 0) {
			newShape = new _Line;
			newShape->readSVG(node_temp);
			set <Attribute*> q = newShape->getProperties(); 
			for (set <Attribute* > ::iterator it = q.begin(); it != q.end(); it++) {
				Att.insert(*it);
			}
			read(node_temp, shape, Att); 
		}
	}

}

class ImportSVG
{
private:
	ifstream input_file;
	rapidxml::xml_document<> doc;
public:
	vector <Shape*> shape;
	void import(char* file_name)
	{
		if (input_file.is_open())
			input_file.close();
		input_file.open(file_name);
		vector <char> buf((istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>());
		buf.push_back('\0');
		doc.parse<0>(&buf[0]);

		rapidxml::xml_node<>* root = doc.first_node();
		if (strcmp(root->name(), "svg") != 0)
			return;
		set <Attribute*> Att; 
		read(root, this->shape, Att); 
		
		input_file.close();
	}
	void export(char* filename) 
	{
		ofstream out(filename);
		out << "<svg xmlns=\"http:\/\/www.w3.org\/2000\/svg\">\n";
		for (vector <Shape*>::iterator it = shape.end(); it != shape.begin(); )
		{
			it--; 
			(*it)->writeSVG(out);
		}
		out << "<\/svg>\n";
		out.close();

	}
	
	void clear(){
		for (int i = 0; i < shape.size(); i++)
			delete shape[i];
		shape.clear();
	}

};

void Img::draw(ImportSVG *svg){
	for (vector <Shape*>::iterator it = svg->shape.begin(); it != svg->shape.end(); ++it)
		(*it)->draw(); 
}