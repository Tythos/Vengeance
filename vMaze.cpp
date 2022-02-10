/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Maze class
	Begun Monday, May 3rd, 2010

	The mazeSquare struct defines wall layouts for each square in the maze

	The vMaze class defines a maze and many maze-specific mechanics for the game. The maze class includes maze layout
	(a matrix of mazeSquare objects), maze generation methods, actor instances (ghosts and pacman), items, etc.
*/

#include "vMaze.h"
#include "Dice.h"
#include <math.h>
#include <time.h>

// --- mazeSquare --- //

mazeSquare::mazeSquare() {
	wallUp = true;
	wallDown = true;
	wallLeft = true;
	wallRight = true;
	visited = false;
	accessible = false;
}

void mazeSquare::reset(bool empty) {
	wallUp = !empty;
	wallDown = !empty;
	wallLeft = !empty;
	wallRight = !empty;
	visited = false;
	accessible = false;
}

// --- vMaze --- //

// --- Constructors --- //

vMaze::vMaze() {
	// Set basic parameters
	numW = 0;
	numH = 0;
	dx = 0;
	dy = 0;
	level = 0;
	levelPoints = 0;
	vulnerabilityDuration = 3.0;
	minW = 5.0f;
	minH = 7.0f;
	maxW = 13.0f;
	maxH = 17.0f;
	fruitDensity = 1.0f / 10.0f;
	levelScaleSpeed = 0.05f;
	squareDim = 40.0f;

	// Load maze textures
	textures = new aTexture();
	textures->loadFromFile("..\\resources\\textures.png");

	// Initialize objects
	squares = NULL;
	die = new Dice();
	items = NULL;

	// Initialize Pacman sprite
	pacman = new vActor();
	pacman->setTexture(textures);
	pacman->setType(V_PACMAN);
	pacman->setMode(AI_GREEDY);

	// Initialize ghost sprites
	blinky = new vActor();
	blinky->setTexture(textures);
	blinky->setType(V_RED_G);
	blinky->setState(SS_UP2);

	pinky = new vActor();
	pinky->setTexture(textures);
	pinky->setType(V_PINK_G);
	pinky->setState(SS_LEFT2);

	inky = new vActor();
	inky->setTexture(textures);
	inky->setType(V_BLUE_G);
	inky->setState(SS_DOWN2);

	clyde = new vActor();
	clyde->setTexture(textures);
	clyde->setType(V_ORANGE_G);
	clyde->setState(SS_RIGHT2);

	// Set ghost locations
	moveToMazeXY(blinky, numW / 2, numH / 2 - 1);
	moveToMazeXY(pinky, numW / 2 - 1, numH / 2);
	moveToMazeXY(inky, numW / 2 + 1, numH / 2);
	moveToMazeXY(clyde, numW / 2, numH / 2);

	// Load wall sprite
	wallSegment = new vSprite(textures);
	wallSegment->setType(V_WALLS);
	wallSegment->setW(squareDim);
	wallSegment->setH(squareDim);
	wallSegment->isTranslucent = true;

	// Initialize states to false
	numEffects = 8;
	soundFlags = new bool[numEffects];
	for (int i = 0; i < numEffects; i++) { soundFlags[i] = false; }
	isPaused = false;
}

vMaze::~vMaze() {
	// Lots of stuff to destroy! (if not null)
	if (squares != NULL) {
		delete[] squares;
		squares = NULL;
	}
	if (die != NULL) {
		delete die;
		die = NULL;
	}
	if (items != NULL) {
		delete[] items;
		items = NULL;
	}
	if (textures != NULL) {
		delete textures;
		textures = NULL;
	}
	if (wallSegment != NULL) {
		delete wallSegment;
		wallSegment = NULL;
	}
	if (soundFlags != NULL) {
		delete[] soundFlags;
		soundFlags = NULL;
	}
}

// --- Private Methods --- //

