#include "entities.h"
#include <vector>
#include <map>
#include <list>
#include <iterator>
#include <string>
#include <cmath>

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
	Ghost *ghost;
	int indexVal;
	Tile* north;
	Tile* south;
	Tile* west;
	Tile* east;
	vector<Tile*> neighbors;
	vector<Tile*>::iterator iter;  //navigates thru the vector

public:
	//constructors for the tile
	Tile(int index):indexVal(index) {}
	Tile(Entity* ent,int index) : val(ent), indexVal(index){}
	//tile stuff getters and setters
	Entity* getEnt()		{ return val; }
	void setEnt(Entity* e)	{ val = e; }
	Ghost* getGhost()		{ return ghost; }
	void setGhost(Ghost* g) { ghost = g; }
	int getIndex()			{ return indexVal; }
	//neighbor-related getters and setters
	vector<Tile*> getNeighbors() { return neighbors; }
	Tile* getNorth()		{ return north; }
	Tile* getSouth()		{ return south; }
	Tile* getWest()			{ return west; }
	Tile* getEast()			{ return east; }
	bool isNorthOf(int otherTilePos) { if (this->getSouth()->getIndex() == otherTilePos) return true; return false; }
	bool isSouthOf(int otherTilePos) { if (this->getNorth()->getIndex() == otherTilePos) return true; return false; }
	bool isEastOf(int otherTilePos) { if (this->getWest()->getIndex() == otherTilePos) return true; return false; }
	bool isWestOf(int otherTilePos) { if (this->getEast()->getIndex() == otherTilePos) return true; return false; }
	void setNorth(Tile* t) { north = t; }
	void setSouth(Tile* t) { south = t; }
	void setWest(Tile* t) { west = t; }
	void setEast(Tile* t) { east = t; }
	void setNeighbor(Tile*, char);
	string getStrNeighbors();
};

class Map{
	const int totalTiles;
	int sqrtOfTiles;  //effectively the length/width of the grid, this will be used in all sorts of calculations
	vector<Tile> tiles;
	void visitNeighbors(int); //designed tile connects to nearby tiles and adds to neighbors list
	Pellet p;
	PwrPlt pp;
	bool intersects[MAP_DIM*MAP_DIM];
public://the constructor sets up each tile, but does not "connect" them (no neighbors detected)
	Map(int numOfVertices) :totalTiles(numOfVertices), sqrtOfTiles(sqrt(numOfVertices)) {
		tiles.reserve(totalTiles);
		for (int i = 0; i < numOfVertices; i++) {
			Tile insert(&p, i);
			tiles.push_back(insert);
			intersects[i] = false;
		}			
	} 

	void initialize();
	int getSqrtTiles() { return sqrtOfTiles; }

	Entity* getEntAt(int index) { return tiles.at(index).getEnt(); };
	void setEntAt(int index, Entity* insert);
	Ghost* getGhostAt(int index) { return tiles.at(index).getGhost(); };;
	void setGhostAt(int index, Ghost* insert);
	void removeGhostAt(int index) { tiles.at(index).setGhost(NULL); }
	Tile getTileAt(int index) { return tiles.at(index); };
	void setTileAt(int index, Tile insert) { tiles.at(index) = insert; };

	void meetNGreet(); //every tile in the map connects to their neighbors
	
	void makeNeighbor(int, Tile*, char); //adds a neighbor(Tile*) to tiles(int) and specifies where neighbor is(char)
	//void makeNeighbor(int, int, int, int);//assuming these two tiles are adjacent, make them neighbors

	void removeNeighbor(int, char); //which direction should i cut off from where i am?
	void makeWall(int, int, int, int); //given robot position and impassable tile position, update the adjacency list
	void deleteTile(int, int);
	
	vector<int> BFS(int, int, int, int);  //returns the shortest paths from A to B
	void printAdjList();
	void printMap();
	vector<int> look(int, int); //takes info 4 directions from whatever map is calling it and returns it
	void look(Map &realmap, vector<int>, bool[25]); //implements the vector look, and is what really reveals the map
	bool hasDied(int, int, int, int);//given a ghost index and the pacbot index, if there is no wall, lose a life
	vector<char> wallExists(int,int);//determines if a wall exists between those two tiles, returns other valid tiles
	vector<char> validPathsAt(int);//returns all valid paths of a certain tile
	int wallAhead(Ghost*, int to_pos);//determines if there is a wall ahead and recalculates the route (for ghost right now)
	int moveGhost(Ghost*, Pac*, bool);
};
