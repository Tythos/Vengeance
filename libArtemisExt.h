#ifndef ARTEMIS_EXTERNAL_H
#define ARTEMIS_EXTERNAL_H

// Libraries
#define _USE_MATH_DEFINES
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

// --- Utility --- //

enum Alignment { ALIGN_NEGATIVE, ALIGN_MIDDLE, ALIGN_POSITIVE };
enum ScreenUnit { UNIT_PIX, UNIT_PCT };

struct ScreenDimension {
	float value;
	Alignment align;
	ScreenUnit unit;
	ScreenDimension() {
		value = 0.0f;
		align = ALIGN_NEGATIVE;
		unit = UNIT_PIX;
	}
	float toPct(float size) {
		if (unit == UNIT_PIX) {
			return value / size;
		} else {
			return value;
		}
	}
	float toPix(float size) {
		if (unit == UNIT_PIX) {
			return value;
		} else {
			return value * size;
		}
	}
	float toPct(int size) {
		float s = (float)(size);
		return toPct(s);
	}
	float toPix(int size) {
		float s = (float)(size);
		return toPix(s);
	}
};

namespace Conversion {
	char * floatToStr(float myFloat);
	char * intToStr(int myInt);
}

class kString {
private:
	char * string;
	int length;
	bool isInit;
protected:
public:
	// Constructors
	kString();
	kString(const char * s);
	kString(const kString& s);
	kString(const int i);
	kString(const float f);
	~kString();

	// Accessors
	int getLength() { return length; }
	char operator[](int n);
	const char operator[](int n) const;
	char * raw();

	// Assignment
	kString& operator=(const kString &rhs);
	kString& operator=(const char* rhs);

	// Concatenation
	kString operator+=(const kString &rhs);
	const kString operator+(const kString &rhs);

	// Comparison
	bool operator==(const kString &rhs) const;
	bool operator!=(const kString &rhs) const;
};

class aColor {
public:
	float r;
	float g;
	float b;
	float a;
	
	aColor();
	aColor(float fr, float fg, float fb, float fa);
	void set(float fr, float fg, float fb, float fa);
	void setAll();
	void setColor();
	void setDiffuseMat();
	void setAmbientMat();
};

// --- Math --- //

class kVec {
private:
	float * vec;
	int len;
protected:
public:
	// Constructors
	kVec();
	kVec(int length);
	kVec(int length, float vals[]);
	kVec(kVec const& b);
	~kVec();
	
	// Access
	int getLength() { return len; }
	void set(int length, float vals[]);
	
	// Math
	float abs();
	
	// Operators
	friend ostream& operator <<(ostream &os, const kVec &myVec);
	friend ostream& operator <<(ostream &os, kVec *myVec);
	float& operator[](int i);
	float operator *(kVec b); // Dot product
	kVec operator %(kVec b);  // Cross product
	kVec operator +(kVec b);
	kVec operator -(kVec b);
	kVec operator *(float b);
	kVec operator /(float b);
	kVec operator =(kVec b);
};

class kMat {
private:
	kVec * mat;
	int r; int c;
protected:
public:
	// Constructors
	kMat();
	kMat(kMat const& b);
	kMat(kVec b);
	kMat(int size);
	kMat(int rows, int cols);
	kMat(int rows, int cols, float * vals);
	~kMat();
	
	// Access
	void set(int rows, int cols, float * vals);
	kVec toVec();
	kVec getRow(int row);
	kVec getCol(int col);
	
	// Math
	float norm();
	float trace();
	kMat I();
	kMat transpose();
	int rank();
	kMat invert();
	float det();
	
	// Operators
	friend ostream& operator <<(ostream &os, const kMat &myMat);
	friend ostream& operator <<(ostream &os, kMat *myMat);
	kVec& operator[](int i);
	kVec operator *(kVec b);
	kMat operator *(kMat b);
	kMat operator *(float b);
	kMat operator +(kMat b);
	kMat operator -(kMat b);
	kMat operator /(float b);
	kMat operator =(kMat b);
	kMat operator ^(int b);
};

class kQuat {
private:
protected:
	float scl;
	float vec[3];
	friend class kTrans;
public:
	// Constructors
	kQuat();
	kQuat(const kQuat &q);
	kQuat(float x, float y, float z);
	kQuat(float s, float i, float j, float k);
	~kQuat();
	