void vMaze::applyAi(vActor * actor) {
	// Apply AI to determine actor's actions
	// Make sure border walls are set
	setVertWall(0);
	setVertWall(numW);
	setHorizWall(0);
	setHorizWall(numH);

	// enum AiObjective { AI_NONE, AI_AVOID, AI_HOMICIDAL, AI_GREEDY, AI_RANDOM };
	int cx = screenX2mazeX((int)(actor->getX().value));
	int cy = screenY2mazeY((int)(actor->getY().value));
	int destX = numW / 2;
	int destY = numH / 2;
	int distanceToItem = numW + numH;
	vItem * toSeek = NULL;
	bool isGhost = !(actor == pacman);

	// If we're still moving into a square, we don't need to recalculate path yet
	if (actor->getVelY() > 0) {
		if (mazeY2screenY(cy) > actor->getY().value) return;
	} else if (actor->getVelX() < 0) {
		if (mazeX2screenX(cx) < actor->getX().value) return;
	} else if (actor->getVelY() < 0) {
		if (mazeY2screenY(cy) < actor->getY().value) return;
	} else if (actor->getVelX() > 0) {
		if (mazeX2screenX(cx) > actor->getX().value) return;
	}

	// AI mode determines path selection
	switch (actor->getMode()) {
		case AI_NONE:
			// Set waypoint to current cell location
			actor->setWaypoint(cx, cy);
			break;
		case AI_AVOID:
			// If waypoint reached, new waypoint away from nearest enemy actor
			if (cx == actor->getWayX() && cy == actor->getWayY()) {
				int destX = numW / 2;
				int destY = numH / 2;
				if (isGhost) {
					int pacX = screenX2mazeX((int)(pacman->getX().value));
					int pacY = screenY2mazeY((int)(pacman->getY().value));
					destX = 2 * cx - pacX;
					destY = 2 * cy - pacY;
					if (destX < 0) destX = 0;
					if (destX >= numW) destX = numW-1;
					if (destY < 0) destY = 0;
					if (destY >= numH) destY = numH-1;
					actor->setWaypoint(destX, destY);
				} else {
					int distToBlinky = (screenX2mazeX((int)(blinky->getX().value)) - cx) * (screenX2mazeX((int)(blinky->getX().value)) - cx) + (screenY2mazeY((int)(blinky->getY().value)) - cy) * (screenY2mazeY((int)(blinky->getY().value)) - cy);
					int distToPinky = (screenX2mazeX((int)(pinky->getX().value)) - cx) * (screenX2mazeX((int)(pinky->getX().value)) - cx) + (screenY2mazeY((int)(pinky->getY().value)) - cy) * (screenY2mazeY((int)(pinky->getY().value)) - cy);
					int distToInky = (screenX2mazeX((int)(inky->getX().value)) - cx) * (screenX2mazeX((int)(inky->getX().value)) - cx) + (screenY2mazeY((int)(inky->getY().value)) - cy) * (screenY2mazeY((int)(inky->getY().value)) - cy);
					int distToClyde = (screenX2mazeX((int)(clyde->getX().value)) - cx) * (screenX2mazeX((int)(clyde->getX().value)) - cx) + (screenY2mazeY((int)(clyde->getY().value)) - cy) * (screenY2mazeY((int)(clyde->getY().value)) - cy);
					vActor * fleeFrom = NULL;
					if (distToBlinky <= distToPinky && distToBlinky <= distToInky && distToBlinky <= distToClyde) {
						fleeFrom = blinky;
					} else if (distToPinky <= distToInky && distToPinky <= distToClyde) {
						fleeFrom = pinky;
					} else if (distToInky <= distToClyde) {
						fleeFrom = inky;
					} else {
						fleeFrom = clyde;
					}
					int fleeX = screenX2mazeX((int)(fleeFrom->getX().value));
					int fleeY = screenY2mazeY((int)(fleeFrom->getY().value));
					destX = 2 * cx - fleeX;
					destY = 2 * cy - fleeY;
				}
				if (destX < 0) destX = 0;
				if (destX >= numW) destX = numW-1;
				if (destY < 0) destY = 0;
				if (destY >= numH) destY = numH-1;
				actor->setWaypoint(destX, destY);
			}
			break;
		case AI_HOMICIDAL:
			// Target nearest, um, target!
			if (isGhost) {
				destX = screenX2mazeX((int)(pacman->getX().value));
				destY = screenY2mazeY((int)(pacman->getY().value));
			} else {
				int distToBlinky = (screenX2mazeX((int)(blinky->getX().value)) - cx) * (screenX2mazeX((int)(blinky->getX().value)) - cx) + (screenY2mazeY((int)(blinky->getY().value)) - cy) * (screenY2mazeY((int)(blinky->getY().value)) - cy);
				int distToPinky = (screenX2mazeX((int)(pinky->getX().value)) - cx) * (screenX2mazeX((int)(pinky->getX().value)) - cx) + (screenY2mazeY((int)(pinky->getY().value)) - cy) * (screenY2mazeY((int)(pinky->getY().value)) - cy);
				int distToInky = (screenX2mazeX((int)(inky->getX().value)) - cx) * (screenX2mazeX((int)(inky->getX().value)) - cx) + (screenY2mazeY((int)(inky->getY().value)) - cy) * (screenY2mazeY((int)(inky->getY().value)) - cy);
				int distToClyde = (screenX2mazeX((int)(clyde->getX().value)) - cx) * (screenX2mazeX((int)(clyde->getX().value)) - cx) + (screenY2mazeY((int)(clyde->getY().value)) - cy) * (screenY2mazeY((int)(clyde->getY().value)) - cy);
				if (!blinky->getIsAlive()) distToBlinky = numH * numW;
				if (!pinky->getIsAlive()) distToPinky = numH * numW;
				if (!inky->getIsAlive()) distToInky = numH * numW;
				if (!clyde->getIsAlive()) distToClyde = numH * numW;
				vActor * seekTo = NULL;
				if (distToBlinky <= distToPinky && distToBlinky <= distToInky && distToBlinky <= distToClyde) {
					seekTo = blinky;
				} else if (distToPinky <= distToInky && distToPinky <= distToClyde) {
					seekTo = pinky;
				} else if (distToInky <= distToClyde) {
					seekTo = inky;
				} else {
					seekTo = clyde;
				}
				destX = screenX2mazeX((int)(seekTo->getX().value));
				destY = screenY2mazeY((int)(seekTo->getY().value));
			}
			if (destX < 0) destX = 0;
			if (destX >= numW) destX = numW-1;
			if (destY < 0) destY = 0;
			if (destY >= numH) destY = numH-1;
			actor->setWaypoint(destX, destY);
			break;
		case AI_GREEDY:
			// Seek nearest item
			if ((cx == actor->getWayX() && cy == actor->getWayY()) || actor->getWayX() == -1 || actor->getWayY() == -1) {
				for (int i = 0; i < numW; i++) {
					for (int j = 0; j < numH; j++) {
						int distanceToHere = (cx - i) * (cx - i) + (cy - j) * (cy - j);
						if (!items[i * numH + j].getIsConsumed() && distanceToHere < distanceToItem) {
							distanceToItem = distanceToHere;
							toSeek = &items[i * numH + j];
						}
					}
				}
				if (toSeek != NULL) {
					actor->setWaypoint(screenX2mazeX((int)(toSeek->getX().value)), screenY2mazeY((int)(toSeek->getY().value)));
				} else {
					actor->setWaypoint(0, 0);
				}
			}
			break;
		case AI_RANDOM:
		default:
			// Choose a random square, if we've already reached the current one
			if ((cx == actor->getWayX() && cy == actor->getWayY()) || actor->getWayX() == -1 || actor->getWayY() == -1) {
				Dice die = Dice();
				int x = die.rollInt(numW);
				int y = die.rollInt(numH);
				actor->setWaypoint(x, y);
			}
			break;
	}

	// We have a waypoint; determine a pathway and turn actor to reach first step
	// Use partial A*: plot distance from destination point to current point recursively
	int currX = actor->getWayX(); int currY = actor->getWayY();
	int * stepsToDest = new int[numW * numH];
	for (int i = 0; i < numW * numH; i++) {
		stepsToDest[i] = -1;
	}
	stepsToDest[currX * numH + currY] = 0;
	aStarPlot(stepsToDest, currX, currY);

	// Check path distances to each side
	int up = cy < numH-1 ? stepsToDest[cx * numH + cy+1] : numW & numH;
	int left = cx > 0 ? stepsToDest[(cx-1) * numH + cy] : numW * numH;
	int down = cy > 0 ? stepsToDest[cx * numH + cy-1] : numW * numH;
	int right = cx < numW-1 ? stepsToDest[(cx+1) * numH + cy] : numW * numH;

	// Enforce bounds
	up = up == -1 ? numW * numH : up;
	left = left == -1 ? numW * numH : left;
	down = down == -1 ? numW * numH : down;
	right = right == -1 ? numW * numH : right;
	mazeSquare * currSquare = getSquare(cx, cy);
	if (up < 0 || up > numW * numH || currSquare->wallUp) up = numW * numH;;
	if (left < 0 || left > numW * numH || currSquare->wallLeft) left = numW * numH;;
	if (down < 0 || down > numW * numH || currSquare->wallDown) down = numW * numH;;
	if (right < 0 || right > numW * numH || currSquare->wallRight) right = numW * numH;;

	// Enact movement
	if (cx == currX && cy == currY) {
		// Continue moving through this square
	} else if (up <= left && up <= down && up <= right && !currSquare->wallUp && up >= 0) {
		turnActor(actor, MD_UP);
	} else if (left <= down && left <= right && !currSquare->wallLeft && left >= 0) {
		turnActor(actor, MD_LEFT);
	} else if (down <= right && !currSquare->wallDown && down >= 0) {
		turnActor(actor, MD_DOWN);
	} else if (!currSquare->wallRight && right >= 0) {
		turnActor(actor, MD_RIGHT);
	} else {
	}
	delete[] stepsToDest;
}

