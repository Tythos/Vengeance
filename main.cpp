/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Entry point
	Begun Monday, April 26th, 2010
*/

#include "libArtemisExt.h"
#include <time.h>
#include <math.h>

// --- Game State Management --- //
enum VengeanceState { VS_LEVEL_START, VS_LEVEL_PLAY, VS_VICTORY, VS_DEFEAT, VS_LEVELING };
VengeanceState currState, prevState;
time_t lastStateChange;

void changeState(VengeanceState newState) {
	currState = newState;
	time(&lastStateChange);
}

// --- Game Messages --- //
bool showHelpMsg = false;
bool showStatusMsg = false;
kString helpMsg;
kString statusMsg;
ScreenDimension helpMsgX;

// --- Game resources --- //
aApp * game;
Mix_Music * mus1;
Mix_Music * mus2;
Mix_Music * mus3;
Mix_Chunk * snd1;
Mix_Chunk * snd2;
Mix_Chunk * snd3;
Mix_Chunk * snd4;
Mix_Chunk * snd5;
Mix_Chunk * snd6;
Mix_Chunk * snd7;
Mix_Chunk * snd8;

// --- Game Classes --- //
#include "vSprite.h"
#include "vMaze.h"

// --- Game objects --- //
vMaze * maze;
vSprite * ghostTip;

// --- Game Mechanics --- //
#include "events.cpp"

kString getMessage(int level) {
	kString toReturn;
	switch (level) {
		case 1:
			toReturn = "Pacman, the iconic evildoer, has invaded the ancestral ghost lands for the last time. Use your arrow keys to hunt down Pacman with Blinky, the Red Ghost, and eliminate him!";
			break;
		case 2:
			toReturn = "Blinky isn't quite as fast as Pacman, so press 'e' to grant him his special ability, Sprint, and gain enough speed to chase him down!";
			break;
		case 3:
			toReturn = "Press 'spacebar' to switch between ghosts. Use Pinky in conjunction with Blinky for more effective teamwork. Pacman is beginning to feel your vengeance!";
			break;
		case 4:
			toReturn = "If you've chosen to level up Pinky's ability, Jump, you can now use her to jump through walls!";
			break;
		case 5:
			toReturn = "Inky is the third ghost to join your quest for vengeance. He can become immune to Pacman's fruit-fueled bloodthirst for a short time.";
			break;
		case 6:
			toReturn = "Try and keep Pacman from closing in on the Big Dots to keep him from eating your Ghosts! The faster you can rotate through your Ghosts, the easier you can close in on Pacman.";
			break;
		case 7:
			toReturn = "The fourth and final Ghost on your squad is Clyde! His ability, Scatter, will randomly transport him to any square in the realm. Use Clyde to keep Pacman on his toes!";
			break;
		case 8:
			toReturn = "As the realm continues to grow in size, the stakes get higher and Pacman becomes more difficult to track down. You'll make more points each level by keeping Pacman from consuming your dots and fruit!";
			break;
		default:
			toReturn = " ";
			break;
	}
	return toReturn;
}

