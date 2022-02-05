/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Maze item class
	Begun Wednesday, May 19th, 2010

	The Item class extends the Sprite class, and defines items within the maze that are consumed by pacman for a
	specific point value
*/

#include "vItem.h"

// --- Constructors --- //

vItem::vItem() : vSprite(NULL) {
	// Default type is small dot; all items are V_CONSUMABLE
	type = V_CONSUMABLE;
	it = IT_SMALL_DOT;

	// Initialize item in unseen corner (-1,-1)
	xSquare = -1;
	ySquare = -1;

	// Initially unconsumed
	isConsumed = false;
}

vItem::vItem(int x, int y) : vSprite(NULL) {
	// Default type is small dot; all items are V_CONSUMABLE
	type = V_CONSUMABLE;
	it = IT_SMALL_DOT;

	// Initialize to given location
	xSquare = x;
	ySquare = y;

	// Initially unconsumed
	isConsumed = false;
}

vItem::~vItem() {
	// Nothing to destroy
}

// --- Accessors --- //

bool vItem::getIsConsumed() {
	// Returns consumed / non-consumed state
	return isConsumed;
}

int vItem::getPointValue() {
	// Returns point value of item, which increases by 10 for each increase in item level
	return isConsumed ? 0 : 10 * ((int)(it) + 1);
}

itemType vItem::getItemType() {
	// Returns item type
	return it;
}

void vItem::specifyTexture(aTexture * t) {
	// Sets texture address
	tex = t;
}

void vItem::setItemType(itemType i) {
	// Sets item type (different from sprite type)
	it = i;
}

// --- Methods --- //

int vItem::consume() {
	// Set item to consumed, disable coordinates, and return point value
	int toReturn = getPointValue();
	disable();
	return toReturn;
}

void vItem::disable() {
	// Hides item by setting 'consumed' flag to true and changing coordinates
	isConsumed = true;
	xSquare = -1;
	ySquare = -1;
}

void vItem::render(aGraphics * context) {
	// Set item dimensions and tweak spriteState by casting itemType (to render correct item texture) before rendering
	this->setW(20.0f);
	this->setH(20.0f);
	state = (spriteState)(it);
	vSprite::render(context);
}

void vItem::update(float dt) {
	// Items don't need to be updated, so don't waste the time
	return;
}