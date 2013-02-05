/*
 * Maze.cpp
 *
 *  Created on: Jan 31, 2013
 *      Author: mnlangs
 */

#include "Maze.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//fill up a new maze with solid rock
//TODO: maybe reorganize so we can reference as [x][y], laziness and stupidity caused this
Maze::Maze(int x,int y) {
	map = new int*[y];
	for(int i = 0; i < y; i++){
		map[i] = new int[x];
		for(int j = 0; j < x; j++){
			map[i][j] = MAZ_SOLID_ROCK;
		}
	}

	mazW = x;
	mazH = y;
	passed = 0;
	int test = 5; //adding to see if I can figure out git
}

void Maze::printMaze(){
	printf("    ");
	for(int i = 0; i < mazW; i++){
		printf("%d  ",i);
		if(i < 10) printf(" ");
	}
	printf("\n   ");
	for(int i = 0; i < mazW; i++){
		printf("----");
	}
	printf("\n");
	for(int i = 0; i < mazH; i++){
		if(i < 10) printf(" ");
		printf("%d| ",i);
		for(int j = 0; j < mazW; j++){
			char pc = '.'; //default to rock
			switch(map[i][j]){
			case MAZ_FLOOR:
				pc = 'x';
				break;
			case MAZ_PATH:
				pc = '#';
				break;
			}
			printf("%c   ",pc);
//			if(j >= 10) printf(" ");
		}
		printf("\n");
	}
	//show room coords
	for(int i = 0; i < rooms.size(); i++){
		printf("Room at (%d,%d)\n",rooms[i].first,rooms[i].second);
	}
}

//attempt to generate n rooms
void Maze::generateRoomTest(int n){
	srand(time(NULL));
	for(int i = 0; i < n; i++){
		int tx = ((rand() % mazW)+1);
		int ty = ((rand() % mazH)+1);
		int bx = (((rand() % (mazW))+1));
		int by = ((rand() % (mazH))+1);
		makeRegularRoom(tx,ty,bx,by);
	}
	printf("Completed %d generations\n",n);

}

//try to dig out a corridor from the wall at (x,y)
bool Maze::makeRegularHallway(std::pair<int,int> p){
	int x = p.first,y = p.second;
	dir wallDir = UNDEF;
	//determine which direction the wall faces,assuming its a wall
	if(!boundsCheck(x,y)){
		//not safe to make hallway here
		printf("Hall: Tried to make unsafe hallway\n");
		return false;
	}
	if((map[y-1][x] == MAZ_SOLID_ROCK) && (map[y+1][x] == MAZ_SOLID_ROCK)){
		//we're facing west or east
		if(map[y][x-1] == MAZ_FLOOR){
			//we're facing east
			wallDir = EAST;
		}
		else if(map[y][x+1] == MAZ_FLOOR){
			//we're facing west
			wallDir = WEST;
		}
	}
	else if(map[y][x-1] == MAZ_SOLID_ROCK && map[y][x+1] == MAZ_SOLID_ROCK){
		//we're facing north or south
		if(map[y+1][x] == MAZ_FLOOR){
			//we're facing north
			wallDir = NORTH;
		}
		else if(map[y-1][x] == MAZ_FLOOR){
			//we're facing south
			wallDir = SOUTH;
		}
	}
	if(wallDir == UNDEF){
		printf("Hall: Was not able to determine wall direction (Not a wall?)\n");
		return false;
	}
//	printf("Hall: direction is %d",wallDir);
	//generate a length for the hallway, for now lets keep them < 5 tiles
	srand(time(NULL));
	int length = (rand()%5)+1,i=0;
	bool boundsViolated = false;
	//dig out entrance to room
	map[y][x] = MAZ_PATH;
	while(i < length && !boundsViolated){
		switch(wallDir){
		case NORTH:
			y--; break;
		case SOUTH:
			y++; break;
		case EAST:
			x++; break;
		case WEST:
			x--; break;
		}
		//check that we're still within the bounds of the dungeon
		//TODO: replace with that function I wrote lol
		if(x < 0 || x > mazW || y < 0 || y > mazH){
			boundsViolated = true;
		}
		else{
			//N|S check left and right side, E|W check top and bottom
			switch(wallDir){
			case NORTH:
			case SOUTH:
				if(map[y][x-1] != MAZ_SOLID_ROCK || map[y][x+1] != MAZ_SOLID_ROCK)
					boundsViolated = true;
				break;
			case WEST:
			case EAST:
				if(map[y-1][x] != MAZ_SOLID_ROCK || map[y+1][x] != MAZ_SOLID_ROCK)
					boundsViolated = true;
				break;
			}
		}
		//if current tile is good, dig it out
		if(!boundsViolated) map[y][x] = MAZ_PATH;;

		i++;
	}
	printf("Dug a hall of length %d and true length %d\n",length,i);
}

