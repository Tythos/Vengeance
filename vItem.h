/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Maze item class
	Begun Wednesday, May 19th, 2010

	The Item class extends the Sprite class, and defines items within the maze that are consumed by pacman for a
	specific point value
*/

#ifndef VENGEANGE_ITEM_H
#define VENGEANCE_ITEM_H

#include "vSprite.h"

// Item type: integer equivalents correspond to row in texture map
enum itemType { IT_SMALL_DOT, IT_LARGE_DOT, IT_BANANA, IT_PEAR, IT_APPLE, IT_PRETZEL, IT_PEACH, IT_STRAWBERRY, IT_CHERRIES };

class vItem : public vSprite {
private:
	// Data
	bool isConsumed;
	int xSquare;
	int ySquare;
	itemType it;
protected:
public:
	// Constructors
	vItem();
	vItem(int x, int y);
	~vItem();

	// Accessors
	bool getIsConsumed();
	int getPointValue();
	itemType getItemType();
	void specifyTexture(aTexture * t);
	void setItemType(itemType i);

	// Methods
	int consume();
	void disable();
	virtual void render(aGraphics * context);
	virtual void update(float dt);
};

#endif