	// Operators
	kQuat operator+ (kQuat operand);
	kQuat operator- (kQuat operand);
	kQuat operator* (float operand);
	kQuat operator* (kQuat operand);
	kQuat operator= (kQuat operand);
	
	// Methods
	kQuat conj();
	kQuat inv();
	float mag();
	void normalize();
	void scale(kQuat scaleBy);
	void unscale(kQuat scaleBy);
	void set(float w, float x, float y, float z);
	void set(kQuat newQuat);
	void print();
	float getScl() { return scl; }
	float getVecI() { return vec[0]; }
	float getVecJ() { return vec[1]; }
	float getVecK() { return vec[2]; }
	
	// Rotation
	// Note the difference between OPERATIONAL and VALUED rotational quaternion,
	// and remember that some quaternions will be merely vectors being
	// transformed by other quaternions or matrices
	void convertValuedToOperational();
	void convertOperationalToValued();
	void reverseValuedRotation();
	void reverseOperationalRotation();
};

class kTrans {
private:
	kQuat * _pos; // Position of child frame. Only i, j, k are used; scl should always be 0
	kQuat * _rot; // Rotation of child frame about parent frame. Does NOT store a rotation quaternion, but rather the strict angle-and-vector rotation values
	kQuat * _scl; // Scaling of object along local axes
protected:
public:
	kTrans();
	~kTrans();
	void setPos(float x, float y, float z);
	void addPos(float x, float y, float z);
	void setRot(float w, float x, float y, float z);
	void addRot(float w, float x, float y, float z);
	void setScl(float x, float y, float z);
	void addScl(float x, float y, float z);
	kQuat getPos();
	kQuat getRot();
	kQuat getScl();
	kTrans operator= (kTrans operand);
	kQuat applyTransformation(kQuat subject);
	kQuat reverseTransformation(kQuat subject);
	void glApply();
	void glUnapply();
	void print();
	void update(kQuat * linearVelocity, kQuat * angularVelocity, float dt);
};

namespace numericMath {
	float getTolerance();
	void setTolerance(float value);
	int getIterLimit();
	void setIterLimit(int value);
	float bisectionRoot(float(*f)(float), float low, float high);
	float bisectionSolve(float(*f)(float), float low, float high, float target);
	float fmod(float a, float b);
	int nextpoweroftwo(int x);
	//int newRound(double x);
}

// --- Base --- //

class aVertex {
public:
	float x;
	float y;
	float z;
	
	aVertex();
	aVertex(float px, float py, float pz);
	void set(float px, float py, float pz);
	float mag();
	aVertex operator- (aVertex operand);
	bool operator== (aVertex operand);
	aVertex cross(aVertex operand);
	aVertex norm();
	void print();
};

class aTexCoord {
public:
	float u;
	float v;
};

class aMesh {
private:
	int _numVertices;
	aVertex * _vertices;
	aVertex * _normals;
	aTexCoord * _texCoords;
protected:
public:
	// Constructors
	aMesh();
	~aMesh();
	
	// Loaders
	bool loadCube(float size); // Loads vertices defining size x size x size cube, where size is the length of one side.
	bool loadTetra(float size); // Loads vertices defining size x size x size tetrahedral, where size is the length of one side
	bool loadSphere(float size, int numFaces); // Loads vertices defining sphere with radius size and the given number of faces
	
	// Methods
	void clear();
	void render();
	void setColor(float r, float g, float b, float a);
	void refreshNormals();
	void debug();
	void disableTexture();
};

class aTexture
{
private:
	GLuint _textureId;
	SDL_Surface * _texture;
	bool _isLoaded;
public:
	aTexture();
	~aTexture();
	bool loadFromFile(kString filename);
	bool loadFromSurface(SDL_Surface * surf);
	bool unload();
	bool isLoaded() { return _isLoaded; }
	bool bind();
	bool unbind();
	int getId() { return _textureId; }
};

class aObject {
private:
protected:
	kTrans * frame;
	kQuat * linearVelocity; // Defined in parent frame
	kQuat * angularVelocity; // Defined in parent frame, valued quaternion rotation
	aObject * parent;
	aObject * children;
	aObject * next;
	aColor * ambientMaterial;
	aColor * diffuseMaterial;
	aTexture * tex;
	aMesh * mesh;
public:
	// Data
	bool isVisible;

	// Constructors
	aObject(aObject * p = NULL);
	aObject(float x, float y, float z, aObject * p = NULL);
	aObject(kQuat orientation, aObject * p = NULL);
	aObject(float x, float y, float z, kQuat orientation, aObject * p = NULL);
	~aObject();
	void freeChildren();
	
