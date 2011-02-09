#include "GameWorld.h"
#include "StaticPlatform.h"
#include "Object.h"
#include "MovingPlatform.h"
#include "Enemy.h"
using namespace std;

/*GameWorld::GameWorld()
{
	nextStaticID = 0;
	nextDynamicID = 0;

	createLevel();

	collisionText = new char[256];

	mPlayer = NULL;
}*/

GameWorld::GameWorld(Player *player)
{
	nextStaticID = 0;
	nextDynamicID = 0;	

	collisionText = new char[256];

	mPlayer = player;

	char buffer[256];
	strcpy(buffer, ACTIVE_LEVEL.c_str());
	loadLevel(buffer);

	if(mPlayer != NULL)
		spawnPlayer();
}

GameWorld::~GameWorld()
{
	// rensa upp listan
	for (int i = 0;i < mStaticObjectList.size();i++)
	{
		delete mStaticObjectList[i];
	}

	// samma f�r dynamic listan!
	for (int i = 0;i < mMovingObjectList.size();i++)
	{
		delete mMovingObjectList[i];
	}
}

void GameWorld::saveLevel(char* levelFile)
{
	ofstream fout;
	char buffer[256];
	int width, height;
	
	fout.open(levelFile);

	
	fout << (int)spawnPos.x << " " << (int)spawnPos.y << endl;

	for (int i = 0;i < mStaticObjectList.size();i++)
	{
		mStaticObjectList[i]->saveToFile(&fout);
	}

	for (int i = 0;i < mMovingObjectList.size();i++)
	{
		
		mMovingObjectList[i]->saveToFile(&fout);
	}

	fout.close();	
}

void GameWorld::loadLevel(char* levelFile)
{	
	// rensa upp s� leveln �r tom innan man laddar in nya object!
	if(mStaticObjectList.size() > 0)	
		mStaticObjectList.clear();

	// samma f�r dynamic listan!
	if(mMovingObjectList.size() > 0)
		mMovingObjectList.clear();
	char buffer[256];

	Object *loadedObject;
	ObjectType type;
	char *text = new char[256];
	char *textureSource = new char[256];

	int tmpType, height, width, xpos, ypos;

	ifstream fin;
	fin.open(levelFile);
	int lines = 0;

	while(!fin.eof())	{
		//MessageBox(0, "sDA", 0, 0);
		fin.getline(text, 256);
		lines++;
	}
	fin.close();
	fin.open(levelFile);
	lines -= 2;

	fin >> spawnPos.x >> spawnPos.y;

	for(int j = 0; j<lines;j++)
	{
		fin >> tmpType;
		
		if(tmpType == STATIC_PLATFORMA)	
		{
			fin >> xpos >> ypos >> width >> height >> textureSource;	
			sprintf(buffer, "xpos: %i, ypos: %i, width: %i, height: %i, source: %s", xpos, ypos, width, height, textureSource);
			//MessageBox(0, buffer, 0, 0);
			loadedObject = new StaticPlatform(xpos, ypos, width, height, textureSource);
			addStaticObject(loadedObject);
		}
		else if(tmpType == MOVING_PLATFORM)
		{
			POS startPos, endPos;
			float speed;
			fin >> xpos >> ypos >> startPos.x >> startPos.y >> endPos.x >> endPos.y >> width >> height >> speed >> textureSource;
			//sprintf(buffer, "xpos: %i, ypos: %i, startPos.x: %i, startPos.y: %i, endPos.x: %i, endPos.y: %i, width: %i, height: %i, speed: %f , source: %s", xpos, ypos, startPos.x,
				//startPos.y, endPos.x, endPos.y, width, height, speed, textureSource);
			loadedObject = new MovingPlatform(xpos, ypos, width, height, textureSource, startPos, endPos, mPlayer, HORIZONTAL, speed);
			addDynamicObject(dynamic_cast<MovingObject*>(loadedObject));
		}
		else if(tmpType == NORMAL_ENEMY)
		{
			POS startPos, endPos;
			float speed;
			int health, damage;

			fin >> xpos >> ypos >> startPos.x >> startPos.y >> endPos.x >> endPos.y >> width >> height >> speed >> health >> damage >> textureSource;
			loadedObject = new Enemy(xpos, ypos, width, height, textureSource, startPos, endPos, mPlayer, HORIZONTAL, speed, health, damage);
			addDynamicObject(dynamic_cast<Enemy*>(loadedObject));
		}		
	}

	fin.close();
}

