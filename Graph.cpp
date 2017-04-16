#include "graph.h"
#include <iostream>
#include "coordindex.h"
#include <ctype.h>

//using std::cout;
//using std::endl;
using namespace std;

void Tile::setNeighbor(Tile* t, char dir) {
	int test = tolower(dir);
	if (test != 'n' && test != 's' && test != 'w' && test != 'e')//must be correct input
		cout << "Failed to set neighbor, must enter one of the following directions: n(north), s(south), w(west), e(east)" << endl;

	else {
		if (t == NULL) {//if wanting to remove, check to see if tile is in neighbors before removal
			if (neighbors.empty())
				cout << "Already empty" << endl;

			else {
				//find out which one to remove exactly
				if (test == 'n')
					t=getNorth();
				if (test == 's')
					t=getSouth();
				if (test == 'w')
					t=getWest();
				if (test == 'e')
					t=getEast();

				//removal from neighbors
				if (t == NULL)//if trying to remove a tile that doesnt exist, dont
					cout << "Tile does not exist, cannot remove" << endl;
				else {//begin search thru vector to remove that tile			
					iter = neighbors.begin();
					while (iter != neighbors.end()) {
						if (*iter == t)
							iter = neighbors.erase(iter);
						else
							++iter;
					}
					t = NULL;
				}
			}
		}//done removal

		else {//wanting to insert
			neighbors.push_back(t);
		}//done insert, now adjust pointers
		if (test == 'n')
			setNorth(t);
		if (test == 's')
			setSouth(t);
		if (test == 'w')
			setWest(t);
		if (test == 'e')
			setEast(t);
	}

}

string Tile::getStrNeighbors() {
	string output = to_string(getIndex())+": <";

	if(getNorth()==NULL)
		output += "N: None |";	
	else
		output += "N: " + to_string(getNorth()->getIndex())+" |";

	if (getSouth() == NULL)
		output += "S: None |";
	else
		output += "S: " + to_string(getSouth()->getIndex())+" |";

	if (getWest() == NULL)
		output += "W: None |";
	else
		output += "W: " + to_string(getWest()->getIndex())+" |";

	if (getEast() == NULL)
		output += "E: None |";
	else
		output += "E: " + to_string(getEast()->getIndex())+" |";

	output += ">   ";

	//now add the vector to the output
	string vect = "   |||   <";

	//this code here was meant to be used for a list to hold the neighbors, but i couldnt get it working 
	/*if (neighbors.empty())
		;
	else {
		iter = neighbors.begin();
		while (iter != neighbors.end()) {
			vect += to_string(*iter->getIndex());
			++iter;
		}
	}*/
	for (int i = 0; i < getNeighbors().size(); i++) {
		vect += to_string(getNeighbors().at(i)->getIndex())+", ";
	}
	vect += ">";
	output += vect;

	return output;
}


void Map::setEntAt(int index, Entity* insert){
	tiles.at(index).setEnt(insert);
	insert->set_rowpos(indexToRownum(index, sqrtOfTiles));
	insert->set_colpos(indexToColnum(index, sqrtOfTiles));
}

void Map::setGhostAt(int index, Ghost* insert) {
	tiles.at(index).setGhost(insert);
	insert->set_rowpos(indexToRownum(index, sqrtOfTiles));
	insert->set_colpos(indexToColnum(index, sqrtOfTiles));
}

void Map::meetNGreet(){
	for (int i = 0; i < totalTiles; i++)
		visitNeighbors(i);
}


void Map::visitNeighbors(int index) { //connects all the tiles to their respective neighbors, map is assumed wall-less at first
	Tile* temp;
	
	if (index > sqrtOfTiles - 1) {
		temp = &tiles.at(index - sqrtOfTiles);
		makeNeighbor(index, temp,'n');
	}

	if (index < totalTiles - sqrtOfTiles) {
		temp = &tiles.at(index + sqrtOfTiles);
		makeNeighbor(index, temp, 's');
	}
	if (index % sqrtOfTiles != 0) {
		temp = &tiles.at(index - 1);
		makeNeighbor(index, temp, 'w');
	}
	if ((index + 1) % sqrtOfTiles != 0) {
		temp = &tiles.at(index + 1);
		makeNeighbor(index, temp, 'e');
	}
}

