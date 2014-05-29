#include <iostream>
#include <list>
#include <cstdlib>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <sstream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;

const int Border = 10;
const int BufferSize = 10;
const int FPS = 60;
int won = 1;
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;
int o_SCREEN_WIDTH = 800;
int o_SCREEN_HEIGHT = 600;
int old_width = 800;
int old_height = 600;
int UP_BOUND;
int DOWN_BOUND;
int LEFT_BOUND;
int RIGHT_BOUND;
int keyPress=0;
int blank = 0;   //indicate the size of the window is too small.

/* Some constants which are related to the speeds */
long AIRRES = 1;
long GRA = 1;
int count;
int count2 = 1;
int Fail;
int fuelCount = 10;
int xSpeed = 0;
int ySpeed = 0;
int m_diff;
int is_first_time = 1;

/* Define the direction of the thruster */
const int UP = 1;
const int DOWN = 2;
const int LEFT = 3;
const int RIGHT = 4;

/*
 * Information to draw on the window.
 */
struct XInfo {
	Display *display;
	int screen;
	Window window;
	GC gc[10];
	int width;     // size of the window
	int height;
	int stage;     //indicate the stage of the game: start screen, in-game
};

/*
 * Function to put out a message on error exits.
 */
void error(string str) {
	cerr << str << endl;
	exit(0);
}

/*
 * An abstract class representing displayable things.
 */
class Displayable {
public:
	virtual void paint(XInfo &xinfo) = 0;
};

int triangleabs_x;
int triangleabs_y;
class triangle: public Displayable {
public:
	virtual void paint(XInfo &xInfo) {
		if (show == 1) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], trianglex,
					triangley, trianglex - 15, triangley + 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], trianglex,
					triangley, trianglex + 15, triangley + 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], trianglex - 15,
					triangley + 15, trianglex + 15, triangley + 15);
		} else {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[3], trianglex,
					triangley, trianglex - 15, triangley + 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[3], trianglex,
					triangley, trianglex + 15, triangley + 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[3], trianglex - 15,
					triangley + 15, trianglex + 15, triangley + 15);
		}
		if (count % 30 == 0) {
			show = show * -1;
		}
	}
	//constructor
	triangle() {
		show = 1;
		level = 1;
		trianglex = SCREEN_WIDTH / 2 + 110;
		triangley = SCREEN_HEIGHT / 2 - 45;
		triangleabs_x = trianglex;
		triangleabs_y = triangley;
	}
	int getLv() {
		return level;
	}
	int setLv(int lv) {
		level = lv;
	}
	void update() {
		trianglex = triangleabs_x + (SCREEN_WIDTH - old_width) / 2;
		triangley = triangleabs_y + (SCREEN_HEIGHT - old_height) / 2;

	}
	void moveLeft() {
		if (level != 1) {
			trianglex = trianglex - 100;
			level = level - 1;
		}
	}
	void moveRight() {
		if (level != 3) {
			trianglex = trianglex + 100;
			level = level + 1;
		}
	}
private:
	int level;
	int trianglex;
	int triangley;
	int show;
};

int starabs_x;
int starabs_y;
class star: public Displayable {
public:
	virtual void paint(XInfo &xInfo) {
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], starx, stary,
				starx - 15, stary + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], starx, stary,
				starx + 15, stary + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], starx - 15,
				stary + 15, starx + 15, stary + 15);
	}
	//constructor
	star() {
		starx = SCREEN_WIDTH / 2;
		stary = SCREEN_HEIGHT / 2 + 350;
	}
	int getLv() {
		return level;
	}
	int setLv(int lv) {
		level = lv;
	}
	void update() {
		starx = starabs_x + (SCREEN_WIDTH - old_width) / 2;
		stary = starabs_y + (SCREEN_HEIGHT - old_height) / 2;

	}
private:
	int level;
	int starx;
	int stary;
};

class Boundary: public Displayable {
public:
	virtual void paint(XInfo &xInfo) {
		if (type == 1) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], LEFT_BOUND,
					UP_BOUND, RIGHT_BOUND, UP_BOUND);
		} else if (type == 2) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], LEFT_BOUND,
					DOWN_BOUND, RIGHT_BOUND, DOWN_BOUND);
		} else if (type == 3) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], LEFT_BOUND,
					UP_BOUND, LEFT_BOUND, DOWN_BOUND);
		} else if (type == 4) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], RIGHT_BOUND,
					UP_BOUND, RIGHT_BOUND, DOWN_BOUND);
		}
	}
	// Constructor
	Boundary() {
	}
	Boundary(int t) {
		type = t;
	}
