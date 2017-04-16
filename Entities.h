/*
Contains info about the Pacman, Ghost, Dots, Powerups
*/
#include <stdlib.h>
#include "coordindex.h"
#include <iostream>

#define MAP_DIM 5//by 5

using std::cout;
using std::endl;

class Entity{ //same thing as an empty space
	char symb; 
	int rowpos, colpos, pntVal;
	bool eatable;
public:
	Entity(): symb(' '), rowpos(-1), colpos(-1), pntVal(0), eatable(true){}
	Entity(int r, int c) : symb(' '), rowpos(r), colpos(c) , pntVal(0), eatable(true) {}
	char getSymb()						{ return symb; };
	int get_rowpos()						{ return rowpos; };
	int get_colpos()						{ return colpos; };
	int getPntVal()						{ return pntVal; };
	bool isEatable()					{ return eatable; };

	void setSymb(char val)			{ symb = val; };
	void set_rowpos(int val)				{ rowpos = val;  };
	void set_colpos(int val)				{ colpos = val;  };
	void setPntVal(int val)				{ pntVal = val;  };
	void setEatable(bool val)			{ eatable = val;  };
};

class Unknown : public Entity {
	//to maybe be implemented
};

class Consumable : public Entity {
public:
	Consumable() :Entity() {}
	Consumable(int x, int y) :Entity(x, y) {}
	//virtual void gotEaten(); might implement later, most likely not
};

class Pellet : public Consumable{
	
public:
	Pellet() :Consumable(){ 
		setSymb('*');
		setPntVal(10);
	}

	Pellet(int x, int y) :Consumable(x, y){
		setSymb('*');
		setPntVal(10);
	}	
};

class PwrPlt : public Pellet{

public:
	PwrPlt() :Pellet() {
		setSymb('@');
		setPntVal(50);
	}
	PwrPlt(int x, int y) :Pellet(x, y) {
		setSymb('@');
		setPntVal(50);
	}
};


class Roamer :public Entity {
	char dir;
public:
	Roamer() : Entity(){
		dir = 'n';
	}
	Roamer(int x, int y) : Entity(x, y) {
		dir = 'n';
	}

	char rand_dir();
	char get_dir() { return dir; }
	void set_dir(char a) { dir = a; }
	int advance(int current, char dir);//advances one tile in specified direction, returns the end tile
};

class Pac :public Roamer {
public:
	Pac() :Roamer() {
		setSymb('P');
	}
	Pac(int x, int y) :Roamer(x, y) {}

};

class Ghost :public Roamer {
	char direction, backwards;
public:
	Ghost() :Roamer() {
		setSymb('G');
		setPntVal(200);
		direction = 'n';
		backwards = 's';
	}
	Ghost(int x, int y) :Roamer(x, y) {}
	int adjustDestination(char dir);//sets direction and backwards, then returns the next tile in that direction
	void setBackwards(char back) { backwards = back; };
	char getDirection() { return direction; }
	char getBackwards() { return backwards; }
	int move(int currentpos, int targetpos, bool[22], bool corrected); //moves one tile towards target tile, uses an array of intersection tiles for speed, target is usually pacbot
	void move_scatter(int cornerpos); //the ghosts retreat to a corner of the map
	void move_chase(int pacpos);
	void move_frightened();
};