vector<char> Map::validPathsAt(int pos) {
	vector<Tile*> neighbors;
	vector<char> paths;
	neighbors = tiles.at(pos).getNeighbors();
	
	for (int i = 0; i < neighbors.size(); i++){
		if (neighbors.at(i) == tiles.at(pos).getNorth())
			paths.push_back('n');
		if (neighbors.at(i) == tiles.at(pos).getSouth())
			paths.push_back('s');
		if (neighbors.at(i) == tiles.at(pos).getEast())
			paths.push_back('e');
		if (neighbors.at(i) == tiles.at(pos).getWest())
			paths.push_back('w');
		}
	return paths;
}

vector<char> Map::wallExists(int pos1, int pos2) {
	vector<Tile*> neighbors;
	//cout << "pos1: "<<pos1 <<"pos2: "<< pos2<<endl; //DEBUG
	neighbors = tiles.at(pos1).getNeighbors();//get the neighbors for the first tile, and see if there is a connection to pos2
	//for (int i = 0; i < neighbors.size(); i++)
		//cout << neighbors.at(i)->getIndex();
	if (pos2 == -1) //if out of grid
		return validPathsAt(pos1);
	
	
	for (int i = 0; i < neighbors.size(); i++) {
		if (pos2 == neighbors.at(i)->getIndex())
            return vector<char>();//if there is a connection, then return an empty vector to show there is no wall between the two
	}

	return validPathsAt(pos1);
}

int Map::wallAhead(Ghost* g, int g_dest){
	int r_g= g->get_rowpos(), c_g= g->get_colpos();

	vector<char>otherpaths = wallExists(coordToIndex(r_g,c_g,sqrtOfTiles),g_dest);

	//if there is a wall infront, orient to another valid destination
	if (otherpaths.size() > 1) {
		g->rand_dir();
		//for (int i = 0; i < otherpaths.size(); i++) //DEBUG
		//cout << otherpaths.at(i) << endl;
		for (int i = 0; i < otherpaths.size(); i++) {
			if (otherpaths[i] != g->getBackwards()) { //change its direction so its not hitting a wall or going backwards
				g->set_dir(otherpaths[i]);
				char direction = g->get_dir();
				if (direction == 'n')
					g_dest = coordToIndex(r_g - 1, c_g, sqrtOfTiles);

				if (direction == 's')
					g_dest = coordToIndex(r_g + 1, c_g, sqrtOfTiles);

				if (direction == 'e')
					g_dest = coordToIndex(r_g, c_g + 1, sqrtOfTiles);

				if (direction == 'w')
					g_dest = coordToIndex(r_g, c_g - 1, sqrtOfTiles);

				i = otherpaths.size();
			}
				//cout << "corrected " << g1_dest <<","<<direction<<endl;//DEBUG
				//exit after the first instance		
		}
		return g_dest;
	}
    //else //check
}

int Map::moveGhost(Ghost* g, Pac* p, bool corrected) {
	int g_pos = coordToIndex(g->get_rowpos(), g->get_colpos(), sqrtOfTiles);
	int p_pos = coordToIndex(p->get_rowpos(), p->get_colpos(), sqrtOfTiles);
    //cout << g_pos;
	if (intersects[g_pos]) {//incase it moves backwards due to intersection, correct it
		//cout << '\a';
		int temppos = g_pos;
		char backwards = g->getBackwards();
		g_pos = g->move(g_pos, p_pos, intersects, corrected);
		if (g->get_dir() == backwards) {

			if (backwards == 'n') 
				g->set_dir('s');			

			if (backwards == 's') 
				g->set_dir('n');		

			if (backwards == 'e') 
				g->set_dir('w');
			
			if (backwards == 'w') 
				g->set_dir('e');

			g_pos = g->move(temppos, p_pos, intersects, true);
			g_pos = g->move(g_pos, p_pos, intersects, true);
		}
	}
	else
		g_pos = g->move(g_pos, p_pos, intersects, corrected);

	g->set_rowpos(indexToRownum(g_pos, sqrtOfTiles));
	g->set_colpos(indexToColnum(g_pos, sqrtOfTiles));

	return g_pos;
}


void Map::makeNeighbor(int index, Tile *to_insert, char dir){
	int r = 0, c = 0;
	try{
		tiles.at(index).setNeighbor(to_insert, dir);
		/*//test code
		if (dir == 'n') {
			 r = indexToRownum(index, sqrtOfTiles);
			 c = indexToColnum(index, sqrtOfTiles);
			 tiles.at(coordToIndex(r - 1, c, sqrtOfTiles)).setNeighbor(tiles.at(index).getSouth(), 's');
		}//end test code*/
	}
	catch (std::out_of_range e){
		cout << "Out of range." << endl;
	}
}