private:
	int type;
};

int sabs_x;
int sabs_y;

class GameText: public Displayable {
public:
	virtual void paint(XInfo &xInfo) {
		if (type == 1) {
			x = (SCREEN_WIDTH - width) / 2;
			y = (SCREEN_HEIGHT - 600) / 2 + 350;
		} else if (type == 2) {
			x = (SCREEN_WIDTH - width) / 2;
			y = (SCREEN_HEIGHT - 600) / 2 + 150;
		} else if (type == 3) {
			x = (SCREEN_WIDTH - width) / 2;
			y = (SCREEN_HEIGHT - 600) / 2 + 150;
		} else if (type == 4) {
			x = (SCREEN_WIDTH - width) / 2;
			y = (SCREEN_HEIGHT - 600) / 2 + 150;
		} else if (type == 5) {
			x = (SCREEN_WIDTH - width) / 2;
			y = (SCREEN_HEIGHT - 600) / 2 + 150;
		} else if (type == 6) {
			x = (SCREEN_WIDTH - width) / 2;
			y = (SCREEN_HEIGHT - 600) / 2 + 150;
		} else if (type == 7) {
			x = (SCREEN_WIDTH - width) / 2;
			y = (SCREEN_HEIGHT - 600) / 2 + 50;

			setText();
		} else if (type == 8) {
			x = (SCREEN_WIDTH - width) / 2;
			y = (SCREEN_HEIGHT - 600) / 2 + 250;
		} else if (type == 9) {
			x = (SCREEN_WIDTH - width) / 2 + 250;
			y = (SCREEN_HEIGHT - 600) / 2 + 30;
		} else if (type == 10) {
			x = (SCREEN_WIDTH - width) / 2 + 250;
			y = (SCREEN_HEIGHT - 600) / 2 + 60;
		}

		if ((type == 7 && fuelCount <= 5) || (type == 9 && abs(xSpeed) > 3)
				|| (type == 10 && abs(ySpeed) > 3)) {
			XDrawImageString(xInfo.display, xInfo.window, xInfo.gc[2], x, y,
					text.c_str(), text.length());
		} else {
			XDrawImageString(xInfo.display, xInfo.window, xInfo.gc[1], x, y,
					text.c_str(), text.length());
		}
	}

	void setText() {
		ostringstream convert;
		if (type == 7) {
			convert << fuelCount;
			text = "FUEL LEFT: " + convert.str();
		} else if (type == 9) {
			convert << xSpeed*10;
			text = "x-speed: " + convert.str()+ " km/h";

		} else if (type == 10) {
			convert << ySpeed*10;
			text = "y-speed: " + convert.str()+ "km/h";
		}
	}
	// constructor
	GameText(int txt, XInfo &xInfo) {
		type = txt;
		move = 1;
		if (txt == 1) {
			text = "Press SPACE to start";
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[1], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
			//x = (SCREEN_WIDTH - width) / 2;
			//y = (SCREEN_HEIGHT - 600 ) / 2 + 350;

		} else if (txt == 2) {
			text = "Lunar Lander";
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[1], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
			//x = (SCREEN_WIDTH - width) / 2;
			//y = (SCREEN_HEIGHT - 600 ) / 2 + 250;
		} else if (txt == 3) {
			text = "Window too small!!!";
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[1], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
			//x = (SCREEN_WIDTH - width) / 2;
			//y = (SCREEN_HEIGHT - 600 ) / 2 + 250;
		} else if (txt == 4) {
			text = "Game Paused.";
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[1], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
			//x = (SCREEN_WIDTH - width) / 2;
			//y = (SCREEN_HEIGHT - 600 ) / 2 + 250;
		} else if (txt == 5) {
			text = "FAILLLLLLL!!!";
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[1], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
			//x = (SCREEN_WIDTH - width) / 2;
			//y = (SCREEN_HEIGHT - 600 ) / 2 + 250;
		} else if (txt == 6) {
			text = "CONGRADULATION!!!";
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[1], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
			//x = (SCREEN_WIDTH - width) / 2;
			//y = (SCREEN_HEIGHT - 600 ) / 2 + 250;
		} else if (txt == 7) {
			text = "FUEL LEFT: 10";
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[2], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
		} else if (txt == 8) {
			text = "SELECT DIFFICULTY AND PRESS SPACE: EASY   NORMAL   HARD";
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[1], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
		} else if (txt == 9) {
			ostringstream convert;
			convert << xSpeed;
			text = "x-speed: " + convert.str();
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[2], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
		} else if (txt == 10) {
			ostringstream convert;
			convert << xSpeed;
			text = "x-speed: " + convert.str();
			XFontStruct *font;
			font = XLoadQueryFont(xInfo.display, "12x24");
			XSetFont(xInfo.display, xInfo.gc[2], font->fid);
			width = XTextWidth(font, text.c_str(), text.length());
		}
	}
	GameText() {
	}

