/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Actor sprite class
	Begun Monday, June 7th, 2010

	The Actor class extends the sprite class, and encapsulates information and abilities specific to player objects
	moving around in the world, such as ghosts and pacman
*/

#ifndef VENGEANCE_ACTOR_H
#define VENGEANCE_ACTOR_H

#include "vSprite.h"
#include "libArtemis.h"
#include <time.h>

// Several AI modes exist
enum AiObjective { AI_NONE, AI_AVOID, AI_HOMICIDAL, AI_GREEDY, AI_RANDOM };

class vActor : public vSprite {
private:
	// Data
	float velocity;
	bool isAlive;
	bool isSelected;
	bool isScared;
	int level;
	int wayX;
	int wayY;
	AiObjective mode;
	time_t abilityTriggered;
protected:
public:
	// Constructors
	vActor();
	~vActor();

	// Actor retrievers
	float getVelocity();
	bool getIsAlive();
	bool getIsSelected();
	bool getIsScared();
	int getLevel();
	int getWayX();
	int getWayY();
	AiObjective getMode();
	time_t getAbilityTriggered();

	// Actor setters
	void setVelocity(float v);
	void setLife(bool l);
	void select();
	void deselect();
	void setScared(bool s);
	void levelUp();	
	void reset();
	void setWaypoint(int x, int y);
	void setMode(AiObjective m);
	void setAbilityTriggered(time_t t);

	// Overridden accessors
	void setTexture(aTexture * t);
	void setType(spriteType t);

	// Methods
	void render(aGraphics * context);
	void update(float dt);
};

#endif