	// Access
	kTrans getFrame();
	kQuat getLocation();
	kQuat getRotation();
	kQuat getLinearVelocity();
	kQuat getAngularVelocity();
	kQuat getScaling();
	
	// Color
	void setAmbient(float r, float g, float b);
	void setDiffuse(float r, float g, float b);
	
	void setTexture(kString filename);
	void setTexture(SDL_Surface * surf);
	void setScaling(float x, float y, float z);
	// TEMPORARY, FOR DEMO; end class will allow manipulation only through physics forces
	void setPosition(float x, float y, float z);
	void setRotation(float w, float x, float y, float z);
	void setLinearVelocity(float x, float y, float z);
	void setAngularVelocity(float w, float x, float y, float z);
		
	// Methods
	virtual void update(float dt, bool updateChildren = true);
	virtual void render(bool renderChildren = true);
	
	// Child manipulation
	void addChild(aObject * newChild);
	
	// Listing
	aObject * get(int n); // n < 0 returns first; n > length returns last
	void set(aObject * value, int n); // n > length and n < 0 sets last
	int listLength();
};

class aSkybox : public aObject
{
private:
	float radius;
	int resolution;
protected:
public:
	aSkybox();
	aSkybox(float r, int n);
	void setRadius(float r);
	void setResolution(int n);
};

class aCamera {
private:
	kQuat * position;
	kQuat * target;
	kQuat * up;
	float fieldAngle;
	float aspectRatio;
	float nearClip;
	float farClip;
	float singularityTolerance;
protected:
public:
	aCamera();
	~aCamera();
	aSkybox * skybox;
	
	// Set accessors
	void setPosition(float x, float y, float z);
	void setTarget(float x, float y, float z);
	void setUp(float x, float y, float z);
	void setRight(float x, float y, float z);
	void setFieldAngle(float v) { fieldAngle = v; }
	void setAspectRatio(float v) { aspectRatio = v; }
	void setNearClip(float v) { nearClip = v; }
	void setFarClip(float v);

	// Camera accessors
	float getFieldAngle() { return fieldAngle; }
	float getAspectRatio() { return aspectRatio; }
	float getNearClip() { return nearClip; }
	float getFarClip() { return farClip; }
	
	// Spacial accessors
	kQuat * getPosition() { return position; }
	kQuat * getTarget() { return target; }
	kQuat * getUp() { return up; }
	
	void applyCamera();
	void print();

	// Skybox (background) settings
	void setSkyboxTexture(kString filename);
	void setSkyboxTexture(SDL_Surface * surf);
	void setSkyboxResolution(int n);
	void renderSkybox();

	// Movement functions
	void rotateFocus(float dTht, float dPhi);
	void rotateCenter(float dTht, float dPhi);
	void pan(float dx, float dy, float dz);
	void focus(float x, float y, float z);
	void zoomFocus(float dz);
};

class aTypewriter {
private:
	TTF_Font * currFont;
	kString currFontName;
	aColor * color;
	ScreenDimension cursorX;
	ScreenDimension cursorY;
	ScreenDimension fontSize;
	int nextpoweroftwo(int x);
	
protected:
public:
	aTypewriter();
	~aTypewriter();
	bool setFont(kString fontName);
	bool setColor(float r, float g, float b);
	void setFontSize(float fs);
	float getFontSize() { return fontSize.value; }
	bool stamp(kString text);
	bool type(kString text, int contextWidth, int contextHeight);
	void moveCursor(ScreenDimension x, ScreenDimension y); // Moves cursor to given screen coordinates
};

class aGraphics {
private:
	int width;
	int height;
	int bpp;
	bool windowOk;
	bool isWindowed;
	SDL_Surface * screen;
	SDL_Color bgColor;
	//SDL_sem * screenLock;
protected:
public:
	aTypewriter * hTypewriter;

	aGraphics();
	~aGraphics();
	bool setScreen(int newHeight, int newWidth, int newBpp);
	bool declareSettings();
	bool toggleFullscreen();
	static void printAttributes();
	void resize(int width, int height);
	void swapBuffers();
	void clearScreen();

	// Accessors
	int getWidth() { return width; }
	int getHeight() { return height; }
	int getBpp() { return bpp; }
	bool getWindowed() { return isWindowed; }
	float getAspectRatio() { return float(width) / float(height); }

