﻿#include "player.h"
#include "Object.h"
#include "Game.h"
#include "CameraManager.h"

extern CameraManager* gCameraManager;

Player::Player(string filename, int width, int height)
	:JUMP_HEIGHT(80)
{
	playerTexture = gGraphics->loadTexture("misc\\textures\\mario.bmp");
	if(!playerTexture)
		MessageBox(0, "Error loading player texture", 0, 0);		

	mWidth = width;
	mHeight = height;

	mHealth = HEALTH;
	mDamage = DAMAGE;

	faceDir = RIGHT;
	frame = 0;
	inair = true;
	mJumping = false;
	mOnGround = false;

	prevWallJumpID = 1337; 
	mWallJumpOk = false;
	mActivateKey = false;

	mAmmo = 0;
	mRightGunTexture = gGraphics->loadObjectTexture((char*)RIGHT_GUN_SOURCE.c_str());
	mLeftGunTexture = gGraphics->loadObjectTexture((char*)LEFT_GUN_SOURCE.c_str());
}

Player::~Player()
{
	// dtor
}

void Player::onLostDevice()
{
	// nothing to do?
}

void Player::onResetDevice(void)
{
	// nothing to do?
}

bool Player::update(double dt, Level *Level)
{
	static double dtsum = 0;
	static bool moving = false;
	static double dJump = 0;
	RECT tmpRect;
	double tmpX = mX;
	double tmpY = mY;

	// update frame
	if(dtsum >= .08)	{
		frame++;
		dtsum = 0;
	}
	else
		dtsum += dt;

	if(frame > 3 || !moving)
		frame = 0;

	moving = false;

	mDX = 0;
	mDY = 0;

	// fall
	mDY = dt*FALLSPEED;
	
	if(gDInput->keyPressed(DIK_W))	{
		if(mOnGround)
			jump(JUMP_HEIGHT);//jumped = true;
		else if(mWallJumpOk)	{
			jump(JUMP_HEIGHT);
			mWallJumpOk = false;
		}
	}

	if(mJumping)
	{
		// falls when max height is reached
		if(dJump <= -MAX_HEIGHT)	{
			mFalling = true;
			mJumping = false;
			dJump = 0;
		}
		else {
			mDY = -(double)JUMPSPEED*dt;
			dJump += mDY;
		}		
	}
	else
		dJump = 0;

	/* strafing */
	if(gDInput->keyDown(DIK_A))	{
		mDX = -dt*MOVESPEED;
		moving = true;
		faceDir = LEFT;
	}
	else if(gDInput->keyDown(DIK_D))	{
		mDX = dt*MOVESPEED;
		moving = true;
		faceDir = RIGHT;
	}

	/* activation key */
	if(gDInput->keyPressed(DIK_E))	{
		mActivateKey = true;
	}

	if(gDInput->keyPressed(DIK_SPACE) && mAmmo > 0)	{
		Bullet tmpBullet(mX, mY, 10, 10, faceDir, 50, 1, 200, PLAYER, (char*)BULLET_SOURCE.c_str());
		mBulletList.push_back(tmpBullet);
		mAmmo--;
	}

	/* update bullet list */
	std::list<Bullet>::iterator i = mBulletList.begin();
	while( i != mBulletList.end())
	{	
		if(!i->getErased())	{
			i->update(dt);
			++i;
		}
		
		else	{
			/* delete whats needed */
			i = mBulletList.erase(i);
		}
	}
	
	if(mShape.origin.x >= 616)
		gCameraManager->gameCamera()->addMovement(mDX, 0);

	move(mDX, mDY);
	
	if(!mOnGround)
		frame = 4;

	return true;//Level->collision(this);
}

void Player::draw(void)
{
	/* draw animation */
	RECT playerRect;//= getRect();	
	playerRect.left = mDrawX - mWidth/2;
	playerRect.right = mDrawX + mWidth/2;
	playerRect.top = mDrawY - mHeight/2;
	playerRect.bottom = mDrawY + mHeight/2;
	
	gGraphics->BlitAnimation(playerTexture, playerRect, 0xFFFFFFFF, 0, 0, frame, 0.0f, faceDir);

	/* draw the gun */
	if(mAmmo > 0)
	{
		RECT r;

		r.top = getY() + mShape.getAABB().bottom / 2;
		r.bottom = getY() + mShape.getAABB().bottom;

		if(faceDir == RIGHT)	{	
			r.left = getX() + mShape.getAABB().right / 2;
			r.right = getX() + mShape.getAABB().right + 20;
			gGraphics->BlitTexture(mRightGunTexture, r);	
		}
		else if(faceDir == LEFT)	{
			r.left = getX() - 20;
			r.right = getX() + mShape.getAABB().right / 2;
			gGraphics->BlitTexture(mLeftGunTexture, r);	
		}
	}

	// draw health
	char buffer[256];
	sprintf(buffer, "Health: %i", mHealth);
	gGraphics->drawText(buffer, 1180, 200);

	/* draw bullet list */
	std::list<Bullet>::iterator i = mBulletList.begin();
	while( i != mBulletList.end())
	{	
		i->draw();
		i++;
	}
}

RECT Player::getRect(void)
{	
	RECT rect;

	rect.left = mX - mWidth/2;
	rect.right = mX + mWidth/2;
	rect.top = mY - mHeight/2;
	rect.bottom = mY + mHeight/2;

	return rect;
}

void Player::move(double dx, double dy)
{
	mX += dx;
	mY += dy;

	// points are defined in local space, only need to move origin
	mShape.origin.x += dx;
	mShape.origin.y += dy;

	mDrawX = mX;
	mDrawY = mY;
}

double Player::getX(void)
{
	return mShape.origin.x;
}

double Player::getY(void) 
{
	return mShape.origin.y;
}

void Player::setXY(float x, float y)
{
	// find out how much to move the camera
	gCameraManager->gameCamera()->addMovement(x - mX, 0);

	mX = x;
	mY = y;

	mShape.origin.x = mX - mWidth/2;
	mShape.origin.y = mY - mHeight/2;
	
	mShape.addPoint(Shape::Point(0, 0));
	mShape.addPoint(Shape::Point(0, mHeight));
	mShape.addPoint(Shape::Point(mWidth, mHeight));
	mShape.addPoint(Shape::Point(mWidth, 0));
	
	mDrawX = mX;
	mDrawY = mY;
}

void Player::jump(int height)
{
	mJumping = true;
	MAX_HEIGHT = height;
}

void Player::testWallJump(int id)
{
 	if(prevWallJumpID != id)	{
		mWallJumpOk = true;
		prevWallJumpID = id;
	}
}

Shape* Player::getShape(void)							
{
	return &mShape;
}


bool Player::getActivateKey(void)
{
	return mActivateKey;
}

void Player::setActivateKey(bool b)
{
	mActivateKey = b;
}

void Player::lootGun(int ammo, int bulletType)
{
	mAmmo = ammo;
}