void GameWorld::addStaticObject(Object *object)
{
	if(object != NULL)	{
		object->setID(nextStaticID);
		mStaticObjectList.push_back(object);
		nextStaticID++;
	}
	else
		MessageBox(0, "Null object", 0, 0);
}
void GameWorld::addDynamicObject(MovingObject *object)
{
	if(object != NULL)	{
		object->setID(nextDynamicID);
		mMovingObjectList.push_back(object);
		nextDynamicID++;
	}
	else
		MessageBox(0, "Null object", 0, 0);
}

void GameWorld::deleteStaticObject(int ID)
{	
	//eh 
	int i = 0;
	vector<Object*>::iterator itr =  mStaticObjectList.begin();
	while(itr != mStaticObjectList.end() && i < mStaticObjectList.size())
	{
		if(mStaticObjectList[i]->getID() == ID)
		{
			delete mStaticObjectList[i];		// viktigt att deleta innan!
			itr = mStaticObjectList.erase(itr);			
			break;
		}
		else	{
			itr++;
			i++;
		}
	}
	nextStaticID -=1;
}

void GameWorld::deleteDynamicObject(int ID)
{	
	//eh 
	int i = 0;
	vector<MovingObject*>::iterator itr =  mMovingObjectList.begin();
	while(itr !=  mMovingObjectList.end() && i <  mMovingObjectList.size())
	{
		if(mMovingObjectList[i]->getID() == ID)
		{
			delete  mMovingObjectList[i];		// viktigt att deleta innan!
			itr =  mMovingObjectList.erase(itr);			
			break;
		}
		else	{
			itr++;
			i++;
		}
	}
	nextDynamicID -=1;
}


void GameWorld::updateLevel(double dt)	
{
	// update player
	mPlayer->update(dt, this);
	// update enemies
	// update entities
	for (int i = 0;i < mMovingObjectList.size();i++)
	{
		mMovingObjectList[i]->update(dt);

		// remove killed enemies from the list
		if(mMovingObjectList[i]->getType() == NORMAL_ENEMY)	{
			Enemy *tmpEnemy = dynamic_cast<Enemy*>(mMovingObjectList[i]);
			if(!tmpEnemy->getAlive())
				deleteDynamicObject(tmpEnemy->getID());
		}
	}
}

void GameWorld::drawLevel(void)
{
	if(!gGraphics)
		MessageBox(0, "NULL PTR", 0, 0);

	if(mPlayer != NULL)
		mPlayer->draw();

	// static
	for (int i = 0;i < mStaticObjectList.size();i++)
	{
		mStaticObjectList[i]->draw();
	}

	// moving
	for (int i = 0;i < mMovingObjectList.size();i++)
	{
		mMovingObjectList[i]->draw();
	}
	// drawEnemies
}

CollisionStruct GameWorld::collision(Player *player)
{
	//CollisionDirection collisionDirection = NONE_COLLISION;
	CollisionStruct collisions;
	collisions.hori = false;
	collisions.vert = false;

	RECT tmpRect;
	bool collision = false;
	RECT objectRect;
	RECT playerRect = player->getRect();

	for (int i = 0;i < mStaticObjectList.size();i++)
	{
		// hitta kolliderande object
		objectRect = mStaticObjectList[i]->getRect();

		// inte p� marken -> fall
		if(!(playerRect.top >= objectRect.bottom ||  playerRect.bottom + 1<= objectRect.top ||  playerRect.right <= objectRect.left ||  playerRect.left >= objectRect.right))
		{
			collisions.vert = true;
		}

		// spelaren har r�rt p� sig
		if(player->getDX() < 0)
		{
			if(!(playerRect.top >= objectRect.bottom ||  playerRect.bottom <= objectRect.top ||  playerRect.right <= objectRect.left ||  playerRect.left + player->getDX() >= objectRect.right))
			{
				collisions.hori = true;
			}
		}
		else if(player->getDX() > 0)
		{
			if(!(playerRect.top >= objectRect.bottom ||  playerRect.bottom <= objectRect.top ||  playerRect.right + player->getDX() <= objectRect.left ||  playerRect.left >= objectRect.right))
			{
				collisions.hori = true;
			}
		}
		
		if(player->getDY() < 0)
		{
			if(!(playerRect.top + player->getDY() >= objectRect.bottom ||  playerRect.bottom <= objectRect.top ||  playerRect.right <= objectRect.left ||  playerRect.left >= objectRect.right))
			{
				collisions.vert = true;
			}
		}
		else if(player->getDY() > 0)
		{
			if(!(playerRect.top >= objectRect.bottom ||  playerRect.bottom  + player->getDY() <= objectRect.top ||  playerRect.right <= objectRect.left ||  playerRect.left >= objectRect.right))
			{
				collisions.vert = true;
			}
		}

	}

	for (int i = 0;i < mMovingObjectList.size();i++)
	{
		// hitta kolliderande object
		objectRect = mMovingObjectList[i]->getRect();

		// inte p� marken -> fall
		if(!(playerRect.top >= objectRect.bottom ||  playerRect.bottom + 1<= objectRect.top ||  playerRect.right <= objectRect.left ||  playerRect.left >= objectRect.right))
		{
			collisions.vert = true;
		}

		// spelaren har r�rt p� sig
		if(player->getDX() < 0)
		{
			if(!(playerRect.top >= objectRect.bottom ||  playerRect.bottom <= objectRect.top ||  playerRect.right <= objectRect.left ||  playerRect.left + player->getDX() >= objectRect.right))
			{
				collisions.hori = true;
			}
		}
		else if(player->getDX() > 0)
		{
			if(!(playerRect.top >= objectRect.bottom ||  playerRect.bottom <= objectRect.top ||  playerRect.right + player->getDX() <= objectRect.left ||  playerRect.left >= objectRect.right))
			{
				collisions.hori = true;
			}
		}
		
		if(player->getDY() < 0)
		{
			if(!(playerRect.top + player->getDY() >= objectRect.bottom ||  playerRect.bottom <= objectRect.top ||  playerRect.right <= objectRect.left ||  playerRect.left >= objectRect.right))
			{
				collisions.vert = true;
			}
		}
		else if(player->getDY() > 0)
		{
			if(!(playerRect.top >= objectRect.bottom ||  playerRect.bottom  + player->getDY() <= objectRect.top ||  playerRect.right <= objectRect.left ||  playerRect.left >= objectRect.right))
			{
				collisions.vert = true;
			}
		}

	}

	return collisions;
}