void vMaze::breakIsolation(int x, int y) {
	// Ensure all cells are connected to the center
	mazeSquare * current = NULL;
	int centerX = numW / 2;
	int centerY = numH / 2;
	if (x == -1 && y == -1) {
		// At beginning, which by definition is not isloated. Check each cell in the maze.
		for (int i = 0; i < numW; i++) {
			for (int j = 0; j < numH; j++) {
				current = getSquare(i, j);
				if (!current->accessible) {
					current->visited = true;
					breakIsolation(i, j);
				}
			}
		}
	} else if (getSquare(x, y)->accessible) {
		// Already accessible; ignore
	} else {
		current = getSquare(x, y);
		current->visited = true;
		bool tried0 = y >= numH-1 || (y == centerY-1 && x >= centerX-1 && x <= centerX+1) || getSquare(x, y+1)->visited;
		bool tried1 = x <= 0 || (x == centerX+2 && y == centerY) || getSquare(x-1, y)->visited;
		bool tried2 = y <= 0 || (y == centerY+1 && x >= centerX-1 && x <= centerX+1) || getSquare(x, y-1)->visited;
		bool tried3 = x >= numW-1 || (x == centerX-2 && y == centerY) || getSquare(x+1, y)->visited;
		while (!current->accessible || !(tried0 && tried1 && tried2 && tried3)) {
			// Try breaking through to a random neighbor
			int breakDirection = die->rollInt(4);
			switch (breakDirection) {
				case 0:
					// Break up, if not at top or already visited, or below ghost town
					if (!tried0) {
						setHorizWall(x, y+1, false);
						refreshAccessibility();
						if (!current->accessible) {
							breakIsolation(x, y+1);
						}
						tried0 = true;
					}
					break;
				case 1:
					// Break left, if not at edge or already broken, or right of ghost town
					if (!tried1) {
						setVertWall(x, y, false);
						refreshAccessibility();
						if (!current->accessible) {
							breakIsolation(x-1, y);
						}
						tried1 = true;
					}
					break;
				case 2:
					// Break down, if not at bottom or already broken, or above ghost town
					if (!tried2) {
						setHorizWall(x, y, false);
						refreshAccessibility();
						if (!current->accessible) {
							breakIsolation(x, y-1);
						}
						tried2 = true;
					}
					break;
				case 3:
				default:
					// Break right, if not at edge or already broken, or left of ghost town
					if (!tried3) {
						setVertWall(x+1, y, false);
						refreshAccessibility();
						if (!current->accessible) {
							breakIsolation(x+1, y);
						}
						tried3 = true;
					}
					break;
			}
		}
	}
	current = NULL;
}

void vMaze::buildGhostTown() {
	// Build ghost town in middle of map
	int centerX = numW / 2;
	int centerY = numH / 2;

	// Center square
	setHorizWall(centerX, centerY, false);
	setHorizWall(centerX, centerY+1);
	setVertWall(centerX, centerY, false);
	setVertWall(centerX+1, centerY, false);

	// Left square
	setHorizWall(centerX-1, centerY);
	setHorizWall(centerX-1, centerY+1);
	setVertWall(centerX-1, centerY);

	// Right square
	setHorizWall(centerX+1, centerY);
	setHorizWall(centerX+1, centerY+1);
	setVertWall(centerX+2, centerY);

	// Bottom square
	setHorizWall(centerX, centerY-1, false);
	setVertWall(centerX, centerY-1, false);
	setVertWall(centerX+1, centerY-1, false);

	// These squares are accessible, by definition
	mazeSquare * current = getSquare(centerX, centerY);
	current->accessible = true;
	current = getSquare(centerX-1, centerY);
	current->accessible = true;
	current = getSquare(centerX+1, centerY);
	current->accessible = true;
	current = getSquare(centerX, centerY-1);
	current->accessible = true;
}

void vMaze::divisionStep(int l, int r, int b, int t) {
	int i = 0;
	if ((l - r) * (l - r) <= 1 || (b - t) * (b - t) <= 1) {
		// Do not continue if we are looking at a sufficiently small / thin area
		return;
	}

	// Set up vertical wall, within bounds
	int vWall = die->rollIntRange(l+1, r-1);
	for (i = b; i <= t; i++) {
		setVertWall(vWall, i);
	}

	// Set up horizontal wall, within bounds
	int hWall = die->rollIntRange(b+1, t-1);
	for (i = l; i <= r; i++) {
		setHorizWall(i, hWall);
	}

	// Create gaps (1-2 each division, v then h)
	int vertGaps = die->rollIntRange(2, 3);
	int gap = 0;
	for (i = 0; i < vertGaps; i++) {
		gap = die->rollIntRange(b, t);
		setVertWall(vWall, gap, false);
	}
	int horizGaps = die->rollIntRange(2, 3);
	for (i = 0; i < horizGaps; i++) {
		gap = die->rollIntRange(l, r);
		setVertWall(hWall, gap, false);
	}

	// Iterate into four subdivisions
	divisionStep(l, vWall, b, hWall);
	divisionStep(vWall, r, b, hWall);
	divisionStep(l, vWall, hWall, t);
	divisionStep(vWall, r, hWall, t);
}