	void setText(string t) {
		text = t;
	}
private:
	int type;
	int move;
	string text;
	int x;
	int y;
	int width;
};

//Ship ship;
star m_star;
triangle m_tri;
Boundary up_boundary;
Boundary down_boundary;
Boundary left_boundary;
Boundary right_boundary;
list<Displayable *> dList;
list<Displayable *> dList2;
GameText welcome;
GameText title;
GameText warning;
GameText paused;
GameText fail;
GameText win;
GameText fuel;
GameText x_Speed;
GameText y_Speed;
GameText selectD;

int labs_x[10];
int labs_y[10];

class Ship: public Displayable {
public:
	int x;
	int y;
	int rightBorder;
	int downBorder;
	virtual void paint(XInfo &xInfo) {
		if (xInfo.stage != 4 && won == 1) {
			XDrawRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x - 13,
					y - (m_diff - 45), 10, m_diff);
			if (fuelCount < (m_diff / 3)) {
				XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[2], x - 13,
						y - (fuelCount - 45), 10, fuelCount);
			} else {
				XFillRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x - 13,
						y - (fuelCount - 45), 10, fuelCount);
			}
			XDrawRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x + 10,
					y + 10, 15, 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x, y + 10, x,
					y + 25);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x, y + 10,
					x + 10, y);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x + 10, y,
					x + 25, y);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x + 25, y,
					x + 35, y + 10);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x + 35, y + 10,
					x + 35, y + 25);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x, y + 25,
					x + 10, y + 35);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x + 10, y + 35,
					x + 25, y + 35);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x + 25, y + 35,
					x + 35, y + 25);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x + 10, y + 35,
					x + 5, y + 45);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x, y + 45,
					x + 10, y + 45);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x + 25, y + 35,
					x + 30, y + 45);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x + 25, y + 45,
					x + 35, y + 45);
		} else if(xInfo.stage == 4) {
			explosion(xInfo);
		}
	}

	int getX(){
		return x;
	}
	int getY(){
		return y;
	}
	
	void thruster(int direction) {
		if (direction == UP) {
			if (vy - 1 != 0) {
				vy = vy - 1;
			} else {
				vy = vy - 2;
			}
		} else if (direction == DOWN) {
			vy = vy + 1;
		} else if (direction == LEFT) {
			vx = vx - 1;
		} else if (direction == RIGHT) {
			vx = vx + 1;
		}
	}

	void explosion(XInfo &xInfo) {
		xInfo.stage = 4;
		if (layer != 6) {
			XDrawArc(xInfo.display, xInfo.window, xInfo.gc[2], x-layer*15/2, y-layer*15/2+30, layer * 15,
					layer * 15, 0, 360 * 64);
			if (count % 15 == 0){
				layer++;
			}

		} else {
			layer = 6;

			xInfo.stage = 0;
			dList.push_front(&fail);
			//dList.push_front(&welcome);
			Fail = 1;
			count2 = 1;
		}
	}

	int checkSpeed(XInfo &xInfo) {
		if ((vx >= -3) && (vx <= 3) && (vy >= -3) && (vy <= 3)) {
			return 1;
		} else {
			return 0;
		}
	}

	// a function to update the position of the ship
	void move(XInfo &xInfo) {
		// horizontal speed is already 0.
		if (vx != 0) {
			if (vx > 0) {
				if (count % 70 == 0) {
					vx = vx - AIRRES;
					sabs_x = sabs_x - AIRRES;
				}
			} else {
				if (count % 70 == 0) {
					vx = vx + AIRRES;
					sabs_x = sabs_x + AIRRES;
				}
			}
		}
		// check the vertical speed
		if (vy < 0) {
			if (count % 100 == 0) { // these lines are just used to make the animation look smoother.
				vy = vy + AIRRES;
				vy = vy + GRA;
				sabs_y = sabs_y + AIRRES;
				sabs_y = sabs_y + GRA;
			}
		} else if (vy > 0) {
			if (count % 100 == 0) {
				//vy = vy - AIRRES;
				vy = vy + GRA;
				sabs_y = sabs_y + GRA;
			}
		} else {
			//if (count % 15 ==0){
			vy = vy + GRA;
			sabs_y = sabs_y + GRA;
			//}
		}

		//update position
		if (count % 4 == 0) {
			x = x + vx;
			sabs_x = vx + sabs_x;
			rightBorder = x + 35;
			y = y + vy;
			sabs_y = sabs_y + vy;
			downBorder = y + 45;
		}

		xSpeed = vx;
		ySpeed = vy;
	}

	// update position when resize
	void update(int xchange, int ychange) {
		x = sabs_x + (SCREEN_WIDTH - old_width) / 2;
		y = sabs_y + (SCREEN_HEIGHT - old_height) / 2;
	}

	//constructor
	Ship(int xp, int yp) {
		x = xp;
		y = yp;
		downBorder = y + 45;
		rightBorder = x + 35;
		vx = 0;
		vy = 0;
		sabs_x = x;
		sabs_y = y;
		layer = 1;
	}
	Ship() {
	}
