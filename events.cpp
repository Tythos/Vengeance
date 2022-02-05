/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Game mechanics
	Begun Monday, April 26th, 2010
*/

bool quitTrig() {
	return game->hKeyboard->checkKey(SDLK_ESCAPE);
}

void quitTarg() {
	game->debug(kString("Quitting..."));
	game->isLooping = false;
}

bool toggleConsoleTrig() {
	return game->hKeyboard->checkPressDown('`');
}

void toggleConsoleTarg() {
	game->debug(kString("Toggling console..."));
	if (game->hConsole->state == CS_DOWN || game->hConsole->state == CS_MOVING_DOWN) {
		game->hConsole->state = CS_MOVING_UP;
	} else {
		game->hConsole->state = CS_MOVING_DOWN;
	}
}

bool arrowUp() {
	return game->hKeyboard->checkKey(SDLK_UP);
}

bool arrowDown() {
	return game->hKeyboard->checkKey(SDLK_DOWN);
}

bool arrowLeft() {
	return game->hKeyboard->checkKey(SDLK_LEFT);
}

bool arrowRight() {
	return game->hKeyboard->checkKey(SDLK_RIGHT);
}

void moveUp() {
	game->debug(kString("Moving up..."));
	maze->turnActor(maze->getSelection(), MD_UP);
}

void moveDown() {
	game->debug(kString("Moving down..."));
	maze->turnActor(maze->getSelection(), MD_DOWN);
}

void moveLeft() {
	game->debug(kString("Moving left..."));
	maze->turnActor(maze->getSelection(), MD_LEFT);
}

void moveRight() {
	game->debug(kString("Moving right..."));
	maze->turnActor(maze->getSelection(), MD_RIGHT);
}

bool pressSpacebar() {
	return game->hKeyboard->checkPressDown(' ');
}

void toggleSelection() {
	game->debug(kString("Rotating selection..."));
	maze->rotateSelection();
}

bool pressN() {
	return game->hKeyboard->checkPressDown('n');
}

void newMaze() {
	game->debug(kString("Generating new maze..."));
	maze->newLevel(game->hGraphics);
}

bool pressD() {
	return game->hKeyboard->checkPressDown('d');
}

void outputDebug() {
	// Print out A* path distances
	int * distances = new int[maze->getNumW() * maze->getNumH()];
	for (int i = 0; i < maze->getNumW() * maze->getNumH(); i++) {
		distances[i] = -1;
	}
	distances[maze->pacman->getWayX() * maze->getNumH() + maze->pacman->getWayY()] = 0;
	maze->aStarPlot(distances, maze->pacman->getWayX(), maze->pacman->getWayY());
	for (int j = maze->getNumH()-1; j >= 0; j--) {
		kString line = kString(j) + kString(" row: ");
		for (int i = 0; i < maze->getNumW(); i++) {
			line = line + kString(distances[i * maze->getNumH() + j]);
			line = line + kString(", ");
			game->hConsole->addLine(line);
		}
	}
	delete[] distances;
	return;
}

bool checkSnd1() {
	return maze->getSoundFlag(1);
}

bool checkSnd2() {
	return maze->getSoundFlag(2);
}

bool checkSnd3() {
	return maze->getSoundFlag(3);
}

bool checkSnd4() {
	return maze->getSoundFlag(4);
}

bool checkSnd5() {
	return maze->getSoundFlag(5);
}

bool checkSnd6() {
	return maze->getSoundFlag(6);
}

bool checkSnd7() {
	return maze->getSoundFlag(7);
}

bool checkSnd8() {
	return maze->getSoundFlag(8);
}

void playSnd1() {
	game->hSoundboard->playSound(snd1);
	maze->setSoundFlag(1, false);
}

void playSnd2() {
	game->hSoundboard->playSound(snd2);
	maze->setSoundFlag(2, false);
}

void playSnd3() {
	game->hSoundboard->playSound(snd3);
	maze->setSoundFlag(3, false);
}

void playSnd4() {
	game->hSoundboard->playSound(snd4);
	maze->setSoundFlag(4, false);
}

void playSnd5() {
	game->hSoundboard->playSound(snd5);
	maze->setSoundFlag(5, false);
}

void playSnd6() {
	game->hSoundboard->playSound(snd6);
	maze->setSoundFlag(6, false);
}

void playSnd7() {
	game->hSoundboard->playSound(snd7);
	maze->setSoundFlag(7, false);
}

void playSnd8() {
	game->hSoundboard->playSound(snd8);
	maze->setSoundFlag(8, false);
}

bool pressP() {
	return game->hKeyboard->checkPressDown('p');
}

void togglePaused() {
	if (maze->getIsPaused()) {
		game->debug("Unpausing game...");
		maze->unpause();
	} else {
		game->debug("Pausing game...");
		maze->pause();
	}
}

bool pressE() {
	return currState == VS_LEVEL_PLAY && game->hKeyboard->checkPressDown('e');
}

void executeAbility() {
	// Execute ability; play noise if successful
	if (maze->executeAbility(maze->getSelection())) {
		maze->setSoundFlag(8, true);
	}
}

bool endLevelStartupTrigger() {
	time_t currTime;
	time(&currTime);
	return currState == VS_LEVEL_START && difftime(currTime, lastStateChange) >= 3;
}

void endLevelStartupAction() {
	changeState(VS_LEVEL_PLAY);
}

bool endLevelPlayWinTrigger() {
	return currState == VS_LEVEL_PLAY && !maze->pacman->getIsAlive();
}

void endLevelPlayWinAction() {
	changeState(VS_VICTORY);
}

bool endLevelPlayLoseTrigger() {
	return currState == VS_LEVEL_PLAY && !maze->blinky->getIsAlive() && !maze->pinky->getIsAlive() && !maze->inky->getIsAlive() && !maze->clyde->getIsAlive();
}

void endLevelPlayLoseAction() {
	changeState(VS_DEFEAT);
}

bool endVictoryTrigger() {
	time_t currTime;
	time(&currTime);
	return currState == VS_VICTORY && difftime(currTime, lastStateChange) >= 3;
}

void endVictoryAction() {
	changeState(VS_LEVELING);
}

bool endDefeatTrigger() {
	time_t currTime;
	time(&currTime);
	return currState == VS_DEFEAT && difftime(currTime, lastStateChange) >= 3 && game->hKeyboard->checkPressDown(' ');
}

void endDefeatAction() {
	changeState(VS_LEVEL_START);
}

bool levelBlinkyTrigger() {
	return currState == VS_LEVELING && game->hKeyboard->checkPressDown('b') && maze->blinky->getIsAlive();
}

void levelBlinkyAction() {
	maze->blinky->levelUp();
	changeState(VS_LEVEL_START);
}

bool levelPinkyTrigger() {
	return currState == VS_LEVELING && game->hKeyboard->checkPressDown('p') && maze->pinky->getIsAlive();
}

void levelPinkyAction() {
	maze->pinky->levelUp();
	changeState(VS_LEVEL_START);
}

bool levelInkyTrigger() {
	return currState == VS_LEVELING && game->hKeyboard->checkPressDown('i') && maze->inky->getIsAlive();
}

void levelInkyAction() {
	maze->inky->levelUp();
	changeState(VS_LEVEL_START);
}

bool levelClydeTrigger() {
	return currState == VS_LEVELING && game->hKeyboard->checkPressDown('c') && maze->clyde->getIsAlive();
}

void levelClydeAction() {
	maze->clyde->levelUp();
	changeState(VS_LEVEL_START);
}