void Map::removeNeighbor(int index, char dir){
	tiles.at(index).setNeighbor(NULL, dir);
}

//precond: since this makes a wall as the robot drives, it makes sense that the two indices should be right next to each other
//PRE COND: either the row or the columns must be equal
void Map::makeWall(int r1, int c1, int r2, int c2) {  //1:robot position   2: target position
	int target = coordToIndex(r2, c2, sqrtOfTiles);
	int orig = coordToIndex(r1, c1, sqrtOfTiles);
	int rowdif= r2 - r1;
	int coldif = c2 - c1;

	if (abs(rowdif + coldif) != 1)
		cout << "Invalid parameters to build wall" << endl;

	else if (rowdif == 0) { //when in the same row

		if (coldif == 1) {  //col2 leads by one, target to the east
			removeNeighbor(orig, 'e');//remove the neighbor of this tile from the east
			removeNeighbor(target, 'w'); //removes the neighbor of the target tile to the robot
		}

		if (coldif == -1) {  //col2 lags by one, target to the west
			removeNeighbor(orig, 'w');//remove the neighbor of this tile from the west
			removeNeighbor(target, 'e'); //removes the neighbor of the target tile to the robot
		}
	}

	else if (coldif == 0) { //when in the same col

		if (rowdif == 1) {  //row2 leads by one, target to the south
			removeNeighbor(orig, 's');
			removeNeighbor(target, 'n'); 
		}

		if (rowdif == -1) {  //row2 lags by one, target to the north
			removeNeighbor(orig, 'n');
			removeNeighbor(target, 's');
		}
	}

}

void Map::printAdjList(){
	for (int i = 0; i < totalTiles; i++)
		cout << tiles.at(i).getStrNeighbors() << endl;
	cout << "--------------------------  \n";
}
/*
KEY:
	--- and | are valid connections
	P is the robot
	A B C D are the ghostses
	* is a regular pellet
	@ is a power pellet
	% is a bonus(?)
	? are unknowns
	# is a wall
*/
void Map::printMap() {

	for (int i = 0; i < sqrtOfTiles; i++){ 
		string output1 = "", output2="";  //output1->current row  output2->southern connections
		
		if (i == 0) {
			for (int k = 0; k < sqrtOfTiles; k++) 
				output1 += "  "+to_string(k) + " ";
			
			cout << output1 << endl;
			output1 = "";
		} 
		for (int j = 0; j < sqrtOfTiles; j++) {
			if(j==0)
				output1 += to_string(i) + " ";

			if (tiles.at(j + i*(sqrtOfTiles)).getGhost() != NULL) {
				output1 += tiles.at(j + i*(sqrtOfTiles)).getGhost()->getSymb();
			}
			else
				output1 += tiles.at(j+i*(sqrtOfTiles)).getEnt()->getSymb();
			//if above tile connection is null, wall
			if (tiles.at(j+i*sqrtOfTiles).getEast() == NULL)
				output1 += "   ";
			else
				output1 += "---";
			
			if (tiles.at(j+i*sqrtOfTiles).getSouth() == NULL)
				output2 += "    ";
			else
				output2 += "  | ";
		}
		cout << output1 << endl;
		cout << output2 << endl;
	}
}


vector<int> Map::BFS(int r1, int c1, int r2, int c2) {
	bool* visited= new bool[totalTiles]; //every tile is set to not visited at first
	list<int> queue; //a waitlist to visit neighbors
	list<int>::iterator it;
	vector<Tile*> temp;
	list<int> indices; //neighbor indices
	map<int, int> pairs;  //each pair will be (child, parent)
	vector<int> path;//final path

	int child = 0;
	int parent = coordToIndex(r1, c1, sqrtOfTiles);
	
	for (int i = 0; i < totalTiles; i++)
		visited[i] = false;

	//push the current tile into the visited pile, input value should be first pair into map
	visited[parent] = true;
	queue.push_back(parent);

	while (!queue.empty()) { //while there are still tiles to look through
		//take the current tile and add its neighbors to the end of the queue
		//cout << "queu" << endl;
		parent = queue.front();
		queue.pop_front();
		temp = tiles.at(parent).getNeighbors();
		//cout << parent << endl;

		for (int i = 0; i < temp.size(); i++) //gets the tile indices
			indices.push_back(temp.at(i)->getIndex());
		
		//filters out whats been visited or not, then adds to the queue and makes a pair
		for (it = indices.begin(); it != indices.end(); it++) {
			//cout << *it << endl;
		    if (!visited[*it]) {
				visited[*it] = true;
				queue.push_back(*it);
//                pairs.insert(pair<int,int>(*it, parent));
                pairs.insert({*it, parent});
			}

			//if found the desired tile, start tracing back
			if (*it == coordToIndex(r2, c2, sqrtOfTiles)) {
				child = *it;
			}				
		}

	}
	while (parent != coordToIndex(r1, c1, sqrtOfTiles)) {
		path.push_back(child);
		parent = pairs[child];
		path.push_back(parent);
		child = parent;
	}
	cout << "BFS from (" << r1 << ", " << c1 << ") to (" << r2 << ", " << c2 << ") " << endl;
	for (int i = 0; i < path.size(); i++) {
		cout << path.at(i) << ", ";
		if ((i + 2) % 2 == 1)
			cout << endl;
	}
	return path;
}

