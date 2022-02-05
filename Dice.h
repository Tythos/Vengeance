/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Dice class
	Begun Monday, May 3rd, 2010

	The dice class enables easy access to the rand.h qRNG in several modes and ranges
*/

#ifndef DICE_CLASS_H
#define DICE_CLASS_H

#include <stdlib.h>
#include <time.h>

class Dice {
private:
protected:
public:
	// Constructors
	Dice();
	~Dice();

	// Methods
	float roll();								// 0-1
	int rollInt(int sides);						// Exclusive
	int rollIntRange(int low, int high);		// Inclusive
	float rollFloat(float limit);				// Exclusive; limited resolution
	float rollFloatRange(float low, float high);// Inclusive; limited resolution
};

#endif