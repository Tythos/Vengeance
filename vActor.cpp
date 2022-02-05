/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Actor sprite class
	Begun Monday, June 7th, 2010

	The Actor class extends the sprite class, and encapsulates information and abilities specific to player objects
	moving around in the world, such as ghosts and pacman
*/

#include "vActor.h"
#include <math.h>

// --- Constructors --- //

vActor::vActor() : vSprite(NULL) {
	// Initialize as not-selected, not-scared
	isSelected = isScared = false;

	// Starts out alive
	isAlive = true;

	// Start at level 0
	level = 0;

	// Default size is 16x16 px; align by center
	setW(16.0f);
	setH(16.0f);
	setAlignment(true);

	// No initial AI or waypoint
	mode = AI_NONE;
	wayX = wayY = -1;

	// Default velocity is globally defined
	velocity = baseVelocity;
	
	// Initialize ability timestamp
	time(&abilityTriggered);
}

vActor::~vActor() {
	// Nothing to destroy
}

// --- Actor Retrievers --- //

float vActor::getVelocity() {
	return velocity;
}

bool vActor::getIsAlive() {
	// Returns alive / not alive status
	return isAlive;
}

bool vActor::getIsSelected() {
	// Returns selection status
	return isSelected;
}

bool vActor::getIsScared() {
	// Returns scared status
	return isScared;
}

int vActor::getLevel() {
	// Returns level of actor
	return level;
}

int vActor::getWayX() {
	// Returns x coordinate of waypoint
	return wayX;
}

int vActor::getWayY() {
	// Returns y coordinate of waypoint
	return wayY;
}

AiObjective vActor::getMode() {
	// Returns AI mode
	return mode;
}

time_t vActor::getAbilityTriggered() {
	return abilityTriggered;
}

// --- Actor Setters --- //

void vActor::setVelocity(float v) {
	velocity = v;
}

void vActor::setLife(bool l) {
	// Sets alive / not alive status
	isAlive = l;
}

void vActor::select() {
	// Only select this actor if it is alive
	if (isAlive) isSelected = true;
}

void vActor::deselect() {
	// Deselect actor
	isSelected = false;
}

void vActor::setScared(bool s) {
	// Sets scared status
	isScared = s;
}

void vActor::levelUp() {
	// Increases actor level
	level++;
}

void vActor::reset() {
	level = 0;
}

void vActor::setWaypoint(int x, int y) {
	// Sets 2d waypoint coordinate
	wayX = x;
	wayY = y;
}

void vActor::setMode(AiObjective m) {
	// Sets AI mode
	mode = m;
}

void vActor::setAbilityTriggered(time_t t) {
	abilityTriggered = t;
}

// --- Overridden Accessors --- //

void vActor::setTexture(aTexture * t) {
	// Set sprite (panel) texture
	tex = t;
}

void vActor::setType(spriteType t) {
	// Change type and adjust abilities, velocity accordingly
	type = t;
	switch (t) {
		case V_PACMAN:
			velocity = baseVelocity;
			break;
		case V_RED_G:
			velocity = 0.8f * baseVelocity;
			break;
		case V_BLUE_G:
			velocity = 0.8f * baseVelocity;
			break;
		case V_ORANGE_G:
			velocity = 0.8f * baseVelocity;
			break;
		case V_PINK_G:
			velocity = 0.8f * baseVelocity;
			break;
		default:
			velocity = 0.0f;
			break;
	}
}

// --- Methods --- //

void vActor::render(aGraphics * context) {
	// Override default panel / sprite rendering to draw selection box with same coordinates and override texture selection for 'scared' actors
	// Draw sprite to screen surface, if alive and visible
	if (isAlive && visible) {
		// Calculate location, position
		int screenWidth = context->getWidth();
		int screenHeight = context->getHeight();
		float centerX = x.unit == UNIT_PIX ? x.value / ((float)screenWidth) : x.value;
		float centerY = y.unit == UNIT_PIX ? y.value / ((float)screenHeight) : y.value;
		float left, right, bottom, top;
		switch (w.align) {
			case ALIGN_NEGATIVE:
				left = centerX;
				right = w.unit == UNIT_PIX ? centerX + w.value / (float)screenWidth : centerX + w.value;
				break;
			case ALIGN_POSITIVE:
				left = w.unit == UNIT_PIX ? centerX - w.value / (float)screenWidth : centerX - w.value;
				right = centerX;
				break;
			case ALIGN_MIDDLE:
			default:
				left = w.unit == UNIT_PIX ? centerX - 0.5f * w.value / (float)screenWidth : centerX - 0.5f * w.value;
				right = w.unit == UNIT_PIX ? centerX + 0.5f * w.value / (float)screenWidth : centerX + 0.5f * w.value;
				break;
		}
		switch (h.align) {
			case ALIGN_NEGATIVE:
				bottom = centerY;
				top = h.unit == UNIT_PIX ? centerY + h.value / (float)screenHeight : centerY + h.value;
				break;
			case ALIGN_POSITIVE:
				bottom = h.unit == UNIT_PIX ? centerY - h.value / (float)screenHeight : centerY - h.value;
				top = centerY;
				break;
			case ALIGN_MIDDLE:
			default:
				bottom = h.unit == UNIT_PIX ? centerY - 0.5f * h.value / (float)screenHeight : centerY - 0.5f * h.value;
				top = h.unit == UNIT_PIX ? centerY + 0.5f * h.value / (float)screenHeight : centerY + 0.5f * h.value;
				break;
		}

		// Draw 2d quad
		bgColor->setAll();

		// Draw textured panel
		tex->bind();
		int currType = isScared ? (int)V_SCARED_G : (int)type;
		float texLeft = spriteTex * (float)state;
		float texTop = spriteTex * (float)currType;
		float texRight = texLeft + spriteTex;
		float texBottom = texTop + spriteTex;
		glBegin(GL_QUADS); {
			glTexCoord2f(texLeft, texBottom);
			glVertex2f(left, bottom);
			glTexCoord2f(texRight, texBottom);
			glVertex2f(right, bottom);
			glTexCoord2f(texRight, texTop);
			glVertex2f(right, top);
			glTexCoord2f(texLeft, texTop);
			glVertex2f(left, top);
		} glEnd();
		tex->unbind();

		// Render selection box, if selected
		if (isSelected) {
			switch (type) {
				case V_RED_G:
					glColor3f(1.0f, 0.0f, 0.0f);
					break;
				case V_BLUE_G:
					glColor3f(0.0f, 1.0f, 0.871f);
					break;
				case V_ORANGE_G:
					glColor3f(1.0f, 0.722f, 0.278f);
					break;
				case V_PINK_G:
					glColor3f(1.0f, 0.722f, 0.871f);
					break;
				default:
					glColor3f(0.0f, 0.0f, 0.0f);
					break;
			}
			glBegin(GL_LINE_STRIP); {
				float margin = 0.005f;
				glVertex2f(left-margin, bottom-margin);
				glVertex2f(right+margin, bottom-margin);
				glVertex2f(right+margin, top+margin);
				glVertex2f(left-margin, top+margin);
				glVertex2f(left-margin, bottom-margin);
			} glEnd();
		}
	}
}

void vActor::update(float dt) {
	// Only update if actor is alive
	if (isAlive) {
		vSprite::update(dt);
	}
}