void Map::deleteTile(int r, int c){
	int temp = coordToIndex(r, c, sqrtOfTiles); //break the neighbors' connections to this	
	try {
		removeNeighbor(temp, 'e');//remove the neighbor of this tile from the east
		removeNeighbor(temp + 1, 'w'); //removes the neighbor of the target tile to the robot
		removeNeighbor(temp, 'w');//remove the neighbor of this tile from the west
		removeNeighbor(temp - 1, 'e'); //removes the neighbor of the target tile to the robot
		removeNeighbor(temp, 's');
		removeNeighbor(temp+sqrtOfTiles, 'n');
		removeNeighbor(temp, 'n');
		removeNeighbor(temp-sqrtOfTiles, 's');	

	}
	catch (std::out_of_range& e) {

	}
}

void Map::initialize() {
	tiles.at(4).setEnt(&pp);
	tiles.at(7).setEnt(&pp);
	makeWall(1, 0, 2, 0);
	makeWall(3, 0, 3, 1);
	makeWall(1, 1, 1, 2);
	makeWall(0, 3, 1, 3);
	makeWall(1, 2, 2, 2);
	makeWall(2, 3, 2, 4);
	makeWall(2, 1, 3, 1);
	makeWall(3, 2, 3, 3);
	makeWall(3, 3, 4, 3);

	for (int i = 0; i < totalTiles; i++)
		if (tiles.at(i).getNeighbors().size() >= 3)
			intersects[i] = true;

}