void vMaze::fillSpaces() {
	// Checks for empty spaces (intersections of all non-walls) and fills them with one wall
	mazeSquare * q1 = NULL;
	mazeSquare * q3 = NULL;
	for (int i = 1; i < numW; i++) {
		for (int j = 1; j < numH; j++) {
			q1 = getSquare(i, j);
			q3 = getSquare(i-1, j-1);
			if (!q1->wallLeft && !q1->wallDown && !q3->wallUp && !q3->wallRight) {
				int wallToFill = die->rollInt(4);
				switch (wallToFill) {
					case 0:
						// Fill upper wall
						setVertWall(i, j);
						break;
					case 1:
						// Fill left wall
						setHorizWall(i-1, j);
						break;
					case 2:
						// Fill bottom wall
						setVertWall(i, j-1);
						break;
					case 3:
					default:
						// Fill right wall
						setHorizWall(i, j);
						break;
				}
			}
		}
	}
}

void vMaze::generate(MazeAlg algorithm) {
	resetSquares();
	switch (algorithm) {
		case MA_DIVISION:
			divisionStep(0, numW, 0, numH);
			buildGhostTown();
			refreshAccessibility();
			breakIsolation();
			fillSpaces();
			break;
		default:
			break;
	}
}

void vMaze::refreshAccessibility(int x, int y) {
	if (x == -1 && y == -1) {
		// Start at the beginning (center), then move down one to entrance
		x = numW / 2;
		y = numH / 2;
		y--;
		resetAccessibility(); // Sets all squares except ghost town to inaccessible
		resetVisited();
		getSquare(x, y)->accessible = true;
	}
	mazeSquare * current = getSquare(x, y);

	// Check up; if not blocked, ensure accessibility or set and recurse
	if (y < numH - 1 && !current->wallUp && !getSquare(x, y+1)->accessible) {
		getSquare(x, y+1)->accessible = true;
		refreshAccessibility(x, y+1);
	}

	// Check down; if not blocked, ensure accessibility or set and recurse
	if (y > 0 && !current->wallDown && !getSquare(x, y-1)->accessible) {
		getSquare(x, y-1)->accessible = true;
		refreshAccessibility(x, y-1);
	}

	// Check left; if not blocked, ensure accessibility or set and recurse
	if (x > 0 && !current->wallLeft && !getSquare(x-1, y)->accessible) {
		getSquare(x-1, y)->accessible = true;
		refreshAccessibility(x-1, y);
	}

	// Check right; if not blocked, ensure accessibility or set and recurse
	if (x < numW - 1 && !current->wallRight && !getSquare(x+1, y)->accessible) {
		getSquare(x+1, y)->accessible = true;
		refreshAccessibility(x+1, y);
	}
	current = NULL;
}

void vMaze::resetAccessibility() {
	mazeSquare * current = NULL;
	int centerX = numW / 2;
	int centerY = numH / 2;
	for (int i = 0; i < numW; i++) {
		for (int j = 0; j < numH; j++) {
			current = getSquare(i, j);
			if ((i == centerX && j == centerY) || (i == centerX && j == centerY-1) || (i == centerX-1 && j == centerY) || (i == centerX+1 && j == centerY)) {
				current->accessible = true;
			} else {
				current->accessible = false;
			}
		}
	}
	current = NULL;
}

void vMaze::resetSquares(bool empty) {
	mazeSquare * current = NULL;
	for (int i = 0; i < numW; i++) {
		for (int j = 0; j < numH; j++) {
			current = getSquare(i, j);
			current->reset(empty);
		}
	}

	if (empty) {
		// Borders should still be solid
		setVertWall(0);
		setVertWall(numW);
		setHorizWall(0);
		setHorizWall(numH);
	}
	current = NULL;
}

void vMaze::resetVisited() {
	mazeSquare * current = NULL;
	for (int i = 0; i < numW; i++) {
		for (int j = 0; j < numH; j++) {
			current = getSquare(i, j);
			current->visited = false;
		}
	}
	current = NULL;
}

void vMaze::resize(int w, int h, aGraphics * context) {
	if (squares != NULL) {
		delete[] squares;
		squares = NULL;
	}
	if (numW % 2 != 0) numW++;
	if (numH % 2 != 0) numH++;
	numW = w > 0 ? w : 1;
	numH = h > 0 ? h : 1;
	squares = new mazeSquare[numW*numH];

	// Calculate coordinate offset
	dx = (int)(context->getWidth() / 2 - (numW * squareDim) / 2);
	dy = (int)(context->getHeight() / 2 - (numH * squareDim) / 2);

	// Initialize items
	if (items != NULL) {
		delete[] items;
		items = NULL;
	}
	items = new vItem[numW*numH];
	int centerX = (int)(numW / 2);
	int centerY = (int)(numH / 2);
	for (int i = 0; i < numW; i++) {
		for (int j = 0; j < numH; j++) {
			// Items default to small dots
			items[i * numH + j].specifyTexture(textures);
			if ((centerX - i) * (centerX - i) <= 1 && j == centerY) {
				// Disable ghost town squares
				items[i * numH + j].disable();
			} else if ((i == 0 && j == 0) || (i == 0 && j == centerY) || (i == 0 && j == numH-1) || (i == centerX && j == 0) || (i == centerX && j == numH-1) || (i == numW-1 && j == 0) || (i == numW-1 && j == centerY) || (i == numW-1 && j == numH-1)) {
				// Assign large dots to symmetrical locations
				items[i * numH + j].setItemType(IT_LARGE_DOT);
			} else {
				// Randomly assign others based on level (point value)
				float roll = die->rollFloat(1.0f);
				if (roll <= fruitDensity) {
					int itemNum = 4 - (int)(1.0f / (levelScaleSpeed * level + 0.25f));
					int randOffset = die->rollInt(6);
					if (randOffset == 5) { itemNum += 2; }
					else if (randOffset >= 3) { itemNum += 1; }
					items[i * numH + j].setItemType((itemType)(itemNum + 2));
				}
			}
		}
	}
	levelPoints = getCurrentPointsTotal();
}