	// Rendering
	void testLoop();
	void drawCube(float xPos, float yPos, float zPos);
	void drawTexturedCube(float xPos, float yPos, float zPos);

	// Manage mode
	void go2d();
	void go3d();
	void go3d(aCamera * cam);
};

class aStopwatch {
private:
	// Reference only: not accurate enough for timers
	time_t currTime;
	time_t lastTime;
	int currTicks;
	int lastTicks;
	tm * currTimeInfo;
	long frame;
	float dt;
	// Timers only: too high of an overhead for frequent reference usage
protected:
public:
	// Reference only: not accurate enough for timers
	aStopwatch();
	void refresh();
	float getFramerate();
	float getDt() { return dt; }
	kString getDayText();
	kString getMonthText();
	int getDayOfWeek();
	int getDayOfMonth();
	int getDayOfYear();
	int getHour();
	int getMinute();
	int getSecond();
	int getYear();
	kString getFullTime();
	kString getTimestamp();
	// Timers only: too high of an overhead for frequent reference usage
};

enum eKeyState {AKEY_NONE, AKEY_NUM, AKEY_CAPS, AKEY_CTRL, AKEY_SHIFT, AKEY_ALT, AKEY_COMMAND, AKEY_WINDOWS};

class aKeyboard {
private:
	bool map[312];
	bool prevMap[312];
	SDL_Event event;
protected:
public:
	aKeyboard();
	void update();
	void update(SDL_Event eve);
	bool checkPressDown(char key);
	bool checkLiftUp(char key);
	bool checkPrevKey(char key);
	bool checkKey(char key);
	bool checkKey(SDL_Keycode aKey);
	eKeyState checkState();
};

enum aAppState {ASTATE_GLOBAL, ASTATE_MENU, ASTATE_INGAME, ASTATE_CINEMATIC};

class aEvent {
private:
protected:
public:
	aAppState eventState;
	bool (*trigger)(void);
	void (*target)(void);
	aEvent();
	~aEvent();
};

class eventList {
private:
	aEvent * thisEvent;
	eventList * nextNode;
protected:
public:
	eventList();
	~eventList();	// NOTE: When a list is deleted, elements are also freed.
	
	// NOTE: All functions assume that they are being called for the head node.
	int listLength();
	int listLength(aAppState myState);
	aEvent * getElement(int n);
	aEvent * createElement();
	void createElement(aAppState myState, bool (*myTrigger)(void), void (*myTarget)(void));
	int createElement(aEvent * newEvent);
	void checkEvents();
	void map();
};

class aPanel {
private:
protected:
	aPanel * next;		// Subsequent siblings
	aPanel * children;	// Panels rendered relative to this one
	aTexture * tex;
	aColor * bgColor;
	ScreenDimension x;
	ScreenDimension y;
	ScreenDimension w;
	ScreenDimension h;
	bool visible;
public:
	bool isTranslucent;

	// Constructors
	aPanel();
	aPanel(float nx, float ny, float nw, float nh);
	~aPanel();

	// Virtual functions
	virtual void update(float dt);
	virtual void render(aGraphics * context);
	virtual void setImage(kString filename);
	virtual void setImage(SDL_Surface * surf);
	
	// Listing
	aPanel * get(int n);
	void set(aPanel * value);
	int listLength();

	// Accessors
	ScreenDimension getX() { return x; }
	ScreenDimension getY() { return y; }
	ScreenDimension getW() { return w; }
	ScreenDimension getH() { return h; }
	bool getVisible() { return visible; }
	void setX(float v);
	void setY(float v);
	void setW(float v);
	void setH(float v);
	void setVisible(bool v);
	void setBgColor(float r, float g, float b, float a);

	// Collision Detection
	bool isWithin(ScreenDimension xCoord, ScreenDimension yCoord, aGraphics * context);
};

class aLight {
private:
	kQuat * position;	// Defined within parent frame; global frame by default
	kQuat * ambient;	// Color of ambient (non-directional) light
	kQuat * diffuse;	// Color of diffuse (directional) light
	aLight * next;
	float attenuation;	// Light attenuation factor
	int lightId;
	bool isEnabled;		// Lights are disabled by default
	bool isDirectional; // Directional lights produce parallel rays in the _position direction; non-directional lights are positional, where _position specifies the light location in the world
	bool isVisible;		// Visible lights are rendered as small points with the light's ambient color
	aMesh * lightObject;
protected:
public:
	// Constructors
	aLight();
	~aLight();
	
