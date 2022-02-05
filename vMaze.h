/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Maze class
	Begun Monday, May 3rd, 2010

	The mazeSquare struct defines wall layouts for each square in the maze

	The vMaze class defines a maze and many maze-specific mechanics for the game. The maze class includes maze layout
	(a matrix of mazeSquare objects), maze generation methods, actor instances (ghosts and pacman), items, etc.
*/

#ifndef VENGEANCE_MAZE_H
#define VENGEANCE_MAZE_H

#include "libArtemisExt.h"
#include "Dice.h"
#include "vItem.h"
#include "vActor.h"
#include "vSprite.h"
#include <time.h>

// Several algorithms are available for maze generation; division is default, biased towards long corridors
enum MazeAlg { MA_DIVISION, MA_PRIM, MA_KRUSKAL, MA_BACKTRACK };

// Basic direction enumerations, up left down right
enum MazeDirection { MD_NONE, MD_UP, MD_LEFT, MD_DOWN, MD_RIGHT };

struct mazeSquare {
	// 'true' means a wall exists in that direction
	bool wallUp;
	bool wallDown;
	bool wallLeft;
	bool wallRight;

	// These flags are used for recursive iteration when exploring the maze
	bool visited;
	bool accessible;

	// Methods
	mazeSquare();
	void reset(bool empty=true);
};

class vMaze {
private:
	// Data
	float minW, minH;
	float maxW, maxH;
	float levelScaleSpeed;
	float squareDim;	// Dimension of one maze square, in pixels
	float fruitDensity;	// Chance of a given square being fruit
	bool * soundFlags;	// Array of sound effect flags
	bool isPaused;		// Will the maze be updated, and how will it be drawn?
	int numEffects;		// Number of sound effects (length of playSnds array)
	int numW, numH;
	int dx, dy;
	int level;
	int levelPoints, totalPoints;
	double vulnerabilityDuration;	// Length in seconds of vulnerability after big dots are eaten
	time_t lastVulnerability;		// Unix time of the last vulnerable period, for countdown

	// Objects
	aTexture * textures;
	Dice * die;
	mazeSquare * squares;
	vItem * items;
	vSprite * wallSegment;

	// Maze Creation (private: no or dangerous use externally)
	void applyAi(vActor * actor);
	void breakIsolation(int x=-1, int y=-1);		// Ensure all cells are connected to the center
	void buildGhostTown();
	void divisionStep(int l, int r, int b, int t);
	void fillSpaces();
	void generate(MazeAlg algorithm);
	void refreshAccessibility(int x=-1, int y=-1);	// Set 'accessible' flag for each square, from center outwards
	void resetAccessibility();
	void resetSquares(bool empty=true);
	void resetVisited();
	void resize(int w, int h, aGraphics * context);
	void setVertWall(int v);
	void setVertWall(int x, int y, bool s=true); 	// x is wall location, y is square location
	void setHorizWall(int h);
	void setHorizWall(int x, int y, bool s=true); 	// x is square location, y is wall location
protected:
public:
	// Actors: public because they are stricly not maze elements, but kept in this namespace to avoid global pollution
	vActor * pacman;
	vActor * blinky;
	vActor * pinky;
	vActor * inky;
	vActor * clyde;

	// Constructors
	vMaze();
	~vMaze();

	// Accessors
	bool getIsPaused();
	bool getSoundFlag(int n);
	int getCurrentPointsTotal();
	int getLevel();
	int getNumH();
	int getNumW();
	int getTotalPoints();
	aTexture * getTextures();
	mazeSquare * getSquare(int x, int y);
	vActor * getActorByType(spriteType actorType);
	vActor * getSelection();
	vItem * getItem(int x, int y);
	void setSoundFlag(int n, bool s);
	void pause();
	void unpause();

	// Methods
	bool checkAccessibility();
	bool executeAbility(vActor * subject);
	void aStarPlot(int * values, int x, int y); // Plots the distance from x,y to each point in the maze
	void drawWallSegment(int k, int x, int y, aGraphics * context);
	void moveToMazeXY(vActor * actor, int x, int y);
	void newLevel(aGraphics * context, bool reset=false);
	void renderMaze(aGraphics * context);
	void rotateSelection();
	void turnActor(vActor * actor, MazeDirection direction);
	void update(float dt);

	// Coordinate transformations
	int mazeX2screenX(int x);
	int mazeY2screenY(int y);
	int screenX2mazeX(int x);
	int screenY2mazeY(int y);
};

#endif