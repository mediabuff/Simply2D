#include "MovingPlatform.h"

MovingPlatform::MovingPlatform(float x, float y, int width, int height, char *textureSource, POINT startPos, POINT endPos, Player *player,  movingType moveType, float speed)
	:DynamicObject(x, y, width, height, textureSource, MOVING_PLATFORM, player)
{
	mStartPos = startPos;
	mEndPos = endPos;
	mSpeed = speed;

	mGoalDir = END;
	mMoveType = moveType;

	// m�ste ta reda p� om endPos �r st�rre �n startPos
}

MovingPlatform::~MovingPlatform()
{
	// dtor
}

void MovingPlatform::update(float dt)
{
	// flytta plattform
	if(mMoveType == HORIZONTAL)
	{
		if(mGoalDir == END)
		{
			if(getX() < mEndPos.x)	{
				move(mSpeed, 0);
				
				if(getPlayer() != NULL)	{
					if(getPlayerCollision())
						movePlayer(mSpeed, 0);
				}
			}
			else
				mGoalDir = START;
		}
		else if(mGoalDir == START)
		{
			if(getX() > mStartPos.x)	{
				move(-mSpeed, 0);

				if(getPlayer() != NULL)	{
					if(getPlayerCollision())
						movePlayer(-mSpeed, 0);
				}
			}
			else 
				mGoalDir = END;
		}
	}

	/*if(getPlayerCollision())	{
		mPlayer->move(
	}*/
}

void MovingPlatform::draw(void)
{
	// Beh�ver inte mer avancerad draw
	Object::draw();
}