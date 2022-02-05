/*	Brian Kirkpatrick
	Pac-Man: Vengeance
	Dice class
	Begun Monday, May 3rd, 2010

	The dice class enables easy access to the rand.h qRNG in several modes and ranges
*/

#include "Dice.h"

// --- Constructors --- //

Dice::Dice() {
	// Initialize with time-based random seed and roll once
	srand((unsigned)time(0));
	float f = roll();
}

Dice::~Dice() {
	// Nothing to destroy
}

// --- Methods --- //

float Dice::roll() {
	// Return random float between 0 and 1 (limited resolution)
	return (float)rand() / RAND_MAX;
}

int Dice::rollInt(int sides){
	// Return random integer between 0 and sides-1
	return rand() % sides;
}

int Dice::rollIntRange(int low, int high) {
	// Return random integer between low and high, inclusive
	if (low >= high) high = low + 1;
	int randomInt = rand();
	int moddedInt = randomInt % (high - low + 1);
	return moddedInt + low;
}

float Dice::rollFloat(float limit) {
	// Return random float between 0 and limit; limited resolution
	return roll() * limit;
}

float Dice::rollFloatRange(float low, float high){
	// Return random float between low and high; limited resolution
	if (low >= high) {
		high = low + 1;
	}
	return low + roll() * (high - low);
}