private:
	int layer;
	int vx;
	int vy;
};

Ship ship;

class Lpad: public Displayable {
public:
	virtual void paint(XInfo &xInfo) {
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
				y_position, x_position + 100, y_position);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
				y_position + 15, x_position + 100, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
				y_position, x_position, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
				y_position, x_position + 15, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 15,
				y_position + 15, x_position + 30, y_position);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 30,
				y_position, x_position + 45, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 45,
				y_position + 15, x_position + 60, y_position);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 60,
				y_position, x_position + 75, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 75,
				y_position + 15, x_position + 90, y_position);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 90,
				y_position, x_position + 100, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 100,
				y_position, x_position + 100, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
				y_position + 15, x_position + 15, y_position);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 15,
				y_position, x_position + 30, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 30,
				y_position + 15, x_position + 45, y_position);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 45,
				y_position, x_position + 60, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 60,
				y_position + 15, x_position + 75, y_position);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 75,
				y_position, x_position + 90, y_position + 15);
		XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 90,
				y_position + 15, x_position + 100, y_position);

	}
	void checkLand(XInfo &xInfo) {
		if (((ship.x <= rightBorder) && (ship.downBorder >= y_position)
				&& (ship.x >= x_position))
				|| ((ship.rightBorder >= x_position)
						&& (ship.downBorder >= y_position)
						&& (ship.rightBorder <= rightBorder))) {
			//Make sure the speed is in the safe range.
			if (ship.checkSpeed(xInfo) == 1) {
				xInfo.stage = 0;
				dList.push_front(&win);
				//dList.push_front(&welcome);
				Fail = 1;
				count2 = 1;
				won = 1;
			} else {
				xInfo.stage = 0;
				dList.push_front(&fail);
				//dList.push_front(&welcome);
				Fail = 1;
				count2 = 1;
			}
		}
	}

	// contructor
	Lpad() {
	}

	Lpad(int pos, int t) {
		type = t;
		if (type == 1) {
			x_position = LEFT_BOUND + 100 * pos;
			y_position = DOWN_BOUND - 70;
		} else {
			x_position = LEFT_BOUND + 100 * pos;
			y_position = DOWN_BOUND - 170;
		}
		labs_x[type] = x_position;
		labs_y[type] = y_position;
		rightBorder = x_position + 100;
	}

	// update the position when resize
	void update(int xchange, int ychange) {
		x_position = labs_x[type] + (SCREEN_WIDTH - old_width) / 2;
		y_position = labs_y[type] + (SCREEN_HEIGHT - old_height) / 2;
		rightBorder = x_position + 100;
	}

private:
	int type;
	int x_position;
	int y_position;
	int rightBorder;
};