void vMaze::setVertWall(int v) {
	mazeSquare * current = NULL;
	if (v == 0) {
		for (int i = 0; i < numH; i++) {
			current = getSquare(v, i);
			current->wallLeft = true;
		}
	} else if (v == numW) {
		for (int i = 0; i < numH; i++) {
			current = getSquare(v-1, i);
			current->wallRight = true;
		}
	} else if (v < 0 || v > numW) {
		return;
	} else {
		for (int i = 0; i < numH; i++) {
			current = getSquare(v-1, i);
			current->wallRight = true;
			current = getSquare(v, i);
			current->wallLeft = true;
		}
	}
	current = NULL;
}

void vMaze::setVertWall(int x, int y, bool s) {
	// x is wall location, y is square location:
	//    -------
	// y |   |   |
	//    -------
	//       x
	mazeSquare * left = getSquare(x-1, y);
	if (left != NULL) {
		left->wallRight = s;
	}
	mazeSquare * right = getSquare(x, y);
	if (right != NULL) {
		right->wallLeft = s;
	}
	left = NULL; right = NULL;
}

void vMaze::setHorizWall(int h) {
	mazeSquare * current = NULL;
	if (h == 0) {
		for (int i = 0; i < numW; i++) {
			current = getSquare(i, h);
			current->wallDown = true;
		}
	} else if (h == numH) {
		for (int i = 0; i < numW; i++) {
			current = getSquare(i, h-1);
			current->wallUp = true;
		}
	} else if (h < 0 || h > numH) {
		return;
	} else {
		for (int i = 0; i < numW; i++) {
			current = getSquare(i, h-1);
			current->wallUp = true;
			current = getSquare(i, h);
			current->wallDown = true;
		}
	}
	current = NULL;
}

void vMaze::setHorizWall(int x, int y, bool s) {
	// x is square location, y is wall location:
	//    ---
	//   |   |
	// y |---|
	//   |   |
	//    ---
	//     x
	if (numW > 100) {
		bool a = true;
	}
	mazeSquare * down = getSquare(x, y-1);
	if (down != NULL) {
		down->wallUp = s;
	}
	mazeSquare * up = getSquare(x, y);
	if (up != NULL) {
		up->wallDown = s;
	}
	down = NULL; up = NULL;
}

// --- Accessors --- //

bool vMaze::getIsPaused() {
	return isPaused;
}

bool vMaze::getSoundFlag(int n) {
	if (n < 0 || n >= numEffects) n = 0;
	return soundFlags[n];
}

int vMaze::getCurrentPointsTotal() {
	// Returns the total point value of all unconsumed items in this level
	int toReturn = 0;
	for (int i = 0; i < numW; i++) {
		for (int j = 0; j < numH; j++) {
			if (!items[i * numH + j].getIsConsumed()) {
				toReturn += items[i * numH + j].getPointValue();
			}
		}
	}
	return toReturn;
}

int vMaze::getLevel() {
	return level;
}

int vMaze::getNumH() {
	return numH;
}

int vMaze::getNumW() {
	return numW;
}

int vMaze::getTotalPoints() {
	return totalPoints;
}

aTexture * vMaze::getTextures() {
	return textures;
}

mazeSquare * vMaze::getSquare(int x, int y) {
	mazeSquare * toReturn = NULL;
	if (squares != NULL && x < numW && y < numH) {
		toReturn = &(squares[x * numH + y]);
		if (x == 4 && !toReturn->wallRight) {
			bool breakMe = true;
		}
	}
	return toReturn;
}

vActor * vMaze::getActorByType(spriteType actorType) {
	// Returns the address of the actor with the given type
	vActor * toReturn = NULL;
	switch (actorType) {
		case V_RED_G:
			toReturn = blinky;
			break;
		case V_PINK_G:
			toReturn = pinky;
			break;
		case V_BLUE_G:
			toReturn = inky;
			break;
		case V_ORANGE_G:
			toReturn = clyde;
			break;
		case V_PACMAN:
		default:
			toReturn = pacman;
			break;
	}
	return toReturn;
}

vActor * vMaze::getSelection() {
	// Returns the address of the first actor that is selected
	for (int i = 0; i < 5; i++) {
		if (getActorByType((spriteType)i)->getIsSelected()) {
			return getActorByType((spriteType)i);
		}
	}
	pacman->select();
	return pacman;
}

vItem * vMaze::getItem(int x, int y) {
	if (items != NULL && x < numW && y < numH) {
		return &(items[x * numH + y]);
	}
	return NULL;
}
void vMaze::setSoundFlag(int n, bool s) {
	if (n < 0 || n >= numEffects) n = 0;
	soundFlags[n] = s;
}

void vMaze::pause() {
	isPaused = true;
}

void vMaze::unpause() {
	isPaused = false;
}

// --- Methods --- //

bool vMaze::checkAccessibility() {
	for (int i = 0; i < numW; i++) {
		for (int j = 0; j < numH; j++) {
			if (!getSquare(i, j)->accessible) {
				return false;
			}
		}
	}
	return true;
}