//will be returning vector after more details are worked out
vector<int> Map::look(int row, int col) { //returns vectors of tiles from N->E->S->W, divided by a NULL to represent new direction
	vector<int> scanning;
	int r = row, c = col, dest = 0;
	bool deadend = false;
	dest = coordToIndex(r, c, sqrtOfTiles);
	//north loop
	while (!deadend) {
		if (tiles.at(dest).getNorth() != NULL) {
			--r;
			dest = coordToIndex(r, c, sqrtOfTiles);
		}
		else
			deadend = true;
	}
	scanning.push_back(coordToIndex(row, c, sqrtOfTiles));
	scanning.push_back(dest);

//south loop
	deadend = false, r = row, c = col, dest = coordToIndex(row, col, sqrtOfTiles);//reset vars

	while (!deadend) {
		if (tiles.at(dest).getSouth() != NULL) {
			++r;
			dest = coordToIndex(r, c, sqrtOfTiles);
		}
		else
			deadend = true;
	}
	scanning.push_back(dest);


//east loop
	deadend = false, r = row, c = col, dest = coordToIndex(row, col, sqrtOfTiles);//reset vars

	while (!deadend) {
		if (tiles.at(dest).getEast() != NULL) {
			++c;
			dest = coordToIndex(r, c, sqrtOfTiles);
		}
		else
			deadend = true;
	}
	scanning.push_back(dest);

//WEST loop
	deadend = false, r = row, c = col, dest = coordToIndex(row, col, sqrtOfTiles);//reset vars

	while (!deadend) {
		if (tiles.at(dest).getWest() != NULL) {
			--c;
			dest = coordToIndex(r, c, sqrtOfTiles);
		}
		else
			deadend = true;
	}
	scanning.push_back(dest);


	/*cout << "Indices: ";
	for (int i = 0; i < scanning.size(); i++)
		cout <<scanning.at(i)<< ", ";
	cout << endl; */

	return scanning;
}
//connect each path from each direction
//places the found entities into the memory map
//deriving information from look()
void Map::look(Map &world, vector<int> scanned, bool (visited)[25]) {
	//NORTH LOOK
	int r1 = indexToRownum(scanned.at(0), world.getSqrtTiles());//robot position rownum
	int r2 = indexToRownum(scanned.at(1), world.getSqrtTiles());//destination of north rownum
	int c1 = indexToColnum(scanned.at(0), world.getSqrtTiles());//robot position colnum
	//cout << "r1= " << r1 << ", r2= " << r2 << ", c1= " << c1 << endl;
	//from r1 to r2, establish a valid path and identify any entities
	for (int i = 0; i <= r1 - r2; i++) {//e.g. from index 5 to index 2, subtract their rownums and count that many times
		int north = coordToIndex(r1 - i, c1, world.getSqrtTiles());//look at the tile above
		setEntAt(north, world.getEntAt(north));//see whats inside
		Tile insert = world.getTileAt(north);
		if (visited[north] == false) {//if the robot has not moved here, pretend it does not see the other sides
			insert.setEast(NULL);
			insert.setWest(NULL);
			visited[scanned.at(0)] = true;
		}
		setTileAt(north, insert);
	}

	//SOUTH LOOK
	r1 = indexToRownum(scanned.at(0), world.getSqrtTiles());
	r2 = indexToRownum(scanned.at(2), world.getSqrtTiles());//row of south destination
	c1 = indexToColnum(scanned.at(0), world.getSqrtTiles());
	for (int i = 0; i <= r2 - r1; i++) {
		int south = coordToIndex(r1 + i, c1, world.getSqrtTiles());//look at the tile above
		setEntAt(south, world.getEntAt(south));//see whats inside
		Tile insert = world.getTileAt(south);
		if (visited[south] == false) {//if the robot has not moved here, pretend it does not see the other sides
			insert.setEast(NULL);
			insert.setWest(NULL);
			visited[scanned.at(0)] = true;
		}
		setTileAt(south, insert);
	}

	//EAST LOOK
	r1 = indexToRownum(scanned.at(0), world.getSqrtTiles());
	c1 = indexToColnum(scanned.at(0), world.getSqrtTiles());
	int c2 = indexToColnum(scanned.at(3), world.getSqrtTiles());//colnum of destination east
	for (int i = 0; i <= c2 - c1; i++) {
		int east = coordToIndex(r1, c1 + i, world.getSqrtTiles());//look at the tile above
		setEntAt(east, world.getEntAt(east));//see whats inside
		Tile insert = world.getTileAt(east);
		if (visited[east] == false) {//if the robot has not moved here, pretend it does not see the other sides
			insert.setNorth(NULL);
			insert.setSouth(NULL);
			visited[scanned.at(0)] = true;
		}
		setTileAt(east, insert);
	}

	//WEST LOOK
	r1 = indexToRownum(scanned.at(0), world.getSqrtTiles());
	c1 = indexToColnum(scanned.at(0), world.getSqrtTiles());
	c2 = indexToColnum(scanned.at(4), world.getSqrtTiles());//colnum of destination west
	for (int i = 0; i <= c1 - c2; i++) {
		int west = coordToIndex(r1, c1 - i, world.getSqrtTiles());//look at the tile above
		setEntAt(west, world.getEntAt(west));//see whats inside
		Tile insert = world.getTileAt(west);
		if (visited[west] == false) {//if the robot has not moved here, pretend it does not see the other sides
			insert.setNorth(NULL);
			insert.setSouth(NULL);
			visited[scanned.at(0)] = true;
		}
		setTileAt(west, insert);
	}
}
	
bool Map::hasDied(int r_pac, int c_pac, int r_ghost, int c_ghost) {
	int pacPos = coordToIndex(r_pac, c_pac, sqrtOfTiles);
	if (r_pac == r_ghost) {
		//cout << "same row" << endl;
		if (c_pac - c_ghost == 1) {//if the ghost is to the left, check if there is wall on the left
			if (getTileAt(pacPos).getWest() != NULL) {
				return true;
			}
		}
		if (c_pac - c_ghost == -1) {
			if (getTileAt(pacPos).getEast() != NULL)
				return true;
		}
	}

	if (c_pac == c_ghost) {
		//cout << "same col" << endl;
		if (r_pac - r_ghost == 1) {//if the ghost is to the north
			if (getTileAt(pacPos).getNorth() != NULL)
				return true;
		}
		if (r_pac - r_ghost == -1) {
			if (getTileAt(pacPos).getSouth() != NULL)
				return true;
		}
	}
	return false;
}
