/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Sprite class
	Begun Monday, May 3rd, 2010

	The sprite class, extending the panel class, represents a moving object within the maze with a specific state
	and a type that determine which segment of the universal texture map will be used to render the 2d element.
*/

#include "vSprite.h"
#include "Dice.h"

// --- Constructors --- //

vSprite::vSprite(aTexture * texture) {
	ScreenDimension sd;
	sd.align = ALIGN_NEGATIVE;
	sd.unit = UNIT_PIX;
	sd.value = spriteDim;
	w = sd; h = sd;
	tex = texture;
	type = V_PACMAN;
	state = SS_NA;
	xVel = 0.0f;
	yVel = 0.0f;
	Dice die = Dice();
	timeSeed = die.rollFloat(1.0f);
}

vSprite::~vSprite() {
}

// --- Retrievers --- //

float vSprite::getVelX() {
	return xVel;
}

float vSprite::getVelY() {
	return yVel;
}

spriteState vSprite::getState() {
	return state;
}

spriteType vSprite::getType() {
	return type;
}

// --- Setters --- //

void vSprite::setAlignment(bool isCentered) {
	if (isCentered) {
		h.align = ALIGN_MIDDLE;
		w.align = ALIGN_MIDDLE;
	} else {
		h.align = ALIGN_NEGATIVE;
		w.align = ALIGN_NEGATIVE;
	}
}

void vSprite::setState(spriteState s) {
	state = s;
	if (state <= SS_NA) {
		xVel = 0.0f;
		yVel = 0.0f;
	} else if (state <= SS_UP3) {
		xVel = 0.0f;
		yVel = baseVelocity;
	} else if (state <= SS_DOWN3) {
		xVel = 0.0f;
		yVel = -baseVelocity;
	} else if (state <= SS_LEFT3) {
		xVel = -baseVelocity;
		yVel = 0.0f;
	} else if (state <= SS_RIGHT3) {
		xVel = baseVelocity;
		yVel = 0.0f;
	}
}

void vSprite::setTimeSeed(float t) {
	timeSeed = t;
}

void vSprite::setVelX(float v) {
	xVel = v;
}

void vSprite::setVelY(float v) {
	yVel = v;
}

// --- Overridden & Virtual Methods --- //

void vSprite::render(aGraphics * context) {
	// Draw image to screen surface
	if (!visible) { return; }

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
	float texLeft = spriteTex * (float)state;
	float texTop = spriteTex * (float)type;
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
}

void vSprite::setType(spriteType t) {
	type = t;
}

void vSprite::update(float dt) {
	// Update position based on velocity
	x.value += xVel * dt;
	y.value += yVel * dt;

	// Update timeseed
	timeSeed += dt;
	while (timeSeed >= animationPeriod) {
		timeSeed -= animationPeriod;
	}

	// Update sprite offset based on timeseed
	if (state != SS_NA) {
		int phase = (state - 1) % 3;
		int offset = (state - 1) / 3;
		if (timeSeed < animationPeriod * 0.5f) {
			if (timeSeed < animationRatio * 0.5f * animationPeriod) {
				phase = 0;
			} else {
				phase = 1;
			}
		} else {
			if (timeSeed - 0.5f * animationPeriod < animationRatio * 0.5f * animationPeriod) {
				phase = 2;
			} else {
				phase = 1;
			}
		}
		state = (spriteState)(phase + offset * 3 + 1); 
	}
}

// --- Methods --- //

void vSprite::moveToPix(int px, int py) {
	x.value = (float)px; x.unit = UNIT_PIX; x.align = ALIGN_MIDDLE;
	y.value = (float)py; y.unit = UNIT_PIX; y.align = ALIGN_MIDDLE;
}