class fuelTank: public Displayable {
public:
	virtual void paint(XInfo &xInfo){
		check();
		if (count % 2 == 0 && xInfo.stage == 1){
			y = y+3;
		}
		XDrawImageString(xInfo.display, xInfo.window, xInfo.gc[1], x, y,
							text.c_str(), text.length());
//		XDrawRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x,
//							y, 46, 46);
		if (y+30 >= DOWN_BOUND){
			srand(time(0));
			int startPos = rand() % 8;
			x =  100*startPos;
			y = UP_BOUND;
		}
	}
	//constructor
	fuelTank(){
		srand(time(0));
		int startPos = rand() % 8;
		text = "F";
		x =  100;
		y = UP_BOUND;
	}
	void check(){
		if(x>=ship.getX() && x+15<=ship.getX()+35 && y>=ship.getY() && y+15<= ship.getY()){
			printf("!!!");
			reset();
			
		}
	}
	void reset(){
		srand(time(0));
		int startPos = rand() % 8+5;
		x = startPos * 100;
		y = UP_BOUND;
	}
	int getY(){
		return y;
	}
private:
	int x;
	int y;
	string text;
};

fuelTank tank;

int tabs_x[10];
int tabs_y[10];

class Terrain: public Displayable {
public:
	virtual void paint(XInfo &xInfo) {
		if (type == 1) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
					y_position, x_position + 20, y_position - 25);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 20,
					y_position - 25, x_position + 40, y_position);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 40,
					y_position, x_position + 75, y_position - 40);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 75,
					y_position - 40, x_position + 100, y_position + 45);
		} else if (type == 2) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
					y_position + 95, x_position + 10, y_position + 30);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 10,
					y_position + 30, x_position + 15, y_position + 60);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 15,
					y_position + 60, x_position + 20, y_position + 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 20,
					y_position + 15, x_position + 35, y_position - 30);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 35,
					y_position - 30, x_position + 75, y_position - 5);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 75,
					y_position - 5, x_position + 100, y_position - 125);
		} else if (type == 3) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
					y_position, x_position + 10, y_position + 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 10,
					y_position + 15, x_position + 15, y_position - 5);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 15,
					y_position - 5, x_position + 20, y_position - 20);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 20,
					y_position - 20, x_position + 35, y_position + 5);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 35,
					y_position + 5, x_position + 90, y_position - 5);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 90,
					y_position - 5, x_position + 100, y_position + 45);
		} else if (type == 4) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
					y_position + 80, x_position + 10, y_position - 10);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 10,
					y_position - 10, x_position + 15, y_position + 35);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 15,
					y_position + 35, x_position + 20, y_position + 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 20,
					y_position + 15, x_position + 35, y_position - 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 35,
					y_position - 15, x_position + 75, y_position);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 75,
					y_position, x_position + 100, y_position - 100);
		} else if (type == 6) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
					y_position, x_position + 10, y_position + 5);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 10,
					y_position + 5, x_position + 15, y_position + 35);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 15,
					y_position + 35, x_position + 20, y_position + 10);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 20,
					y_position + 10, x_position + 35, y_position - 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 35,
					y_position - 15, x_position + 75, y_position);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 75,
					y_position, x_position + 85, y_position - 10);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 85,
					y_position - 10, x_position + 95, y_position);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 95,
					y_position, x_position + 100, y_position + 250);
		} else if (type == 5) {
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
					y_position + 30, x_position + 10, y_position);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 10,
					y_position, x_position + 15, y_position + 35);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 15,
					y_position + 35, x_position + 20, y_position + 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 20,
					y_position + 15, x_position + 35, y_position - 15);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 35,
					y_position - 15, x_position + 75, y_position);
			XDrawLine(xInfo.display, xInfo.window, xInfo.gc[1], x_position + 75,
					y_position, x_position + 100, y_position + 100);
		} else {
			XDrawRectangle(xInfo.display, xInfo.window, xInfo.gc[1], x_position,
					y_position, 100, 50 * (type + 1));
		}
	}
	void checkCrash(XInfo &xInfo) {
		if (((ship.x <= rightBorder) && (ship.downBorder >= y_position - 15)
				&& (ship.x >= x_position))
				|| ((ship.rightBorder >= x_position)
						&& (ship.downBorder >= y_position - 15)
						&& (ship.rightBorder <= rightBorder))) {
//			xInfo.stage = 0;
//			dList.push_front(&fail);
//			//dList.push_front(&welcome);
//			Fail = 1;
//			count2 = 1;
			won = 0;
			ship.explosion(xInfo);
		}
	}
	// Constructer
	Terrain(int pos, int t) {
		// give the terrain a position
		// type1 = height 100
		// type2 = height 150
		// type3 = height 200
		// type4 = height 250
		// type5 = height 300
		// type6 = height 350
		type = t;
		x_position = LEFT_BOUND + 100 * pos;
		y_position = DOWN_BOUND - 50 * (type + 1);
		tabs_x[type] = x_position;
		tabs_y[type] = y_position;
		rightBorder = x_position + 100;
	}
	Terrain() {
	}
	// update the position when resize
	void update(int xchange, int ychange) {
		//x_position = x_position + xchange;
		x_position = tabs_x[type] + (SCREEN_WIDTH - old_width) / 2;
		//y_position = y_position + ychange;
		y_position = tabs_y[type] + (SCREEN_HEIGHT - old_height) / 2;
		rightBorder = x_position + 100;
	}

