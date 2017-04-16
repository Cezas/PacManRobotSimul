#include "graph.h"
//#include "conio.h"
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
int _getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77
#define KB_ESCAPE 27
#define KB_TAB 15
#define KB_SPACE 32


//todo: remake the grid, and implement a way to mirror one side of the map to the other

int main(){

//building the maze, initialization
	const int MAP_SIZE = 25;
	Map world(MAP_SIZE); //this is the actual map
	Map memory(MAP_SIZE); //TEMPORARY: robot's memory
	Map& toWorld = world;
	bool visited[MAP_SIZE];//an array to keep track which tiles have been visited

	for (int i=0;i<MAP_SIZE;i++)
		visited[i] = 0;

	world.meetNGreet();
	world.initialize();
	/*bool intersects[22];
	for (int i = 0; i < 22; i++)
		intersects[i] = false;
	intersects[6] = true,
	intersects[8] = true,
	intersects[11] = true,
	intersects[12] = true,
	intersects[13] = true,
	intersects[17] = true,
	intersects[21] = true,
	intersects[22] = true;*/
//testing the robot
	//variables for the controls
	//sets up the pacbot and the ghostbots locations

	//original position indices
	int pacPosOrig = 0,
		g1PosOrig = 22;

	//locations of the roamers
	int r_pac = indexToRownum(pacPosOrig, world.getSqrtTiles());
	int c_pac = indexToColnum(pacPosOrig, world.getSqrtTiles());
	//int r_g1 = indexToRownum(g1PosOrig, world.getSqrtTiles());
	//int c_g1 = indexToColnum(g1PosOrig, world.getSqrtTiles());
	int g1_pos = g1PosOrig;
	int g1_prevpos = g1PosOrig;

//robots being placed in the maps
	Pac pac;
	world.setEntAt(pacPosOrig, &pac);
	memory.setEntAt(pacPosOrig, &pac);
	visited[0] = true;

	//when a ghost is initialized, make sure to put it in the memory map as well
	Ghost g1;
	g1.setSymb('A');
	world.setGhostAt(g1PosOrig, &g1);
	memory.setGhostAt(g1PosOrig, &g1);

	Entity empty;
	int row = 0,
		col = 0,
		current = 0,
		origin = 0,
		KB_code = 0,
		pv = 0,
		total = 0,
		time = 0,
		lives = 3;
	bool gameover=false;

	//stuff for the kb space
	vector<char> otherpaths = {};
	char direction = g1.get_dir();
	char backwards = '$';//arbitrary value
	int g1_dest = g1_pos;
	bool corrected = false;//used to adjust move()
	int prev_dest = -1;
	//print out starting map
	memory.look(toWorld, world.look(row, col), visited);//scanned = world.look(row, col);
	world.printMap();
	memory.printMap();

	//controls

	while (KB_code != KB_ESCAPE && !gameover) {
		KB_code = _getch();//scans the keyboard input as an int
		if (KB_code == 224)
			KB_code = _getch();//need to call it twice or it wont work properly (im serious)
		printf("KB_code = %i \n", KB_code);

		origin = coordToIndex(row, col, world.getSqrtTiles());
		//processes the controls
		switch (KB_code)
		{

		case KB_SPACE://increment simulation time by 1, basically just tap and watch
		/* the actual pacman wave phases
		Scatter for 7 seconds, then Chase for 20 seconds.
		Scatter for 7 seconds, then Chase for 20 seconds.
		Scatter for 5 seconds, then Chase for 20 seconds.
		Scatter for 5 seconds, then switch to Chase mode permanently.
		 */
			g1_dest=g1.adjustDestination(direction);
			backwards = g1.getBackwards();
			/*if (direction == 'n') { //old code, just hide this
				g1_dest = coordToIndex(r_g1 - 1, c_g1, world.getSqrtTiles());
				backwards = 's';
			}
			 if (direction == 's') {
				g1_dest = coordToIndex(r_g1 + 1, c_g1, world.getSqrtTiles());
				backwards = 'n';
			}
			 if (direction == 'e') {
				g1_dest = coordToIndex(r_g1, c_g1 + 1, world.getSqrtTiles());
				backwards = 'w';
			}
			 if (direction == 'w') {
				g1_dest = coordToIndex(r_g1, c_g1 - 1, world.getSqrtTiles());
				backwards = 'e';
			}*/
			 //cout << direction << backwards << endl;//DEBUG
			 //cout << g1.get_dir(); //DEBUG
		
			  prev_dest= world.wallAhead(&g1, g1_dest);
			  //cout << g1_dest <<endl<< prev_dest;
			if (g1_dest != prev_dest)//if its been corrected
				corrected = true;
			g1_dest = prev_dest;
			direction = g1.get_dir();
			backwards = g1.getBackwards();
			//cout << direction<< endl; //DEBUG
			//cout << backwards<< endl;//DEBUG
			 //cout << "uncorrected "<<g1_dest<<", "<<direction<<endl;//DEBUG
			/*otherpaths=world.wallExists(g1_pos, g1_dest); //old code
			//if there is a wall infront, orient to another valid destination
			if (otherpaths.size() > 1) {
				g1.rand_dir();
				//for (int i = 0; i < otherpaths.size(); i++) //DEBUG
					//cout << otherpaths.at(i) << endl;
				for (int i = 0; i < otherpaths.size(); i++)
					if (otherpaths[i] != backwards) { //change its direction so its not hitting a wall or going backwards
						g1.set_dir(otherpaths[i]);
						direction = g1.get_dir();

						if (direction == 'n') 
							g1_dest = coordToIndex(r_g1 - 1, c_g1, world.getSqrtTiles());
													
						 if (direction == 's') 
							g1_dest = coordToIndex(r_g1 + 1, c_g1, world.getSqrtTiles());	
						
						if (direction =='e') 
							g1_dest = coordToIndex(r_g1, c_g1 + 1, world.getSqrtTiles());

						if (direction == 'w') 
							g1_dest = coordToIndex(r_g1, c_g1 - 1, world.getSqrtTiles());
						
						i = otherpaths.size();

						corrected = true;
						//cout << "corrected " << g1_dest <<","<<direction<<endl;//DEBUG
						//exit after the first instance
					}
				//cout << g1.get_dir() << endl; //DEBUG
				//cout << backwards << endl;//DEBUG
			}*/
			//make sure it never moves backwards, except for the special tiles (that may be implemented)
			
			g1_pos=world.moveGhost(&g1, &pac, corrected);
			//memory.moveGhost(&g1, &pac, corrected);
			direction = g1.get_dir();
			backwards = g1.getBackwards();
			//cout << direction<< endl; //DEBUG
			//cout << backwards<< endl;//DEBUG
		/*	if (intersects[g1_pos]) {//incase it moves backwards due to intersection, correct it
				int temppos = g1_pos;
				g1_pos = g1.move(g1_pos, pacPosOrig, intersects, corrected);
				if (g1.get_dir() == backwards) {

					if (backwards == 'n') {
						direction='s';
						g1.set_dir(direction);
						g1_pos = g1.move(temppos, pacPosOrig, intersects, true);
						g1_pos = g1.move(g1_pos, pacPosOrig, intersects, true);
					}

					if (backwards == 's') {
						direction = 'n';
						g1.set_dir(direction);
						g1_pos = g1.move(temppos, pacPosOrig, intersects, true);
						g1_pos = g1.move(g1_pos, pacPosOrig, intersects, true);
					}

					if (backwards == 'e') {
						direction = 'w';
						g1.set_dir(direction);
						g1_pos = g1.move(temppos, pacPosOrig, intersects, true);
						g1_pos = g1.move(g1_pos, pacPosOrig, intersects, true);
					}

					if (backwards == 'w') {
						direction = 'e';
						g1.set_dir(direction);
						g1_pos = g1.move(temppos, pacPosOrig, intersects, true);
						g1_pos = g1.move(g1_pos, pacPosOrig, intersects, true);
					}
				}
			}else
				g1_pos = g1.move(g1_pos, pacPosOrig, intersects, corrected);
			direction = g1.get_dir();
			g1.set_rowpos(indexToRownum(g1_pos, world.getSqrtTiles()));
			g1.set_colpos(indexToColnum(g1_pos, world.getSqrtTiles()));*/
			//update the map
			world.setGhostAt(g1_pos, &g1);
			world.removeGhostAt(g1_prevpos);
			memory.setGhostAt(g1_pos, &g1);
			memory.removeGhostAt(g1_prevpos);
			g1_prevpos = g1_pos;
			corrected = false;

			if (time == 0 || time == 27 || time == 54 || time == 79)
				cout << "scattering" << endl;
			else if (time == 7 || time == 34 || time == 59 || time == 84)
				cout << "chasing" << endl;


			break;

		//cases for the arrows are manual movements
		case KB_LEFT:
			--col;
			current = coordToIndex(row, col, world.getSqrtTiles());
			if (current == -1) {
				cout << "Out of bounds" << endl;
				++col;
				break;
			}

			//attempt a move, making sure to check if path exists in that direction
			if (world.getTileAt(current).getEast() != NULL) {
				//first get the point value from dest before moving
				pv = world.getEntAt(current)->getPntVal();
				world.setEntAt(current, &pac);
				//make the tile you just left marked as traveled
				world.setEntAt(origin, &empty);

				//the ghost location is known at all times
				//memory.setGhostAt()
				memory.setEntAt(current, &pac);
				memory.setEntAt(origin, &empty);
				//visited[origin] = true;
			}
			else ++col;
			break;

		case KB_RIGHT:
			++col;
			current = coordToIndex(row, col, world.getSqrtTiles());
			if (current == -1) {
				cout << "Out of bounds" << endl;
				--col;
				break;
			}

			//attempt a move, making sure to check if path exists in that direction
			if (world.getTileAt(current).getWest() != NULL) {
				//first get the point value from dest before moving
				pv = world.getEntAt(current)->getPntVal();
				world.setEntAt(current, &pac);
				//make the tile you just left marked as traveled
				world.setEntAt(origin, &empty);

				memory.setEntAt(current, &pac);
				memory.setEntAt(origin, &empty);
				//visited[current] = true;
			}
			else --col;
			break;

		case KB_UP:
			--row;
			current = coordToIndex(row, col, world.getSqrtTiles());
			if (current == -1) {
				cout << "Out of bounds" << endl;
				++row;
				break;
			}
			//attempt a move, making sure to check if path exists in that direction
			if (world.getTileAt(current).getSouth() != NULL) {
				//first get the point value from dest before moving
				pv = world.getEntAt(current)->getPntVal();
				world.setEntAt(current, &pac);
				//make the tile you just left marked as traveled
				world.setEntAt(origin, &empty);

				memory.setEntAt(current, &pac);
				memory.setEntAt(origin, &empty);
				//visited[current] = true;
			}
			else ++row;
			break;

		case KB_DOWN:
			++row;
			current = coordToIndex(row, col, world.getSqrtTiles());

			if (current == -1) {
				--row;
				cout << "Out of bounds" << endl;
				break;
			}
			//attempt a move, making sure to check if path exists in that direction
			if (world.getTileAt(current).getNorth() != NULL) {
				//first get the point value from dest before moving
				pv = world.getEntAt(current)->getPntVal();
				world.setEntAt(current, &pac);
				//make the tile you just left marked as traveled
				world.setEntAt(origin, &empty);

				memory.setEntAt(current, &pac);
				memory.setEntAt(origin, &empty);
				//visited[current] = true;
			}
			else --row;
			break;
		}//end of controls
	
		if (KB_code != 224) {
			total += pv;
			++time;
			//}

			r_pac = indexToRownum(current, world.getSqrtTiles());
			c_pac = indexToColnum(current, world.getSqrtTiles());

			cout << r_pac << ", " << c_pac << " : " << g1.get_rowpos() << ", " << g1.get_rowpos() << endl;
			if (world.hasDied(r_pac, c_pac, g1.get_rowpos(), g1.get_colpos())) {
				--lives;
				//remove all the ghost from its current position and places it back into the original
			}
		}
		if (lives == 0)
			gameover = true;

		//look and update the map
		memory.look(toWorld, world.look(row, col), visited);//scanned = world.look(row, col);
		cout << "Total Points: " << total << "	Time: " << time << "	Lives: "<< lives<<endl;
		memory.printMap();
		//world.printMap();
	}//end of game

	cout << "\a Total Points: " << total << "	Time: " << time << "	Lives: " << lives << endl;
	cout << "L O S E R " << endl;

	return 0;
}