	// Access
	void enable();
	void disable();
	bool getStatus() { return isEnabled; }
	bool getVisible() { return isVisible; }
	void setVisible(bool v) { isVisible = v; }
	
	// Lighting
	void setAmbient(float r, float g, float b);
	void setDiffuse(float r, float g, float b);
	void setPosition(float x, float y, float z);
	void setAttenuation(float a) { attenuation = a; }
	void setDirectional() { isDirectional = true; }
	void setPositional() { isDirectional = false; }
	void refreshPosition();
	
	// List methods
	aLight * get(int n); // n < 0 returns first; n > length returns last
	void set(aLight * value, int n); // n > length and n < 0 sets last
	int listLength();
	
	int getGLLightNum(int n);
};

enum MouseState { NO_BUTTONS, LEFT_BUTTON, RIGHT_BUTTON, BOTH_BUTTONS };

class aMouse
{
private:
	int currX;
	int currY;
	int prevX;
	int prevY;
	MouseState currState;
	MouseState prevState;
protected:
public:
	aMouse(void);
	~aMouse(void);
	void update();
	void update(SDL_Event eve);

	int getCurrX() { return currX; }
	int getCurrY() { return currY; }
	int getPrevX() { return prevX; }
	int getPrevY() { return prevY; }
	int getDX() { return currX - prevX; }
	int getDY() { return currY - prevY; }
	ScreenDimension getXCoord();
	ScreenDimension getYCoord();
	MouseState getCurrState() { return currState; }
	MouseState getPrevState() { return prevState; }
};

enum ConsoleState { CS_UP, CS_DOWN, CS_MOVING_UP, CS_MOVING_DOWN };

struct cLine {
	kString contents;
	int ttl;
	cLine * next;
	cLine() {
		contents = "";
		next = NULL;
	}
	cLine(kString stuff) {
		contents = stuff;
		next = NULL;
	}
	~cLine() {
		if (next != NULL) {
			delete next;
			next = NULL;
		}
	}
	int numLines() {
		if (next == NULL) { return 1; }
		else { return next->numLines() + 1; }
	}
	cLine * get(int n) {
		if (n < 0) {
			// Negative index; return this
			return this;
		} else if (next == NULL && n > 0) {
			// Reached end; return last
			return this;
		} else if (n == 0) {
			// Reached desired location; return
			return this;
		} else {
			// Not there yet; increment
			return next->get(n-1);
		}
	}
};

class aConsole : public aPanel {
private:
	cLine * lines;
	ScreenDimension speed; // Speed at which console is animated {unit / sec)
	ScreenDimension limit; // Limit (from top of screen) of visible console
	kString font;
	aColor fontColor;
	float fontSize;
protected:
public:
	ConsoleState state;
	aConsole();
	~aConsole();
	void addLine(kString contents);
	kString getLine(int n);
	void render(aGraphics * context);
	void update(float dt);
	void setFont(kString f);
	void setFontColor(float r, float g, float b);
	void setFontSize(float pct);
};

class aCursor : public aPanel {
private:
	aMouse * mouse;
protected:
public:
	aCursor(aMouse * m);
	~aCursor(void);
	void render(aGraphics * context);
};

class aSoundboard {
private:
	bool isInit;
	int audioRate;
	int audioFormat;
	int numChannels;
	int bufferSize;
	int fadeLength; // Length of fade effects, in ms
	float volume;
protected:
public:
	aSoundboard();
	~aSoundboard();

	// Sound effects
	Mix_Chunk * loadSound(kString filename);
	void playSound(kString filename);
	void playSound(Mix_Chunk * sound);

	// Music songs
	Mix_Music * loadSong(kString filename);
	void playSong(kString filename);
	void playSong(Mix_Music * mus);
	void stopSong();
	bool isPlaying();
	bool repeatSongs;
};

class aApp {
private:
protected:
	aPanel * panels;
	aObject * origin;
	aLight * lights;
	kQuat * globalAmbient;
	ofstream debugFile;
public:
	// Data
	aGraphics * hGraphics;
	aStopwatch * hStopwatch;
	aKeyboard * hKeyboard;
	aMouse * hMouse;
	aAppState currState;
	eventList * gameEvents;
	aCamera * camera;
	aConsole * hConsole;
	aCursor * hCursor;
	aSoundboard * hSoundboard;
	bool isDebugToFile;
	bool isDebugToConsole;
	bool isLooping;
	bool (*externalRender)(void);
	