bool vMaze::executeAbility(vActor * subject) {
	// Executes special ability, sets ability timer, and returns success
	bool success = true;
	if (subject->getLevel() < 1) return false;

	// Controls level scaling for ability parameters, from 0.0f to 1.0f
	int cx, cy = 0;
	Dice die = Dice();
	time_t currentTime;
	time(&currentTime);
	if (difftime(currentTime, subject->getAbilityTriggered()) < subject->getLevel()) {
		// Ability still cooling down
		return false;
	}

	switch (subject->getType()) {
		case V_RED_G:
			// Sprint! (increase speed)
			// Check ability duration (level) + cooldown (level)
			if (difftime(currentTime, subject->getAbilityTriggered()) < 2 * subject->getLevel()) {
				return false;
			}
			// Adjust velocity
			blinky->setVelocity(baseVelocity * 0.8f * (2.0f - pow(0.5f, (float)subject->getLevel())));
			if (blinky->getVelX() != 0.0f) {
				blinky->setVelX(blinky->getVelX() > 0 ? blinky->getVelocity() : -blinky->getVelocity());
			}
			if (blinky->getVelY() != 0.0f) {
				blinky->setVelY(blinky->getVelY() > 0 ? blinky->getVelocity() : -blinky->getVelocity());
			}
			break;
		case V_PINK_G:
			// Jump! (skip walls)
			cx = screenX2mazeX((int)(subject->getX().value));
			cy = screenY2mazeY((int)(subject->getY().value));
			if (subject->getState() <= (int)SS_UP3) {
				cy += pinky->getLevel();
				if (cy >= numH) cy = numH-1;
			} else if (subject->getState() <= (int)SS_DOWN3) {
				cy -= pinky->getLevel();
				if (cy < 0) cy = 0;
			} else if (subject->getState() <= (int)SS_LEFT3) {
				cx -= pinky->getLevel();
				if (cx < 0) cx = 0;
			} else if (subject->getState() <= (int)SS_RIGHT3) {
				cx += pinky->getLevel();
				if (cx >= numW) cx = numW-1;
			}
			moveToMazeXY(subject, cx, cy);
			break;
		case V_BLUE_G:
			// Immunity (change scared state)
			// Check ability duration (level) + cooldown (level)
			if (difftime(currentTime, subject->getAbilityTriggered()) < 2 * subject->getLevel()) {
				return false;
			}
			// Break vulnerability
			inky->setScared(false);
			break;
		case V_ORANGE_G:
			// Scatter (teleport)
			cx = screenX2mazeX((int)(subject->getX().value));
			cy = screenY2mazeY((int)(subject->getY().value));
			int newX, newY;
			do {
				newX = die.rollIntRange(cx - 2 * clyde->getLevel(), cx + 2 * clyde->getLevel());
				newY = die.rollIntRange(cy - 2 * clyde->getLevel(), cy + 2 * clyde->getLevel());
				if (newX < 0) newX = 0;
				if (newX >= numW) newX = numW-1;
				if (newY < 0) newY = 0;
				if (newY >= numH) newY = numH-1;
			} while (newX == cx && newY == cy);
			moveToMazeXY(subject, newX, newY);
			break;
		default:
			success = false;
			break;
	}
	if (success) subject->setAbilityTriggered(currentTime);
	return success;
}

void vMaze::aStarPlot(int * values, int x, int y) {
	// Should only be looking from registered squares
	int curr = values[x * numH + y];
	if (curr == -1) {
		return;
	}

	// Recurse up
	if (y < numH-1 && (values[x * numH + y+1] > curr + 1 || values[x * numH + y+1] == -1) && !getSquare(x, y)->wallUp) {
		values[x * numH + y+1] = curr + 1;
		aStarPlot(values, x, y+1);
	}

	// Recurse left
	if (x > 0 && (values[(x-1) * numH + y] > curr + 1 || values[(x-1) * numH + y] == -1) && !getSquare(x, y)->wallLeft) {
		values[(x-1) * numH + y] = curr + 1;
		aStarPlot(values, x-1, y);
	}

	// Recurse down
	if (y > 0 && (values[x * numH + y-1] > curr + 1 || values[x * numH + y-1] == -1) && !getSquare(x, y)->wallDown) {
		values[x * numH + y-1] = curr + 1;
		aStarPlot(values, x, y-1);
	}

	// Recurse right
	if (x < numW-1 && (values[(x+1) * numH + y] > curr + 1 || values[(x+1) * numH + y] == -1) && !getSquare(x, y)->wallRight) {
		values[(x+1) * numH + y] = curr + 1;
		aStarPlot(values, x+1, y);
	}
}

void vMaze::drawWallSegment(int k, int x, int y, aGraphics * context) {
	// Renders segment at maze coordinates x, y using texture key k
	if (wallSegment == NULL) { return; }
	int screenX = (int)((x) * squareDim) + dx - (int)(squareDim / 2);
	int screenY = (int)((y) * squareDim) + dy - (int)(squareDim / 2);
	wallSegment->moveToPix(screenX, screenY);
	wallSegment->setState((spriteState)k);
	wallSegment->render(context);
}

void vMaze::moveToMazeXY(vActor * actor, int x, int y) {
	int cellX = mazeX2screenX(x);
	int cellY = mazeY2screenY(y);
	actor->moveToPix(cellX, cellY);
}

void vMaze::newLevel(aGraphics * context, bool reset) {
	// New level: resize, re-generate...
	level = reset ? 1 : level + 1;
	pacman->setLife(true);

	// Update point totals
	if (level == 1) {
		totalPoints = 0;
	} else {
		totalPoints += getCurrentPointsTotal();
	}

	// Maze size starts at 5x7 and asymptotically approaches 13x17
	int w = (int)((minW - maxW) / ((maxW - minW) * levelScaleSpeed * (level-1) + 1) + maxW);
	int h = (int)((minH - maxH) / ((maxH - minH) * levelScaleSpeed * (level-1) + 1) + maxH);
	resize(w, h, context);

	// Start pacman in random location along edge
	Dice die = Dice();
	int location = die.rollInt(2 * numW + 2 * numH);
	int pacX, pacY;
	if (location < numW) {
		pacX = location;
		pacY = numH-1;
	} else if (location < numW + numH) {
		pacX = 0;
		pacY = location - numW;
	} else if (location < 2 * numW + numH) {
		pacX = location - numH - numW;
		pacY = 0;
	} else {
		pacX = numW-1;
		pacY = location - 2 * numW - numH;
	}
	moveToMazeXY(pacman, pacX, pacY);
	items[pacX * numH + pacY].consume();

	// Available ghosts will depend upon level; reset to locations
	blinky->setLife(level > 0);
	pinky->setLife(level > 2);
	inky->setLife(level > 4);
	clyde->setLife(level > 6);

	// Set ghost locations
	moveToMazeXY(blinky, numW / 2, numH / 2 - 1);
	moveToMazeXY(pinky, numW / 2 - 1, numH / 2);
	moveToMazeXY(inky, numW / 2 + 1, numH / 2);
	moveToMazeXY(clyde, numW / 2, numH / 2);

	// Reset all velocities
	pacman->setVelX(0.0f); pacman->setVelY(0.0f);
	blinky->setVelX(0.0f); blinky->setVelY(0.0f);
	pinky->setVelX(0.0f); pinky->setVelY(0.0f);
	inky->setVelX(0.0f); inky->setVelY(0.0f);
	clyde->setVelX(0.0f); clyde->setVelY(0.0f);

	// Blinky will be selected first
	pacman->deselect();
	blinky->select();
	pinky->deselect();
	inky->deselect();
	clyde->deselect();

	// Generate maze
	generate(MA_DIVISION);

	// Reset ghost 'scared'ness
	blinky->setScared(false);
	pinky->setScared(false);
	inky->setScared(false);
	clyde->setScared(false);

	// Reset ghost levels, if new game
	if (reset) {
		blinky->reset();
		pinky->reset();
		inky->reset();
		clyde->reset();
	}
}

