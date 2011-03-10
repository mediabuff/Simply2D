#ifndef OBJECT_H
#define OBJECT_H

#include "constants.h"
#include "d3dUtil.h"
#include "Shape.h"
#include "Level.h"
#include "MTV.h"
//#include "Player.h"
//#include "Events.h"

class Player;
class Event;
//class MTV;

class Object
{
public:
	Object(float x, float y, int width, int height, char *textureSource, ObjectType type, bool updates = false);
	virtual ~Object();

	virtual void draw(void);
	virtual void drawPath(void)	{};
	virtual void OnEvent(Event *e)	{};

	Shape* getShape(void)							{return &mShape;};
	double getX(void);
	double getY(void);
	double getHeight(void);
	double getWidth(void);
	int getID(void)												{return mID;};
	int getType(void)											{return mType;};
	RECT getRect(void);	
	char* getTextureSource(void)								{return mTextureSource;};

	void setID(int ID) {mID = ID;};
	void setTexture(IDirect3DTexture9* texture)					{mTexture = texture;};
	void setTextureSource(char *source);
	IDirect3DTexture9 *getTexture(void)							{return mTexture;};

	virtual void editorMove(float dx, float dy);
	virtual void move(float dx, float dy);
	virtual void scale(direction side, int dwidth, int dheight);
	virtual void setXY(float x, float y)						{mX = x; mY = y;};

	void setWidth(int width)									{mWidth = width;};
	void setHeight(int height)									{mHeight = height;};

	virtual void update(float dt)								{};

	virtual void saveToFile(std::ofstream *fout);
	virtual void loadFromFile(std::ofstream *fout);
	bool getResizeable(void)									{return resizeable;};
	void setResizeable(bool b)									{resizeable = b;};

	// new 
	virtual void onPlayerCollision(Player *player, MTV mtv)				{};

	bool doUpdate(void) 										{return mUpdates;};
	void setStatic(bool b)										{mStatic = b;};
	bool isStatic(void)											{return mStatic;};

private:
	IDirect3DTexture9* mTexture;
	char *mTextureSource;

	Shape mShape;

	float mX;
	float mY;
	int mWidth;
	int mHeight;
	int mID;
	ObjectType mType;
	bool resizeable;
	bool mUpdates;
	bool mStatic;
};

#endif