	// Structors
	aApp();
	~aApp();
	
	// Functions
	void initialize(int height, int width);
	void execute();
	void terminate();
	void mainLoop();
	void update();
	void render();
	void debug(kString msg);
	
	// Origin management
	void setOriginVisibility(bool isVisible);

	// List management
	void addPanel(aPanel * p);
	void addObject(aObject * o);
	void addLight(aLight * l);
	
	// Global settings
	void setGlobalAmbient(float r, float g, float b);
};

// --- Implementation --- //

enum ButtonState { AB_DISABLED, AB_NORMAL, AB_OVER, AB_DOWN };

class aButton :	public aPanel {
private:
	SDL_Surface * images[4];
	ButtonState prevState;
	ButtonState nextState;
	bool isSingular;
	void (*target)(void);
protected:
public:
	aButton(void);
	~aButton(void);
	bool isEnabled();
	bool getIsSingular();
	void setState(ButtonState state);
	ButtonState getState() { return prevState; }
	void setImage(ButtonState state, SDL_Surface * image);
	void render(aGraphics * context);
	void update(float dt);
};

class aCube : public aObject {
private:
	float size;
protected:
public:
	// Structors
	aCube();
	virtual ~aCube();

	// Accessors
	float getSize() { return size; }
	void setSize(float v);
};

struct dataPoint {
	float x;
	float y;
	dataPoint * next;

	dataPoint() {
		x = 0.0f;
		y = 0.0f;
		next = NULL;
	}

	dataPoint(float nx, float ny) {
		x = nx;
		y = ny;
		next = NULL;
	}

	dataPoint(const dataPoint &operand) {
		// Deep copy
		x = operand.x;
		y = operand.y;
		if (operand.next != NULL) {
			next = new dataPoint(*(operand.next));
		} else {
			next = NULL;
		}
	}

	dataPoint(int n) {
		// Create numerical function with n points (n,0)
		if (n < 1) {
			return;
		} else if (n == 1) {
			x = (float)n;
			y = 0.0f;
			next = NULL;
			return;
		} else {
			x = (float)n;
			y = 0.0f;
			next = new dataPoint(n-1);
			return;
		}
	}

	~dataPoint() {
		if (next != NULL) {
			delete next;
			next = NULL;
		}
	}

	dataPoint get(int n) {
		dataPoint toReturn;
		if (n == 0) {
			toReturn.x = x;
			toReturn.y = y;
		} else if (next == NULL) {
			toReturn.x = 0.0f;
			toReturn.y = 0.0f;
		} else {
			return next->get(n-1);
		}
		toReturn.next = NULL;
		return toReturn;
	}

	float get(float nx) {
		// Linear interpolation
		if (nx < x || next == NULL) {
			return y;
		} else if (x < nx && nx < next->x) {
			return y + (next->y - y) * (nx - x) / (next->x - x);
		} else {
			return next->get(nx);
		}
	}

	void set(float nx, float ny) {
		// Creates new point (nx,ny) (if nx is not present)
		if (x == nx) {
			// This value of x is already here; replace
			y = ny;
			return;
		} else if (x < nx) {
			if (next == NULL) {
				// Reached end; add new point
				next = new dataPoint(nx, ny);
			} else {
				// More points; continue
				next->set(nx, ny);
			}
		} else {
			// nx < x; insert
			dataPoint * insert = new dataPoint(x, y);
			x = nx; y = ny;
			insert->next = next;
			next = insert;
		}
	}

	void set(int n, float ny) {
		// Sets y value at point n to ny
		if (n == 0) {
			// Set value here
			y = ny;
		} else if (next == NULL) {
			// Index too big; insert here at end
			next = new dataPoint(x + 1.0f, ny);
		} else {
			// Not finished yet; continue
			next->set(n-1, ny);
		}
	}

	int length() {
		if (next == NULL) {
			return 1;
		} else {
			return next->length() + 1;
		}
	}
};

class aGraph : public aPanel {
private:
	aColor * fgColor;
	float margin;
	kString * title;
protected:
public:
	dataPoint * data;
	aGraph(kString t);
	~aGraph();
	void addValue(float x, float y);
	void render(aGraphics * context);
	void setFgColor(float r, float g, float b);
	float getMargin() { return margin; }
	void setMargin(float m);
	void setData(dataPoint * d);
};

#endif