void renderInterface() {
	// Summarize current selection above maze: ghost, level, and ability
	ScreenDimension sd1 = ScreenDimension(); sd1.value = 64.0f;
	ScreenDimension sd2 = ScreenDimension(); sd2.value = 816.0f;
	kString selectionSummary;
	vActor * currentSelection = maze->getSelection();
	time_t currTime;
	time(&currTime);

	// Determine text
	switch (currentSelection->getType()) {
		case V_RED_G:
			selectionSummary = kString("Blinky   Level: ") + currentSelection->getLevel();
			// Only show ability description if not on cooldown
			if (difftime(currTime, currentSelection->getAbilityTriggered()) > 2 * currentSelection->getLevel() && currentSelection->getLevel() > 0) {
				selectionSummary = selectionSummary + "   Ability: Sprint";
			}
			game->hGraphics->hTypewriter->setColor(1.0f, 0.0f, 0.0f);
			break;
		case V_PINK_G:
			selectionSummary = kString("Pinky    Level: ") + currentSelection->getLevel();
			if (difftime(currTime, currentSelection->getAbilityTriggered()) > currentSelection->getLevel() && currentSelection->getLevel() > 0) {
				selectionSummary = selectionSummary + "   Ability: Jump";
			}
			game->hGraphics->hTypewriter->setColor(1.0f, 0.722f, 0.871f);
			break;
		case V_BLUE_G:
			selectionSummary = kString("Inky     Level: ") + currentSelection->getLevel();
			if (difftime(currTime, currentSelection->getAbilityTriggered()) > 2 * currentSelection->getLevel() && currentSelection->getLevel() > 0) {
				selectionSummary = selectionSummary + "   Ability: Immunity";
			}
			game->hGraphics->hTypewriter->setColor(0.0f, 1.0f, 0.871f);
			break;
		case V_ORANGE_G:
			selectionSummary = kString("Clyde    Level: ") + currentSelection->getLevel();
			if (difftime(currTime, currentSelection->getAbilityTriggered()) > currentSelection->getLevel() && currentSelection->getLevel() > 0) {
				selectionSummary = selectionSummary + "   Ability: Scatter";
			}
			game->hGraphics->hTypewriter->setColor(1.0f, 0.722f, 0.278f);
			break;
		default:
			selectionSummary = kString("No moar ghostz!");
			game->hGraphics->hTypewriter->setColor(1.0f, 1.0f, 1.0f);
			break;
	}

	// Show selection?
	if (currState != VS_LEVELING) {
		// Render selection text
		game->hGraphics->hTypewriter->moveCursor(sd1, sd2);
		game->hGraphics->hTypewriter->type(selectionSummary, game->hGraphics->getWidth(), game->hGraphics->getHeight());

		// Render selection's ghost sprite
		ghostTip->setX(sd1.value - 1.5f * ghostTip->getW().value);
		ghostTip->setY(sd2.value + 0.25f * ghostTip->getH().value);
		ghostTip->setType(currentSelection->getType());
		ghostTip->render(game->hGraphics);
	}

	// Render status message?
	if (showStatusMsg) {
		ScreenDimension sd3 = ScreenDimension(); sd3.unit = UNIT_PIX; sd3.value = game->hGraphics->getWidth() / 2 - 64.0f;
		ScreenDimension sd4 = ScreenDimension(); sd4.unit = UNIT_PIX; sd4.value = game->hGraphics->getHeight() / 2 + 1.0f;
		float size = game->hGraphics->hTypewriter->getFontSize();
		game->hGraphics->hTypewriter->setFontSize(64.0f);
		game->hGraphics->hTypewriter->setColor(1.0f, 1.0f, 1.0f);
		game->hGraphics->hTypewriter->moveCursor(sd3, sd4);
		game->hGraphics->hTypewriter->type(statusMsg, game->hGraphics->getWidth(), game->hGraphics->getHeight());
		game->hGraphics->hTypewriter->setFontSize(size);
	}

	// Render help message?
	if (showHelpMsg) {
		ScreenDimension helpMsgY = ScreenDimension();
		helpMsgY.unit = UNIT_PCT;
		helpMsgY.value = 0.05f;
		helpMsgY.align = ALIGN_NEGATIVE;
		game->hGraphics->hTypewriter->setColor(0.61f, 0.61f, 0.91f);
		game->hGraphics->hTypewriter->moveCursor(helpMsgX, helpMsgY);
		game->hGraphics->hTypewriter->type(helpMsg, game->hGraphics->getWidth(), game->hGraphics->getHeight());
	}

	// Show scores
	kString sc1 = kString("Points remaining: ") + maze->getCurrentPointsTotal();
	kString sc2 = kString("Points saved: ") + maze->getTotalPoints();
	ScreenDimension sd5 = ScreenDimension(); sd5.value = 10.0f;
	ScreenDimension sd6 = ScreenDimension(); sd6.value = 350.0f;
	game->hGraphics->hTypewriter->setColor(0.1f, 0.2f, 0.4f);
	game->hGraphics->hTypewriter->moveCursor(sd5, sd5);
	game->hGraphics->hTypewriter->type(sc1, game->hGraphics->getWidth(), game->hGraphics->getHeight());
	game->hGraphics->hTypewriter->moveCursor(sd6, sd5);
	game->hGraphics->hTypewriter->type(sc2, game->hGraphics->getWidth(), game->hGraphics->getHeight());
	return;
}

