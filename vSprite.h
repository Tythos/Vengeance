/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Sprite class
	Begun Monday, May 3rd, 2010

	The sprite class, extending the panel class, represents a moving object within the maze with a specific state
	and a type that determine which segment of the universal texture map will be used to render the 2d element.
*/

#ifndef VENGEANCE_SPRITE_H
#define VENGEANCE_SPRITE_H

#include "libArtemisExt.h"

// Sprite type dteremines roll and row within texture map
enum spriteType { V_PACMAN, V_RED_G, V_PINK_G, V_BLUE_G, V_ORANGE_G, V_SCARED_G, V_WALLS, V_CONSUMABLE };

// Sprite state determines animation, velocity, and column within texture map
enum spriteState { SS_NA, SS_UP1, SS_UP2, SS_UP3, SS_DOWN1, SS_DOWN2, SS_DOWN3, SS_LEFT1, SS_LEFT2, SS_LEFT3, SS_RIGHT1, SS_RIGHT2, SS_RIGHT3, SS_SPEC1, SS_SPEC2, SS_SPEC3 };

// Global variables used to tune parameters
static float spriteDim = 32.0f;
static float spriteTex = 0.0625f; // Percent of texture taken up by single sprite
static float baseVelocity = 100.0f; // Pixels / second
static float animationPeriod = 0.66f; // Number of seconds per animation cycle
static float animationRatio = 0.33f; // Proportion of non-centered to centered sprite time in animation cycle

class vSprite : public aPanel {
private:
protected:
	// Data
	float timeSeed;
	float xVel; // Pixels / second
	float yVel; // Pixels / second
	spriteState state;
	spriteType type;
public:
	// Constructors
	vSprite(aTexture * texture);
	~vSprite();

	// Retrievers
	float getVelX();
	float getVelY();
	spriteState getState();
	spriteType getType();

	// Setters
	void setAlignment(bool isCentered);
	void setState(spriteState s);
	void setTimeSeed(float t);
	void setVelX(float v);
	void setVelY(float v);

	// Overridden & Virtual Methods
	virtual void render(aGraphics * context);
	virtual void setType(spriteType t);
	virtual void update(float dt);

	// Methods
	void moveToPix(int px, int py);
};

#endif