private:
	int x_position;
	int y_position;
	int type;
	int rightBorder;
};


Terrain t1;
Terrain t2;
Terrain t3;
Terrain t4;
Terrain t5;
Terrain t6;
Lpad p1;
Lpad p2;
//fuelTank tank;

/*
 * Intialize the whole game.
 */
void initGame(XInfo &xInfo) {
	srand(time(0));
	int startPos = rand() % 8;

	xSpeed = 0;
	ySpeed = 0;
	
	won = 1;

	m_star = star();
	m_tri = triangle();
	ship = Ship(LEFT_BOUND + 100, UP_BOUND + 100);
	up_boundary = Boundary(1);
	down_boundary = Boundary(2);
	left_boundary = Boundary(3);
	right_boundary = Boundary(4);
	t1 = Terrain(startPos, 1);
	p1 = Lpad((startPos + 1) % 8, 1);
	t2 = Terrain((startPos + 2) % 8, 2);
	t5 = Terrain((startPos + 3) % 8, 5);
	t3 = Terrain((startPos + 4) % 8, 3);
	p2 = Lpad((startPos + 5) % 8, 2);
	t4 = Terrain((startPos + 6) % 8, 4);
	t6 = Terrain((startPos + 7) % 8, 6);
	tank = fuelTank();
	dList.push_front(&t1);
	dList.push_front(&p1);
	dList.push_front(&t2);
	dList.push_front(&t5);
	dList.push_front(&t3);
	dList.push_front(&p2);
	dList.push_front(&t4);
	dList.push_front(&t6);
	dList.push_front(&ship);
	dList.push_front(&up_boundary);
	dList.push_front(&down_boundary);
	dList.push_front(&left_boundary);
	dList.push_front(&right_boundary);
	dList.push_front(&x_Speed);
	dList.push_front(&y_Speed);
	dList.push_back(&tank);

	is_first_time = 0;
}

// A function to check if the ship crash into terrains or screen border.
void checkCrash(XInfo &xInfo) {
	//check border
	if (ship.x + 35 <= LEFT_BOUND || ship.y + 45 <= UP_BOUND
			|| ship.y >= DOWN_BOUND || ship.x >= RIGHT_BOUND) {
//		xInfo.stage = 0;
//		dList.push_front(&fail);
//		//dList.push_front(&welcome);
//		Fail = 1;
//		count2 = 1;
		won = 0;
		ship.explosion(xInfo);
	}
	//check terrain
	if (count % 2 == 0) {
		p1.checkLand(xInfo);
		p2.checkLand(xInfo);
		t1.checkCrash(xInfo);
		t2.checkCrash(xInfo);
		t3.checkCrash(xInfo);
		t4.checkCrash(xInfo);
		t5.checkCrash(xInfo);
		t6.checkCrash(xInfo);
	}
}

void handleAnimation(XInfo &xInfo) {
	// display the speed of the ship
	x_Speed.setText();
	y_Speed.setText();

	if (xInfo.stage == 1) {
		ship.move(xInfo);
		checkCrash(xInfo);
	} else if (xInfo.stage == 0 || xInfo.stage == 2 || xInfo.stage == 4) {
		//flash message
		if (Fail == 1) {
			if (count % 30 == 0) {
				if (count2 == 1) {
					if (dList2.size() == 0) {
						count2 = 0;
						dList2.push_back(&welcome);
					}
				} else {
					if (dList2.size() == 1) {
						dList2.clear();
						count2 = 1;
					}
				}
			}
		}

	}
}