void vMaze::renderMaze(aGraphics * context) {
	// Arrange squares by quadrant
	mazeSquare * qOne;
	mazeSquare * qTwo;
	mazeSquare * qThree;

	// 9 possible states: 4 corners, 4 walls, and interior intersections
	for (int i = 0; i <= numW; i++) {
		for (int j = 0; j <= numH; j++) {
			if (i == 0) {
				if (j == 0) {
					// Bottom-left corner
					drawWallSegment(9, i, j, context);
				} else if (j == numH) {
					// Top-left corner
					drawWallSegment(12, i, j, context);
				} else {
					// Left wall
					qOne = getSquare(i, j);
					if (qOne->wallDown) {
						drawWallSegment(13, i, j, context);
					} else {
						drawWallSegment(5, i, j, context);
					}
				}
			} else if (i == numW) {
				if (j == 0) {
					// Bottom-right corner
					drawWallSegment(3, i, j, context);
				} else if (j == numH) {
					// Top-right corner
					drawWallSegment(6, i, j, context);
				} else {
					// Right wall
					qTwo = getSquare(i-1, j);
					if (qTwo->wallDown) {
						drawWallSegment(7, i, j, context);
					} else {
						drawWallSegment(5, i, j, context);
					}
				}
			} else {
				if (j == 0) {
					// Bottom wall
					qOne = getSquare(i, j);
					if (qOne->wallLeft) {
						drawWallSegment(11, i, j, context);
					} else {
						drawWallSegment(10, i, j, context);
					}
				} else if (j == numH) {
					// Top wall
					qThree = getSquare(i-1, j-1);
					if (qThree->wallRight) {
						drawWallSegment(14, i, j, context);
					} else {
						drawWallSegment(10, i, j, context);
					}
				} else {
					// Interior intersection
					qOne = getSquare(i, j);
					qThree = getSquare(i-1, j-1);
					int key = 1 * (int)qOne->wallLeft + 2 * (int)qThree->wallUp + 4 * (int)qThree->wallRight + 8 * (int)qOne->wallDown;
					drawWallSegment(key, i, j, context);
				}
			}
		}
	}

	// Render items on top; should be 1 in each square
	for (int i = 0; i < numW; i++) {
		for (int j = 0; j < numH; j++) {
			if (!items[i * numH + j].getIsConsumed()) {
				items[i * numH + j].moveToPix((int)((i) * squareDim) + dx + 10, (int)((j) * squareDim) + dy + 10);
				items[i * numH + j].render(context);
			}
		}
	}

	// Render actors (pacman, ghosts)
	blinky->render(context);
	pinky->render(context);
	inky->render(context);
	clyde->render(context);
	pacman->render(context);
}

void vMaze::rotateSelection() {
	// Rotate selected ghost to next alive
	int currentSelection = (int)(getSelection()->getType());
	blinky->deselect();
	pinky->deselect();
	inky->deselect();
	clyde->deselect();
	int numTries = 0;
	while (numTries < 4) {
		currentSelection = currentSelection % 4 + 1;
		if (getActorByType((spriteType)currentSelection)->getIsAlive()) {
			getActorByType((spriteType)currentSelection)->select();
			return;
		}
		numTries++;
	}
}

void vMaze::turnActor(vActor * actor, MazeDirection direction) {
	// Calculate relevant coordinates
	int mazeX = screenX2mazeX((int)(actor->getX().value));
	int mazeY = screenY2mazeY((int)(actor->getY().value));
	int cellX = mazeX2screenX(mazeX);
	int cellY = mazeY2screenY(mazeY);
	mazeSquare * currentSquare = getSquare(mazeX, mazeY);
	bool invalidTurn = false;

	// Determine current direction
	MazeDirection currDir;
	if (actor->getVelY() > 0.0f) {
		currDir = MD_UP;
	} else if (actor->getVelX() < 0.0f) {
		currDir = MD_LEFT;
	} else if (actor->getVelY() < 0.0f) {
		currDir = MD_DOWN;
	} else if (actor->getVelX() > 0.0f) {
		currDir = MD_RIGHT;
	} else {
		currDir = MD_NONE;
	}

	// Turning the same way? Ignore.
	if (direction == currDir) { return; }

	// Validity of direction determined by relative location through square compared to desired direction
	if (direction == MD_UP || direction == MD_DOWN) {
		invalidTurn = (actor->getX().value > cellX + 0.25f * squareDim || actor->getX().value < cellX - 0.25f * squareDim) && direction != MD_RIGHT;
	} else if (direction == MD_LEFT || direction == MD_RIGHT) {
		invalidTurn = (actor->getY().value > cellY + 0.25f * squareDim || actor->getY().value < cellY - 0.25f * squareDim) && direction != MD_DOWN;
	}

	// Turn the given actor in the given direction
	// Stop if the direction is blocked, accelerate otherwise
	switch (direction) {
		case MD_UP:
			actor->setState(SS_UP2);
			actor->setVelX(0.0f);
			actor->setVelY(invalidTurn || (currentSquare->wallUp && actor->getY().value >= cellY) ? 0.0f : actor->getVelocity());
			break;
		case MD_LEFT:
			actor->setState(SS_LEFT2);
			actor->setVelX(invalidTurn || (currentSquare->wallLeft && actor->getX().value <= cellX) ? 0.0f : -actor->getVelocity());
			actor->setVelY(0.0f);
			break;
		case MD_DOWN:
			actor->setState(SS_DOWN2);
			actor->setVelX(0.0f);
			actor->setVelY(invalidTurn || (currentSquare->wallDown && actor->getY().value <= cellY) ? 0.0f : -actor->getVelocity());
			break;
		case MD_RIGHT:
			actor->setState(SS_RIGHT2);
			actor->setVelX(invalidTurn || (currentSquare->wallRight && actor->getX().value >= cellX) ? 0.0f : actor->getVelocity());
			actor->setVelY(0.0f);
			break;
		default:
			actor->setState(SS_NA);
			actor->setVelX(0.0f);
			actor->setVelY(0.0f);
			break;
	}

	// Adjust location to align with the current square
	if (actor->getVelX() == 0.0f && actor->getVelY() != 0.0f) {
		actor->setX((float)cellX);
	}
	if (actor->getVelY() == 0.0f && actor->getVelX() != 0.0f) {
		actor->setY((float)cellY);
	}
}