bool Maze::boundsCheck(int x,int y){
	if(x <= 0 || y <= 0 || x >= mazW-1 || y >=mazH-1)
		return false;
	return true;
}

//dig a regular room in the dungeon, the 4 parameters specify
// the (x.y) of one corner of a rectangle and the (x,y) of another corner
// t means "top" and b "bottom" but there's no guranantee that that is the case
// returns true if it was able to do it, false otherwise
bool Maze::makeRegularRoom(int tx,int ty,int bx,int by){
	//do bounds checking
	//TODO: replace parts with that function I wrote 
	if( (abs(tx-bx) < 2) || (abs(ty-by) < 2) || (tx <= 0 || ty <= 0 || bx <= 0 || by <= 0)
			|| (tx >= mazW-1 || bx >= mazW-1)
			|| (ty >= mazH-1 || by >= mazH-1)
			|| (tx == bx) || (ty == by)
			//in addition to these normal bounds checks, since these are "regular" rooms
			//	we don't want any rooms greater than 1/4th the size of the map
			|| (abs(tx-bx) > (mazW/4)) || (abs(ty-by) > (mazW/4))
		)
	{
		return false;
	}
	//find leftmost,rightmost,topmost,bottommost, work with that from now on
	int lm = (tx < bx) ? tx : bx;
	int rm = (lm == tx) ? bx : tx;
	int tm = (ty < by) ? ty : by;
	int bm = (tm == ty) ? by : ty;
	//check if perimeter (wall) and room space is not already occupied
	for(int i = lm-1; i <= rm+1; i++){
		for(int j = tm-1; j <= bm+1; j++){
			if(map[j][i] != MAZ_SOLID_ROCK){
				return false;
			}
		}
	}
	printf("(%d,%d,%d,%d) passed bounds check\n",tx,ty,bx,by);
	passed++;
	//all should be good..dig out the room!
	for(int i = tm; i <= (tm+(bm-tm)); i++){
		for(int j = lm; j <= (lm+(rm-lm)); j++){
			map[i][j] = MAZ_FLOOR;
		}
	}
	//we've successfully made the room, store it
	//regular rooms are stored as the coord of their "top left" point
	rooms.push_back(std::make_pair(lm,tm)); //leftmost topmost should be "top left" yea?
	return true;
}

//returns the x,y coord of some random wall in a random room
//TODO: only works for regular rooms!
std::pair<int,int> Maze::getRandomWall(){
	pair<int,int> room = getRandomRoom();
	printf("RWall: Random room is (%d,%d)\n",room.first,room.second);
	int x = room.first;
	int y = room.second;
	srand(time(NULL));
	int wall = rand()%4;
	printf("RWall: Random wall is %d\n",wall);
	switch(wall){
	case NORTH:
		x++; //move one to the right and up so this is the coord of the north wall
		y--;
		break;
	case SOUTH:
		//move one to the right and down, go down until we hit solid rock
		x++;
		y--;
		while(map[y][x] != MAZ_SOLID_ROCK){
			y--;
		}
		break;
	case EAST:
		//move right until we hit solid rock, go down so we're on the east wall
		while(map[y][x] != MAZ_SOLID_ROCK){
			x++;
		}
		y++;
		break;
	case WEST:
		//we're already on the west wall!
		x--;
		y++;
		break;
	}
	return std::make_pair(x,y);
}

pair<int,int> Maze::getRandomRoom(){
	srand(time(NULL));
	int r = rand()%rooms.size();
	return rooms[r];
}

void Maze::generateBoringDungeon(){
	/*
	 * To make a boring dungeon:
	 *  Fill the whole map with solid earth (Maze constructor)
		Dig out a single room in the center of the map
		while dungeon is not complete: (arbitrary decision)
			Pick a wall of any room
			Decide upon a new feature to build
			if there is room to add the feature through the wall:
				add the feature
			else:
				continue
		add up/down staircases at random points
		sprinkle monsters and items liberally

		most of the bounds checking (if there is room etc..) is done in the respective methods,
		so just check for the return value of those methods to determine if it was successful
	 */
	//how big should it be? For now lets say a boring dungeon is 100x40 but this could (should) be
	// a randomly generated value in a range of acceptable x and y
	Maze m(100,40);
	//TODO: Finish when intermediate steps are completed, final goal of Dungeon class
}

Maze::~Maze() {
	for(int i = 0; i < mazH; i++)
		delete[] map[i];
	delete[] map;
}

int main(int argc,char** argv){
	int n = 1000;
	Maze m(40,25);
	m.generateRoomTest(n);
	m.makeRegularHallway(m.getRandomWall());
	m.printMaze();
}

