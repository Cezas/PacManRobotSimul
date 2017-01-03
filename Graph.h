#include "Entities.h"
#include <vector>
#include <map>
#include <list>
#include <iterator>
#include <string>

using std::map;
using std::list;
using std::vector;
using std::string;

/*
Contains the definitions of the following classes:
	Tile: A tile of the map, contains an Entity value that can hold dots, Ghostses, SuperPac, etc.  Also holds pointers to nearby tiles.
	Map: The actual map itself, which contains a vector(a better array) of Tiles, and can display what the hell is happening (eventually)
*/

class Tile{
	Entity *val;
	int indexVal;
	Tile* north;
	Tile* south;
	Tile* west;
	Tile* east;
	vector<Tile*> neighbors;
	vector<Tile*>::iterator iter;  //navigates thru the vector

public:
	Tile(int index):indexVal(index) {}
	Tile(Entity* ent,int index) : val(ent), indexVal(index){}

	Entity* getEnt()		{ return val; }
	void setEnt(Entity* e)	{ val = e; }
	int getIndex()			{ return indexVal; }

	vector<Tile*> getNeighbors() { return neighbors; }

	Tile* getNorth()		{ return north; }
	Tile* getSouth()		{ return south; }
	Tile* getWest()			{ return west; };
	Tile* getEast()			{ return east; };

	void setNorth(Tile* t) { north = t; }
	void setSouth(Tile* t) { south = t; }
	void setWest(Tile* t) { west = t; }
	void setEast(Tile* t) { east = t; }


	void setNeighbor(Tile*, char);
	string getStrNeighbors();
	//void removeNeighbor(Tile*, char);
};

class Map{
	int totalTiles;
	int sqrtOfTiles;  //effectively the length/width of the grid, this will be used in all sorts of calculations
	vector<Tile> tiles;
	void visitNeighbors(int); //designed tile connects to nearby tiles and adds to neighbors list
	Pellet p;
	PwrPlt pp;

public://the constructor sets up each tile, but does not "connect" them (no neighbors detected)
	Map(int numOfVertices) :totalTiles(numOfVertices), sqrtOfTiles(sqrt(numOfVertices)) {
		tiles.reserve(totalTiles);
		for (int i = 0; i < numOfVertices; i++) {
			Tile insert(&p, i);
			tiles.push_back(insert);
		}
		tiles.at(4).setEnt(&pp);
		tiles.at(7).setEnt(&pp);
		
			
	} 

	int getSqrtTiles() { return sqrtOfTiles; }
	Entity* getEntAt(int);
	void setEntAt(int, Entity*);

	Tile getTileAt(int);

	void meetNGreet(); //every tile in the map connects to their neighbors
	
	void addNeighbor(int, Tile*, char); //adds a neighbor(Tile*) to tiles(int) and specifies where neighbor is(char)
	void removeNeighbor(int, char); //which direction should i cut off from where i am?
	void makeWall(int, int, int, int); //given robot position and impassable tile position, update the adjacency list
	void deleteTile(int, int);
	
	vector<int> BFS(int, int, int, int);  //returns the shortest paths from A to B
	void printAdjList();
	void printMap();
};