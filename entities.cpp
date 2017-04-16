#include "entities.h"
int Ghost::adjustDestination(char dir) {
	int g_dest = -1;
	//cout << get_rowpos() << endl << get_colpos();
	if (dir == 'n') {
		g_dest = coordToIndex(get_rowpos() - 1, get_colpos(), MAP_DIM);
		setBackwards('s');
	}
	if (dir == 's') {
		g_dest = coordToIndex(get_rowpos() + 1, get_colpos(), MAP_DIM);
		setBackwards('n');
	}
	if (dir == 'e') {
		g_dest = coordToIndex(get_rowpos(), get_colpos() + 1, MAP_DIM);
		setBackwards('w');
	}
	if (dir == 'w') {
		g_dest = coordToIndex(get_rowpos(), get_colpos() - 1, MAP_DIM);
		setBackwards('e');
	}
	//cout << g_dest;
	return g_dest;
}
char Roamer::rand_dir() {
	int rando;
	int prev=-1;
	int backwards=-1;
		//choose a random direction that is not the opposite of the way you are going
		if (get_dir() == 'n')
			prev = 0;
		else if (get_dir() == 'e')
			prev = 1;
		else if (get_dir() == 's')
			prev = 2;
		else if (get_dir() == 'w')
			prev = 3;
		else {
			prev = 3;
		}

		rando = rand() % 4;
		//if it its the backwards direction, change it
		backwards = (prev + 2) % 4;

		if (rando == backwards) 
			prev = 3 - prev;
		
		if (prev == 0) {
			set_dir('n');
			return 'n';
		}
		if (prev == 1) {
			set_dir('e');
			return 'e';
		}
		if (prev == 2) {
			set_dir('s');
			return 's';
		}
		if(prev==3){
			set_dir('w');
			return 'w';
		}
		return 'n';
}

int Roamer::advance(int current, char dir){
	int r=indexToRownum(current, MAP_DIM);
	int c=indexToColnum(current, MAP_DIM);
	if (dir == 'n')
		--r;
	if (dir == 's')
		++r;
	if (dir == 'e')
		++c;
	if (dir == 'w')
		--c;

	return coordToIndex(r, c, MAP_DIM);
	
}

void Ghost::move_scatter(int cornerpos) {
	;
}

void Ghost::move_chase(int pacpos) {
	;
}

//void Roamer::move() {	;}

//TODO:include no pass upwards tiles
int Ghost::move(int currentpos, int targetpos, bool intersects[22], bool corrected){//generic movement pattern, direction priority is up>left>down when heading towards target	
	//move one tile in given direction, if no direction is given, draw a line to the target, and choose the shortest x or y component
	int r_target = indexToRownum(targetpos, 5);
	int c_target = indexToColnum(targetpos, 5);
	int r_ghost = indexToRownum(currentpos, 5);
	int c_ghost = indexToColnum(currentpos, 5);

	if (intersects[currentpos] && !corrected) {
		if (abs(r_target - r_ghost) > abs(c_target - c_ghost)) {//if the horizontal distance is shorter, go east or west
			if (c_target > c_ghost) //if the target is to the right of the ghost
				set_dir('e');
			else
				set_dir('w');
		}
		else if (abs(r_target - r_ghost) < abs(c_target - c_ghost)) {
			if (r_target < r_ghost)//if the target is to the north of the ghost
				set_dir('n');
			else
				set_dir('s');
		}
		else if (abs(r_target - r_ghost) == abs(c_target - c_ghost))//incase equal distance
			set_dir('n');

		//cout << "inter " << get_dir();//DEBUG
	}


	return advance(currentpos, get_dir());

}