void handleKeyPress(XInfo &xInfo, XEvent &event) {
	KeySym key;
	char text[BufferSize];
	/*
	 * Exit when 'q' is typed.
	 */
	int i = XLookupString((XKeyEvent *) &event, text, BufferSize, &key, NULL);
	if (i == 1) {
		if (text[0] == 'q') {
			error("Terminating normally.");
		} else if (text[0] == ' ' && xInfo.stage == 3) {
			if (m_tri.getLv() == 1) {
				fuelCount = 60;
				m_diff = 60;
			} else if (m_tri.getLv() == 2) {
				fuelCount = 45;
				m_diff = 45;
			} else if (m_tri.getLv() == 3) {
				fuelCount = 30;
				m_diff = 30;
			}
			xInfo.stage = 1;
			dList.clear();
			initGame(xInfo);
		} else if (text[0] == ' ' && xInfo.stage != 3 && xInfo.stage != 4) {
			Fail = 0;
			if (xInfo.stage == 0) {
				xInfo.stage = 3;
				dList.clear();
				dList2.clear();
				dList.push_front(&selectD);
				dList.push_front(&m_tri);
			} else if (xInfo.stage == 1) {
				// pause/resume game
				xInfo.stage = 2;
				paused = GameText(4, xInfo);
				dList.push_back(&paused);
				dList.push_back(&welcome);
				count2 = 1;
				Fail = 1;
			} else {
				xInfo.stage = 1;
				dList2.clear();
				dList.pop_back();
				dList.pop_back();
			}
		}
	} else {
		if (fuelCount > 0 && xInfo.stage == 1) {
			keyPress++;
			if (keyPress % 2 == 0){
				if (key == XK_Up) {
					ship.thruster(UP);
					fuelCount = fuelCount - 1;
				} else if (key == XK_Down) {
					ship.thruster(DOWN);
					fuelCount = fuelCount - 1;
				} else if (key == XK_Left) {
					ship.thruster(LEFT);
					fuelCount = fuelCount - 1;
				} else if (key == XK_Right) {
					ship.thruster(RIGHT);
					fuelCount = fuelCount - 1;
				}
			}
		} else if (xInfo.stage == 3) {
			if (key == XK_Left) {
				m_tri.moveLeft();
			} else if (key == XK_Right) {
				m_tri.moveRight();
			}

		}
	}
}

void handleResize(XInfo &xInfo, XEvent &event) {
	XConfigureEvent xce = event.xconfigure;
	int xChange = (xce.width - 800) / 2;
	int yChange = (xce.height - 600) / 2;
	int xc;
	int yc;
	if (xce.width != SCREEN_WIDTH || xce.height != SCREEN_HEIGHT) {
		SCREEN_WIDTH = xce.width;
		SCREEN_HEIGHT = xce.height;
		xc = (SCREEN_WIDTH - o_SCREEN_WIDTH) / 2;
		yc = (SCREEN_HEIGHT - o_SCREEN_HEIGHT) / 2;
		o_SCREEN_WIDTH = SCREEN_WIDTH;
		o_SCREEN_HEIGHT = SCREEN_HEIGHT;
	}

	if (SCREEN_WIDTH < 800 || SCREEN_HEIGHT < 600) {
		blank = 1;
	} else if (SCREEN_WIDTH == 800 || SCREEN_HEIGHT == 600) {
	} else {
		UP_BOUND = yChange;
		DOWN_BOUND = 600 + yChange;
		LEFT_BOUND = xChange;
		RIGHT_BOUND = 800 + xChange;
		t1.update(xc, yc);
		t2.update(xc, yc);
		t3.update(xc, yc);
		t4.update(xc, yc);
		t5.update(xc, yc);
		t6.update(xc, yc);
		p1.update(xc, yc);
		p2.update(xc, yc);
		m_tri.update();
		ship.update(xc, yc);
		blank = 0;
	}
}

/*
 * Function to repaint a display list
 */
void repaint(XInfo &xinfo) {
	if (blank == 0) {
		list<Displayable *>::const_iterator begin = dList.begin();
		list<Displayable *>::const_iterator end = dList.end();

		list<Displayable *>::const_iterator begin2 = dList2.begin();
		list<Displayable *>::const_iterator end2 = dList2.end();

		XClearWindow(xinfo.display, xinfo.window);

		// draw display list
		while (begin != end) {
			Displayable *d = *begin;
			d->paint(xinfo);
			begin++;
		}

		while (begin2 != end2) {
			Displayable *d = *begin2;
			d->paint(xinfo);
			begin2++;
		}
		XFlush(xinfo.display);
	} else { // display warning message when the window is too small
		warning = GameText(3, xinfo);
		warning.paint(xinfo);
	}
}

/*
 * Intialize X and create a window.
 */