void extUpdate(float dt) {
	// Manage states
	if (prevState != currState) {
		// State change
		switch (currState) {
			case VS_LEVEL_START:
				game->hSoundboard->playSong(mus1);
				if (prevState == VS_DEFEAT) {
					maze->newLevel(game->hGraphics, true);
				} else {
					maze->newLevel(game->hGraphics);
				}
				maze->pause();
				showHelpMsg = true;
				helpMsgX.value = 0.8f;
				helpMsg = getMessage(maze->getLevel());
				showStatusMsg = false;
				break;
			case VS_VICTORY:
				game->hSoundboard->playSong(mus3);
				maze->pause();
				showHelpMsg = false;
				showStatusMsg = true;
				statusMsg = "Victory!";
				break;
			case VS_DEFEAT:
				game->hSoundboard->playSong(mus2);
				maze->pause();
				showHelpMsg = true;
				helpMsg = "You have been defeated! Pacman roams across your ancestral lands, consuming all in his path. Press 'spacebar' to start a new game!";
				helpMsgX.value = 0.8f;
				showStatusMsg = true;
				statusMsg = "Defeat!";
				break;
			case VS_LEVELING:
				maze->pause();
				showHelpMsg = true;
				helpMsg = "Choose which Ghost will be leveled up by pressing the corresponding key. Abilities will have 1 second of additional cooldown per level.";
				helpMsgX.value = 0.8f;
				showStatusMsg = false;
				break;
			case VS_LEVEL_PLAY:
			default:
				maze->unpause();
				showHelpMsg = true;
				helpMsg = getMessage(maze->getLevel());
				showStatusMsg = false;
				break;
		}
	}

	prevState = currState;
	if (showHelpMsg) {
		static float scrollSpeed = 0.3f; // pct / second?
		helpMsgX.value -= dt * scrollSpeed;
		helpMsgX.unit = UNIT_PCT;
		helpMsgX.align = ALIGN_NEGATIVE;
	}
}

void renderLeveling(aGraphics * context) {
	// Render leveling interface
	ScreenDimension fromBottom = ScreenDimension();
	ScreenDimension fromLeft = ScreenDimension();
	fromBottom.value = 0.8f;
	fromBottom.unit = UNIT_PCT;
	fromLeft.value = 0.1f;
	fromLeft.unit = UNIT_PCT;
	float diff = 0.1f;
	kString label;

	ghostTip->setX(fromLeft.toPix(game->hGraphics->getWidth()) - 48.0f);
	if (maze->blinky->getIsAlive()) {
		// Blinky
		ghostTip->setY(fromBottom.toPix(game->hGraphics->getHeight()) + 8.0f);
		ghostTip->setType(V_RED_G);
		ghostTip->render(context);
		int factor = (int)(100.0f * (1.0f - pow(0.5f, (float)(maze->blinky->getLevel() + 1))));
		label = kString("Level up Sprint ('b'): ") + factor; label = label + "% speed bonus";
		game->hGraphics->hTypewriter->setColor(1.0f, 0.0f, 0.0f);
		game->hGraphics->hTypewriter->moveCursor(fromLeft, fromBottom);
		game->hGraphics->hTypewriter->type(label, game->hGraphics->getWidth(), game->hGraphics->getHeight());
		fromBottom.value -= diff;
	}

	if (maze->pinky->getIsAlive()) {
		// Pinky
		ghostTip->setY(fromBottom.toPix(game->hGraphics->getHeight()) + 8.0f);
		ghostTip->setType(V_PINK_G);
		ghostTip->render(context);
		label = kString("Level up Jump ('p'): ") + (maze->pinky->getLevel()+1); label = label + " square distance";
		game->hGraphics->hTypewriter->setColor(1.0f, 0.722f, 0.871f);
		game->hGraphics->hTypewriter->moveCursor(fromLeft, fromBottom);
		game->hGraphics->hTypewriter->type(label, game->hGraphics->getWidth(), game->hGraphics->getHeight());
		fromBottom.value -= diff;
	}

	if (maze->inky->getIsAlive()) {
		// Inky
		ghostTip->setY(fromBottom.toPix(game->hGraphics->getHeight()) + 8.0f);
		ghostTip->setType(V_BLUE_G);
		ghostTip->render(context);
		label = kString("Level up Immunity ('i'): ") + (maze->inky->getLevel()+1); label = label + " second duration";
		game->hGraphics->hTypewriter->setColor(0.0f, 1.0f, 0.871f);
		game->hGraphics->hTypewriter->moveCursor(fromLeft, fromBottom);
		game->hGraphics->hTypewriter->type(label, game->hGraphics->getWidth(), game->hGraphics->getHeight());
		fromBottom.value -= diff;
	}

	if (maze->clyde->getIsAlive()) {
		// Clyde
		ghostTip->setY(fromBottom.toPix(game->hGraphics->getHeight()) + 8.0f);
		ghostTip->setType(V_ORANGE_G);
		ghostTip->render(context);
		label = kString("Level up Scatter ('c'): ") + 2*(maze->clyde->getLevel()+1); label = label + " square radius";
		game->hGraphics->hTypewriter->setColor(1.0f, 0.722f, 0.278f);
		game->hGraphics->hTypewriter->moveCursor(fromLeft, fromBottom);
		game->hGraphics->hTypewriter->type(label, game->hGraphics->getWidth(), game->hGraphics->getHeight());
		fromBottom.value -= diff;
	}
}

