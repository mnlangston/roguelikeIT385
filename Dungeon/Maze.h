/*
 * Maze.h
 *
 *  Created on: Jan 31, 2013
 *      Author: mnlangs
 */

#ifndef MAZE_H_
#define MAZE_H_

#include <vector>
using namespace std;

class Maze {
private:
	enum {MAZ_SOLID_ROCK,MAZ_FLOOR,MAZ_PATH};
	enum dir {NORTH,SOUTH,EAST,WEST,UNDEF};
	int** map;
	int mazW,mazH;
	int passed;
	vector<pair<int,int> > rooms; //rooms are stored as the coord of their "top left" point
	//TODO: for now this only stores the x,y of the point, later it will store more (room type?)
public:
	inline int getPassed(){ return passed; }
	Maze(int,int);
	bool boundsCheck(int,int);
	void generateRoomTest(int);
	void generateBoringDungeon();
	pair<int,int> getRandomWall();
	pair<int,int> getRandomRoom();
	bool makeRegularRoom(int,int,int,int);
	bool makeRegularHallway(pair<int,int>);
	void printMaze();
	virtual ~Maze();
};

#endif /* MAZE_H_ */