void initX(int argc, char *argv[], XInfo &xInfo) {
	XSizeHints hints;
	unsigned long white, black, red;

	/* Display opening uses the DISPLAY environment variable.
	 * It can go wrong if DISPLAY isn't set, or you don't have permission.
	 */
	xInfo.display = XOpenDisplay("");
	if (!xInfo.display) {
		error("Can't open display.");
	}

	/*
	 * Find out some things about the display you're using.
	 */
	xInfo.screen = DefaultScreen(xInfo.display);

	white = XWhitePixel(xInfo.display, xInfo.screen);
	black = XBlackPixel(xInfo.display, xInfo.screen);

	//get the red pixel
	string reds = "red1";
	Colormap cmap;
	XColor near_color, true_color;
	cmap = DefaultColormap(xInfo.display, 0);
	XAllocNamedColor(xInfo.display, cmap, reds.c_str(), &near_color,
			&true_color);
	red = near_color.pixel;

	hints.x = 100;
	hints.y = 100;
	hints.width = SCREEN_WIDTH;
	hints.height = SCREEN_HEIGHT;
	hints.flags = PPosition | PSize;

	UP_BOUND = 0;
	DOWN_BOUND = 600;
	LEFT_BOUND = 0;
	RIGHT_BOUND = 800;

	xInfo.window = XCreateSimpleWindow(xInfo.display,
			DefaultRootWindow(xInfo.display), hints.x, hints.y, hints.width,
			hints.height, Border, white, black);

	XSetStandardProperties(xInfo.display, xInfo.window, "Lunar Launcher",
			"Lunar Launcher", None, argv, argc, &hints);
	// Set the stage to initial stage.
	xInfo.stage = 0;

	// Create Graphics Contexts for start screen
	int i = 3;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], black);
	XSetBackground(xInfo.display, xInfo.gc[i], white);
	XSetFillStyle(xInfo.display, xInfo.gc[i], FillSolid);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineSolid, CapButt,
			JoinRound);

	i = 2;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], red);
	XSetBackground(xInfo.display, xInfo.gc[i], black);
	XSetLineAttributes(xInfo.display, xInfo.gc[i], 1, LineOnOffDash, CapButt,
			JoinRound);

	i = 1;
	xInfo.gc[i] = XCreateGC(xInfo.display, xInfo.window, 0, 0);
	XSetForeground(xInfo.display, xInfo.gc[i], white);
	XSetBackground(xInfo.display, xInfo.gc[i], black);

	XSelectInput(xInfo.display, xInfo.window,
			ButtonPressMask | KeyPressMask | PointerMotionMask
					| StructureNotifyMask);

	XMapRaised(xInfo.display, xInfo.window);
	XFlush(xInfo.display);
	sleep(1);

	//Draw the instruction screen
	welcome = GameText(1, xInfo);
	title = GameText(2, xInfo);
	fuel = GameText(7, xInfo);
	x_Speed = GameText(9, xInfo);
	y_Speed = GameText(10, xInfo);
	win = GameText(6, xInfo);
	fail = GameText(5, xInfo);
	selectD = GameText(8, xInfo);
}

// get microseconds
unsigned long now() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void eventLoop(XInfo &xinfo) {

	// Add stuff to paint to the display list
	dList.push_front(&title);
	dList.push_front(&welcome);

	XEvent event;
	unsigned long lastRepaint = 0;
	count = 0;
	while (true) {
		if (XPending(xinfo.display) > 0) {
			XNextEvent(xinfo.display, &event);
			switch (event.type) {
			case ButtonPress:
				break;
			case KeyPress:
				handleKeyPress(xinfo, event);
				break;
			case MotionNotify:
				break;
			case ConfigureNotify:
				handleResize(xinfo, event);
				break;
			}
		}
		// animation timing
		unsigned long end = now();
		if (end - lastRepaint > 1000000 / FPS) {
			handleAnimation(xinfo);
			repaint(xinfo);
			count = count + 1;
			lastRepaint = now();
		}
		// give the system time to do other things
		if (XPending(xinfo.display) == 0) {
			usleep(1000000 / FPS - (end - lastRepaint));
		}

	}
}

/*
 * Start executing here.
 * First initialize window.
 * Next loop responding to events.
 * Exit forcing window manager to clean up.
 */
int main(int argc, char *argv[]) {
	XInfo xInfo;
	initX(argc, argv, xInfo);
	eventLoop(xInfo);
	XCloseDisplay(xInfo.display);
}