bool extRender() {
	float dt = 0.01f;
	if (currState == VS_LEVELING) {
		renderLeveling(game->hGraphics);
	} else {
		maze->renderMaze(game->hGraphics);
	}
	renderInterface();
	maze->update(dt);
	extUpdate(dt);
	return true;
}

int main(int argc, char * argv[]) {
	// Initialize game
	game = new aApp();
	game->hGraphics->setScreen(870, 675, 32);
	game->hCursor->setVisible(false);

	// We don't want to show the origin coordinate frame for a 2d game
	game->setOriginVisibility(false);

	// Configure console, fonts
	game->hConsole->setFont("CONSOLAB.TTF");
	game->hConsole->setBgColor(0.871f, 0.871f, 0.871f, 1.0f);
	game->hConsole->setFontColor(0.0f, 0.0f, 0.0f);
	game->hConsole->setFontSize(0.04f);
	game->hConsole->setVisible(false);

	// Add events
	game->gameEvents->createElement(ASTATE_GLOBAL, (*quitTrig), (*quitTarg));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*toggleConsoleTrig), (*toggleConsoleTarg));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*arrowUp), (*moveUp));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*arrowDown), (*moveDown));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*arrowLeft), (*moveLeft));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*arrowRight), (*moveRight));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*pressSpacebar), (*toggleSelection));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*pressN), (*newMaze));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*pressD), (*outputDebug));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*checkSnd1), (*playSnd1));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*checkSnd2), (*playSnd2));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*checkSnd3), (*playSnd3));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*checkSnd4), (*playSnd4));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*checkSnd5), (*playSnd5));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*checkSnd6), (*playSnd6));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*checkSnd7), (*playSnd7));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*checkSnd8), (*playSnd8));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*pressP), (*togglePaused));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*pressE), (*executeAbility));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*endLevelStartupTrigger), (*endLevelStartupAction));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*endLevelPlayWinTrigger), (*endLevelPlayWinAction));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*endLevelPlayLoseTrigger), (*endLevelPlayLoseAction));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*endVictoryTrigger), (*endVictoryAction));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*endDefeatTrigger), (*endDefeatAction));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*levelBlinkyTrigger), (*levelBlinkyAction));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*levelPinkyTrigger), (*levelPinkyAction));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*levelInkyTrigger), (*levelInkyAction));
	game->gameEvents->createElement(ASTATE_GLOBAL, (*levelClydeTrigger), (*levelClydeAction));

	// Preload music and turn looping off
	mus1 = game->hSoundboard->loadSong("Start.mp3");
	mus2 = game->hSoundboard->loadSong("Fail.mp3");
	mus3 = game->hSoundboard->loadSong("Win.mp3");
	game->hSoundboard->repeatSongs = false;

	// Preload sound effects
	snd1 = game->hSoundboard->loadSound("Hit1.wav");
	snd2 = game->hSoundboard->loadSound("Hit2.wav");
	snd3 = game->hSoundboard->loadSound("Pop.wav");
	snd4 = game->hSoundboard->loadSound("Powerdown.wav");
	snd5 = game->hSoundboard->loadSound("Powerup.wav");
	snd6 = game->hSoundboard->loadSound("Bad.wav");
	snd7 = game->hSoundboard->loadSound("Yay.wav");
	snd8 = game->hSoundboard->loadSound("Laser.wav");

	// Load maze
	maze = new vMaze();
	maze->newLevel(game->hGraphics);

	// Load tip sprite
	ghostTip = new vSprite(maze->getTextures());
	ghostTip->setState(SS_RIGHT2);

	// Make sure maze and other misc content is rendered
	game->externalRender = extRender;

	// Run game
	prevState = VS_DEFEAT;
	changeState(VS_LEVEL_START);
	game->execute();

	// Terminate game
	game->terminate();
	delete game;
	return 0;
}