void vMaze::update(float dt) {
	if (isPaused) return;

	// Make sure border walls are set
	setVertWall(0);
	setVertWall(numW);
	setHorizWall(0);
	setHorizWall(numH);

	// Update actors
	int px, py;
	vActor * currActor = NULL;
	for (int i = 0; i < 5; i++) {
		currActor = getActorByType((spriteType)i);
		if (currActor->getIsAlive()) {
			// Calculate maze coordinate and cell center of current location
			int mx = screenX2mazeX((int)(currActor->getX().value));
			int my = screenY2mazeY((int)(currActor->getY().value));
			int cx = mazeX2screenX(mx);
			int cy = mazeY2screenY(my);

			// Check to see if actor is fully entered (more than halfway) through the cell
			bool fullyEntered = false;
			if (currActor->getVelY() > 0 && currActor->getY().value >= cy) {
				fullyEntered = true;
			} else if (currActor->getVelX() < 0 && currActor->getX().value <= cx) {
				fullyEntered = true;
			} else if (currActor->getVelY() < 0 && currActor->getY().value <= cy) {
				fullyEntered = true;
			} else if (currActor->getVelX() > 0 && currActor->getX().value >= cx) {
				fullyEntered = true;
			}

			// Check pacman, ghost coordinates for intersection (will someone be eaten?)
			if (currActor == pacman) {
				px = (int)(currActor->getX().value);
				py = (int)(currActor->getY().value);
			} else {
				if (abs((int)(currActor->getX().value) - px) < 8 && abs((int)(currActor->getY().value) - py) < 8) {
					if (currActor->getIsScared()) {
						// Ghost will perish! Play sound effect, set death
						setSoundFlag(6, true);
						currActor->setLife(false);
						currActor->reset();
					} else {
						// Pacman will perish! Play sound effect, set death
						setSoundFlag(7, true);
						pacman->setLife(false);
					}
				}
			}

			// Is there a wall in direction of velocity, and are we more than halfway through the cell?
			// If so, stop and reset to center of cell, facing in same direction but motionless
			mazeSquare * cell = getSquare(mx, my);
			if (cell->wallUp && currActor->getVelY() > 0 && fullyEntered) {
				currActor->setVelY(0.0f);
				currActor->setY((float)cy);
			} else if (cell->wallLeft && currActor->getVelX() < 0 && fullyEntered) {
				currActor->setVelX(0.0f);
				currActor->setX((float)cx);
			} else if (cell->wallDown && currActor->getVelY() < 0 && fullyEntered) {
				currActor->setVelY(0.0f);
				currActor->setY((float)cy);
			} else if (cell->wallRight && currActor->getVelX() > 0 && fullyEntered) {
				currActor->setVelX(0.0f);
				currActor->setX((float)cx);
			}

			// Align within wall
			if (currActor->getVelX() != 0.0f && currActor->getVelY() == 0.0f) {
				currActor->setY((float)cy);
			}
			else if (currActor->getVelY() != 0.0f && currActor->getVelX() == 0.0f) {
				currActor->setX((float)cx);
			}

			if (i == 0) {
				// Check pacman consumption
				if (items != NULL && !items[mx * numH + my].getIsConsumed() && fullyEntered) {
					setSoundFlag(1, !getSoundFlag(1) && items[mx * numH + my].getItemType() == IT_SMALL_DOT);
					setSoundFlag(2, !getSoundFlag(2) && items[mx * numH + my].getItemType() == IT_LARGE_DOT);
					setSoundFlag(3, !getSoundFlag(3) && !getSoundFlag(1) && !getSoundFlag(2));
					items[mx * numH + my].consume();
					if (items[mx * numH + my].getItemType() == IT_LARGE_DOT) {
						// Begin vulnerability! Change ghost sprites, pacman ai
						blinky->setScared(true);
						pinky->setScared(true);
						inky->setScared(true);
						clyde->setScared(true);
						pacman->setMode(AI_HOMICIDAL);
						setSoundFlag(5, true);
						time(&lastVulnerability);
					}
				} else {
					setSoundFlag(1, false);
					setSoundFlag(2, false);
					setSoundFlag(3, false);
				}

				// AI time!
				applyAi(pacman);
			}
			currActor->update(dt);
		}
	}

	// Check vulnerability countdown
	time_t currTime;
	time(&currTime);
	double dif = difftime(currTime, lastVulnerability);
	if (dif > vulnerabilityDuration + 0.5 * level) {
		blinky->setScared(false);
		pinky->setScared(false);
		inky->setScared(false);
		clyde->setScared(false);
		pacman->setMode(AI_GREEDY);
	}

	// Check ability duration (blinky, inky only)
	if (difftime(currTime, blinky->getAbilityTriggered()) > blinky->getLevel()) {
		blinky->setVelocity(0.8f * baseVelocity);
	}
	if (difftime(currTime, inky->getAbilityTriggered()) > inky->getLevel()) {
		inky->setScared(blinky->getIsScared());
	}
}

// --- Coordinate Transformations --- //

int vMaze::mazeX2screenX(int x) {
	// Converts maze X coordinate to pixel X coordinate on screen
	return (int)(x * squareDim) + dx + (int)(0.5f * squareDim);
}

int vMaze::mazeY2screenY(int y) {
	// Converts maze Y coordinate to pixel Y coordinate on screen
	return (int)(y * squareDim) + dy + (int)(0.5f * squareDim);
}

int vMaze::screenX2mazeX(int x) {
	// Converts pixel X coordinate on screen to maze X coordinate
	int toReturn = (int)(((float)x - dx - 0.5f * squareDim) / squareDim + 0.5f);
	if (toReturn < 0) { toReturn = -1; }
	if (toReturn > numW) { toReturn = numW; }
	return toReturn;
}

int vMaze::screenY2mazeY(int y) {
	// Converts pixel Y coordinate on screen to maze Y coordinate
	int toReturn = (int)(((float)(y - dy - 0.5f * squareDim)) / squareDim + 0.5f);
	if (toReturn < 0) { toReturn = -1; }
	if (toReturn > numH) { toReturn = numH; }
	return toReturn;
}