Object* GameWorld::getObjectAt(POINT mpos)
{
	Object *tmp = NULL;
	RECT objectRect;

	for (int i = 0;i < mStaticObjectList.size();i++)
	{
		objectRect = mStaticObjectList[i]->getRect();

		if(mpos.x > objectRect.left && mpos.x < objectRect.right && 
			mpos.y > objectRect.top && mpos.y < objectRect.bottom)
			tmp = mStaticObjectList[i];
	}
	// tr�ck inte p� n�tt statiskt, prova dynamiskt!
	if(tmp == NULL)
	{
		for (int i = 0;i < mMovingObjectList.size();i++)
		{
			objectRect = mMovingObjectList[i]->getRect();

			if(mpos.x > objectRect.left && mpos.x < objectRect.right && 
				mpos.y > objectRect.top && mpos.y < objectRect.bottom)
				tmp = mMovingObjectList[i];
		}
	}

	// samma sak f�r dynamic listan!
	return tmp;
}

Object* GameWorld::objectIntersection(Object *activeObject)
{
	RECT objectRect;
	RECT activeObjectRect = activeObject->getRect();
		// returnerar true om det blir en intersect
		for (int i = 0;i < mStaticObjectList.size();i++)
		{		
			objectRect = mStaticObjectList[i]->getRect();
			if(activeObjectRect.top <= objectRect.bottom && activeObjectRect.bottom >= objectRect.top && activeObjectRect.right >=objectRect.left &&
				activeObjectRect.left <= objectRect.right && activeObject->getID() != mStaticObjectList[i]->getID())
			{
				return mStaticObjectList[i];
			}
		}
	
	return NULL;
}

void GameWorld::drawEditorLevel(void)
{
	if(!gGraphics)
		MessageBox(0, "NULL PTR", 0, 0);

	// statiska
	for (int i = 0;i < mStaticObjectList.size();i++)
	{
		mStaticObjectList[i]->draw();
	}

	// dynamiska
	for (int i = 0;i < mMovingObjectList.size();i++)
	{
		mMovingObjectList[i]->draw();
		mMovingObjectList[i]->drawPath();
	}
}

void GameWorld::spawnPlayer(void)
{
	mPlayer->setXY(spawnPos.x, spawnPos.y);
}

RECT GameWorld::getSpawnRect(void)
{
	RECT tmpRect;

	tmpRect.left = spawnPos.x - USER_WIDTH/2;
	tmpRect.right = spawnPos.x + USER_WIDTH/2;
	tmpRect.top = spawnPos.y - USER_HEIGHT/2;
	tmpRect.bottom = spawnPos.y + USER_HEIGHT/2;

	return tmpRect;
}

void GameWorld::moveWorld(float dx, float dy)
{	
	// should also have one for dY, but not needed right now. only scrolls sidways currently

	if(mPlayer->getPos().x > 520)
		{
			// statiska
			for (int i = 0;i < mStaticObjectList.size();i++)
			{
				mStaticObjectList[i]->move(dx, dy);
			}

			// dynamiska
			for (int i = 0;i < mMovingObjectList.size();i++)
			{
				mMovingObjectList[i]->move(dx, dy);
			}
		}
}

